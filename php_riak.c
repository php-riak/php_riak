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

#include <php.h>
#include <php_ini.h>
#include <riack.h>
#include "php_riak.h"
#include "client.h"
#include "object.h"
#include "bucket.h"
#include "bucket_properties.h"
#include "exceptions.h"

int le_riak_connection_list;

ZEND_DECLARE_MODULE_GLOBALS(riak)

zend_function_entry riak_functions[] = {
  { NULL, NULL, NULL }
};

struct RIACK_ALLOCATOR riack_php_persistent_allocator = 
{
  riack_php_persistent_alloc,
  riack_php_persistent_free,
};

struct RIACK_ALLOCATOR riack_php_allocator =
{
  riack_php_alloc,
  riack_php_free
};

zend_module_entry riak_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_RIAK_EXTNAME,
  riak_functions,
  PHP_MINIT(riak),
  PHP_MSHUTDOWN(riak),
  NULL,
  NULL,
  NULL,
  PHP_RIAK_VERSION,
  PHP_MODULE_GLOBALS(riak),
  PHP_GINIT(riak),
  PHP_GSHUTDOWN(riak),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

// install module
ZEND_GET_MODULE(riak)

PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("riak.persistent.connections", "20", PHP_INI_ALL, OnUpdateLong, persistent_connections, zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.persistent.timeout", "1000", PHP_INI_ALL,   OnUpdateLong, persistent_timeout,     zend_riak_globals, riak_globals)
PHP_INI_END()

// Module constructor
PHP_MINIT_FUNCTION(riak) 
{
  REGISTER_INI_ENTRIES();
  riack_init();
  // TODO Store persistant connections here
  le_riak_connection_list = zend_register_list_destructors_ex(NULL, le_riak_connections_pefree, "Persistent clients", module_number);
  riak_client_init(TSRMLS_C);
  riak_object_init(TSRMLS_C);
  riak_bucket_init(TSRMLS_C);
  riak_bucket_props_init(TSRMLS_C);
  riak_exceptions_init(TSRMLS_C);
  return SUCCESS;
}

// Module destructor
PHP_MSHUTDOWN_FUNCTION(riak)
{
  riack_cleanup();
  UNREGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_GINIT_FUNCTION(riak)
{
  riak_globals->persistent_connections = 20;
  riak_globals->persistent_timeout = 1000;
#ifdef ZTS
  riak_globals->pool_mutex = tsrm_mutex_alloc();
#endif
}

PHP_GSHUTDOWN_FUNCTION(riak) 
{
#ifdef ZTS
  tsrm_mutex_free(riak_globals->pool_mutex);
#endif
}


void throw_exception(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC)
{
  if (errorStatus == RIACK_ERROR_COMMUNICATION) {
    zend_throw_exception(riak_communication_exception_ce, "Communcation error", 1001 TSRMLS_CC);
  } else if (errorStatus == RIACK_ERROR_RESPONSE) {
    if (client->last_error) {
      zend_throw_exception(riak_response_exception_ce, client->last_error, 1002 TSRMLS_CC); 
    } else {
      zend_throw_exception(riak_response_exception_ce, "Unexpected response from riak", 1002 TSRMLS_CC);
    }
  }
}

//////////////////////////////////////////////////////////////
// Connection pooling

zend_bool lock_pool(TSRMLS_D)
{
#ifdef ZTS
  if (tsrm_mutex_lock(RIAK_GLOBAL(pool_mutex)) == 0) {
    return 1;
  } else {
    return 0;
  }
#endif
}

void unlock_pool(TSRMLS_D)
{
#ifdef ZTS
  tsrm_mutex_unlock(RIAK_GLOBAL(pool_mutex));
#endif
}

void release_client(struct RIACK_CLIENT *client TSRMLS_DC)
{
  // If we fail to lock we might have a stuck client, find a way to deal with this.
  if (lock_pool(TSRMLS_C)) {
    // Release
    unlock_pool(TSRMLS_C);
  }
}

struct RIACK_CLIENT *take_client(char* host, int host_len, int port TSRMLS_DC)
{
  char *szHost;
  char szConnection[512];
  riak_connection_pool* pool;
  struct RIACK_CLIENT *client;
  szHost = pestrndup(host, host_len, 0);
  snprintf(szConnection, sizeof(szConnection), "%s:%d", szHost, port );
  pefree(szHost, 0);

  if (lock_pool(TSRMLS_C)) {
    pool = pool_for_url(szConnection TSRMLS_CC);
    client = take_client_from_pool(pool);
    unlock_pool(TSRMLS_C);
  }
  return client;
}

struct RIACK_CLIENT *take_client_from_pool(riak_connection_pool *pool)
{
  return NULL;
}

riak_connection_pool *pool_for_url(char* szUrl TSRMLS_DC)
{
  zend_rsrc_list_entry *le;
  riak_connection_pool *pool;
  zend_rsrc_list_entry nle;
  if (zend_hash_find(&EG(persistent_list), szUrl, strlen(szUrl)+1, (void**)&le) == FAILURE) {
    pool = initialize_pool(TSRMLS_C);
    nle.ptr = pool;
    nle.type = le_riak_connection_list;
    nle.refcount = 1;
    zend_hash_update(&EG(persistent_list), szUrl, strlen(szUrl)+1, (void*)&nle, sizeof(zend_rsrc_list_entry), NULL);
  } else {
    pool = (riak_connection_pool*)le->ptr;
  }
  return pool;
}

riak_connection_pool* initialize_pool(TSRMLS_D) 
{
  riak_connection_pool* pool;
  pool = pemalloc(sizeof(riak_connection_pool), 1);
  pool->count = RIAK_GLOBAL(persistent_connections);
  pool->connections = pemalloc(pool->count * sizeof(riak_connection), 1);
  memset(pool->connections, 0, pool->count * sizeof(riak_connection));
  return pool;
}

void le_riak_connections_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC) 
{
  int i;
  riak_connection_pool *pool = (riak_connection_pool*)rsrc->ptr;
  if (pool->connections) {
    for (i=0; i<pool->count; ++i) {
      if (pool->connections[i].client) {
        riack_free(pool->connections[i].client);
      }
    }
    pefree(pool->connections, 1);
  }
  pefree(pool, 1);
}

//////////////////////////////////////////////////////////////
// Riack allocator

void *riack_php_alloc(void *allocator_data, size_t size)
{
  (void) allocator_data;
  if (size == 0) {
    return 0;
  }
  return pemalloc(size, 0);
}

void riack_php_free (void *allocator_data, void *data)
{
  (void) allocator_data;
  if (data) {
    pefree(data, 0);
  }
}

void *riack_php_persistent_alloc(void *allocator_data, size_t size)
{
  (void) allocator_data;
  if (size == 0) {
    return 0;
  }
  return pemalloc(size, 1);
}

void riack_php_persistent_free (void *allocator_data, void *data)
{
  (void) allocator_data;
  if (data) {
    pefree(data, 1);
  }
}

