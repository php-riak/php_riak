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
#include <string.h>

zend_bool ensure_connected(riak_context *context TSRMLS_DC) /* {{{ */
{
  zend_bool result = 1;
  if (difftime(time(NULL), context->last_used_at) > RIAK_GLOBAL(persistent_timeout)) {
      context->needs_reconnect = 1;
   }
   if (context->needs_reconnect) {
       if (context->connection) {
           riak_connection_free(&(context->connection));
           context->connection = 0;
       }
       char szport[20];
       sprintf(szport, "%d", context->port);
       riak_error err = riak_connection_new(context->config,
                                            &context->connection,
                                            context->szhost, szport, NULL);
       if (err == ERIAK_OK) {
           context->needs_reconnect = 0;
       } else {
           result = 0;
       }
      RIAK_GLOBAL(reconnects)++;
   }
   return result;
}
/* }}} */

zend_bool ensure_connected_init(riak_context *context, char* host, int host_len, int port TSRMLS_DC) /* {{{ */
{
    char *szHost;
    zend_bool result;
    result = 0;

    if (!context->szhost ||
            strncmp(context->szhost, host, host_len) != 0 ||
            context->port != port) {
        riak_c_persistent_free(context->szhost);
        // to be on the safe side make a copy with zero termination
        context->szhost = riak_c_persistent_alloc(host_len+1);
        memcpy(context->szhost, host, host_len);
        context->szhost[host_len] = 0;
        context->port = port;
        context->needs_reconnect = 1;
    }
    result = ensure_connected(context TSRMLS_CC);
/* TODO
        options.send_timeout_ms = RIAK_GLOBAL(send_timeout);
        options.recv_timeout_ms = RIAK_GLOBAL(recv_timeout);
        options.keep_alive_enabled = RIAK_GLOBAL(keep_alive);
*/
    return result;
}
/* }}} */

void mark_for_reconnect(riak_context *context) /* {{{ */
{
   context->needs_reconnect = 1;
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
  return 1;
}
/* }}} */

void unlock_pool(TSRMLS_D) /* {{{ */
{
#ifdef ZTS
    tsrm_mutex_unlock(RIAK_GLOBAL(pool_mutex));
#endif
}
/* }}} */

void release_context(riak_context *context TSRMLS_DC) /* {{{ */
{
   riak_connection_pool* pool = NULL;
   RIAK_GLOBAL(open_connections)--;
   if (context->persistent) {
      /* If we fail to lock we might have a stuck client, find a way to deal with this. */
      if (lock_pool(TSRMLS_C)) {
         context->last_used_at = time(NULL);
         RIAK_GLOBAL(open_connections_persistent)--;
         pool = pool_for_host_port(context->szhost,
            strlen(context->szhost), context->port TSRMLS_CC);
         release_context_from_pool(pool, context);
         unlock_pool(TSRMLS_C);
      }
   } else {
      if (context->connection) {
          riak_connection_free(&(context->connection));
          context->connection = 0;
      }
   }
}
/* }}} */

riak_context *take_connection(char* host, int host_len, int port TSRMLS_DC) /* {{{ */
{
   riak_context* context;
   riak_connection_pool* pool;
   riak_connection_pool_entry *entry = NULL;

   if (lock_pool(TSRMLS_C)) {
      pool = pool_for_host_port(host, host_len, port TSRMLS_CC);
      entry = take_connection_entry_from_pool(pool);
      unlock_pool(TSRMLS_C);
   }
   if (entry) {
      context = &entry->context;
      if (!ensure_connected_init(context, host, host_len, port TSRMLS_CC)) {
         context->needs_reconnect = 1;
         release_context_from_pool(pool, context);
         return NULL;
      }
      RIAK_GLOBAL(open_connections_persistent)++;
   } else {
      /* We could not get a persistent connection, make a new non persistent connection. */
      context = pemalloc(sizeof(riak_context), 0);
      memset(context, 0, sizeof(riak_context));
      context->persistent = 0;
      context->last_used_at = time(NULL);
      if (!ensure_connected_init(context, host, host_len, port TSRMLS_CC)) {
         release_context(context TSRMLS_CC);
         return NULL;
      }
   }
   if (context) {
      RIAK_GLOBAL(open_connections)++;
   }
   return context;
}
/* }}} */

void release_context_from_pool(riak_connection_pool* pool, riak_context *context) /* {{{ */
{
    int i;
    riak_connection_pool_entry* current_entry;
    for (i=0; i<pool->count; ++i) {
        current_entry = &pool->entries[i];
        if (&(current_entry->context) == context) {
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
            current_entry->context.persistent = 1;
            if (!current_entry->context.config) {
                riak_config_new(&current_entry->context.config,
                                riak_c_persistent_alloc,
                                riak_c_persistent_realloc,
                                riak_c_persistent_free,
                                riak_c_pb_persistent_alloc,
                                riak_c_pb_persistent_free);
/*
riak_config_new(riak_config     **cfg,
                riak_alloc_fn     alloc,
                riak_realloc_fn   realloc,
                riak_free_fn      freeme,
                riak_pb_alloc_fn  pb_alloc,
                riak_pb_free_fn   pb_free);
*/
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
            if (pool->entries[i].context.connection) {
                riak_connection_free(&(pool->entries[i].context.connection));
            }
            if (pool->entries[i].context.config) {
                riak_config_free(&(pool->entries[i].context.config));
            }
        }
        pefree(pool->entries, 1);
    }
    pefree(pool, 1);
}
/* }}} */
