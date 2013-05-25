/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "pool.h"
#include "php_riak.h"
#include <php.h>
#include <riack.h>

zend_bool ensure_connected(riak_connection *connection TSRMLS_DC) /* {{{ */
{
  zend_bool result = 1;
  if (difftime(time(NULL), connection->last_used_at)*1000 > RIAK_GLOBAL(persistent_timeout)) {
      connection->needs_reconnect = 1;
   }
   if (connection->needs_reconnect) {
      if (riack_reconnect(connection->client) == RIACK_SUCCESS) {
         connection->needs_reconnect = 0;
      } else {
         result = 0;
      }
      RIAK_GLOBAL(reconnects)++;
   }
   return result;
}
/* }}} */

zend_bool ensure_connected_init(riak_connection *connection, char* host, int host_len, int port TSRMLS_DC) /* {{{ */
{
   char *szHost;
   zend_bool result;
   result = 0;
   
   if (connection->client->sockfd <= 0) {
      szHost = pestrndup(host, host_len, 0);
      if (riack_connect(connection->client, szHost, port, NULL) == RIACK_SUCCESS) {
         connection->needs_reconnect = 0;
         result = 1;
      }
      pefree(szHost, 0);
   } else {
      result = ensure_connected(connection TSRMLS_CC);
   }
   return result;
}
/* }}} */

void mark_for_reconnect(riak_connection *connection) /* {{{ */
{
   connection->needs_reconnect = 1;
}
/* }}} */

/*************************************************
* Connection pooling
*************************************************/

zend_bool lock_pool(TSRMLS_D) /* {{{ */
{
#ifdef ZTS
  if (tsrm_mutex_lock(RIAK_GLOBAL(pool_mutex)) == 0) {
    return 1;
  } else {
    return 0;
  }
#endif
}
/* }}} */

void unlock_pool(TSRMLS_D) /* {{{ */
{
#ifdef ZTS
  tsrm_mutex_unlock(RIAK_GLOBAL(pool_mutex));
#endif
}
/* }}} */

void release_connection(riak_connection *connection TSRMLS_DC) /* {{{ */
{
   riak_connection_pool* pool = NULL;
   RIAK_GLOBAL(open_connections)--;
   if (connection->persistent) {
      /* If we fail to lock we might have a stuck client, find a way to deal with this. */
      if (lock_pool(TSRMLS_C)) {
         connection->last_used_at = time(NULL);
         RIAK_GLOBAL(open_connections_persistent)--;
         pool = pool_for_host_port(connection->client->host, 
            strlen(connection->client->host), connection->client->port TSRMLS_CC);
         release_connection_from_pool(pool, connection);
         unlock_pool(TSRMLS_C);
      }
   } else {
      if (connection->client) {
         riack_free(connection->client);
      }
      pefree(connection, 0);
   }
}
/* }}} */

riak_connection *take_connection(char* host, int host_len, int port TSRMLS_DC) /* {{{ */
{
   riak_connection* connection;
   riak_connection_pool* pool;
   riak_connection_pool_entry *entry = NULL;

   if (lock_pool(TSRMLS_C)) {
      pool = pool_for_host_port(host, host_len, port TSRMLS_CC);
      entry = take_connection_entry_from_pool(pool);
      unlock_pool(TSRMLS_C);
   }
   if (entry) {
      connection = &entry->connection;
      if (!ensure_connected_init(connection, host, host_len, port TSRMLS_CC)) {
         connection->needs_reconnect = 1;
         release_connection_from_pool(pool, connection);
         return NULL;
      }
      RIAK_GLOBAL(open_connections_persistent)++;
   } else {
      /* We could not get a persistent connection, make a new non persistent connection. */
      connection = pemalloc(sizeof(riak_connection), 0);
      memset(connection, 0, sizeof(riak_connection));
      connection->persistent = 0;
      connection->client = riack_new_client(&riack_php_allocator);
      connection->last_used_at = time(NULL);
      if (!ensure_connected_init(connection, host, host_len, port TSRMLS_CC)) {
         release_connection(connection TSRMLS_CC);
         return NULL;
      }
   }
   if (connection) {
      RIAK_GLOBAL(open_connections)++;
   }
   return connection;
}
/* }}} */

void release_connection_from_pool(riak_connection_pool* pool, riak_connection *connection) /* {{{ */
{
  int i;
  riak_connection_pool_entry* current_entry;
  for (i=0; i<pool->count; ++i) {
    current_entry = &pool->entries[i];
    if (&(current_entry->connection) == connection) {
      current_entry->in_use = 0;
    }
  }
  /* Not found we should warn about this. */
}
/* }}} */

riak_connection_pool_entry *take_connection_entry_from_pool(riak_connection_pool *pool) /* {{{ */
{
  int i;
  riak_connection_pool_entry* current_entry;
  for (i=0; i<pool->count; ++i) {
    current_entry = &pool->entries[i];
    if (!current_entry->in_use) {
      current_entry->in_use = 1;
      current_entry->connection.persistent = 1;
      if (!current_entry->connection.client) {
         current_entry->connection.last_used_at = time(NULL);
         current_entry->connection.client = riack_new_client(&riack_php_persistent_allocator);
      }
      return current_entry;
    }
  }
  return NULL;
}
/* }}} */

riak_connection_pool *pool_for_host_port(char* host, int host_len, int port TSRMLS_DC) /* {{{ */
{
  char *szHost;
  char szConnection[512];
  zend_rsrc_list_entry *le;
  riak_connection_pool *pool;
  zend_rsrc_list_entry nle;

  szHost = pestrndup(host, host_len, 0);
  snprintf(szConnection, sizeof(szConnection), "%s:%d", szHost, port );
  pefree(szHost, 0);

  if (zend_hash_find(&EG(persistent_list), szConnection, strlen(szConnection)+1, (void**)&le) == FAILURE) {
    pool = initialize_pool(TSRMLS_C);
    nle.ptr = pool;
    nle.type = le_riak_connection_list;
    nle.refcount = 1;
    zend_hash_update(&EG(persistent_list), szConnection, strlen(szConnection)+1, (void*)&nle, sizeof(zend_rsrc_list_entry), NULL);
  } else {
    pool = (riak_connection_pool*)le->ptr;
  }
  return pool;
}
/* }}} */

riak_connection_pool* initialize_pool(TSRMLS_D) /* {{{ */
{
  riak_connection_pool* pool;
  pool = pemalloc(sizeof(riak_connection_pool), 1);
  pool->count = RIAK_GLOBAL(persistent_connections);
  pool->entries = pemalloc(pool->count * sizeof(riak_connection_pool_entry), 1);
  memset(pool->entries, 0, pool->count * sizeof(riak_connection_pool_entry));
  return pool;
}
/* }}} */

void le_riak_connections_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
  int i;
  riak_connection_pool *pool = (riak_connection_pool*)rsrc->ptr;
  if (pool->entries) {
    for (i=0; i<pool->count; ++i) {
      if (pool->entries[i].connection.client) {
        riack_free(pool->entries[i].connection.client);
      }
    }
    pefree(pool->entries, 1);
  }
  pefree(pool, 1);
}
/* }}} */
