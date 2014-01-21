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

#ifndef RIAK_POOL__H__
#define RIAK_POOL__H__

#include "php_riak_internal.h"
#include <php.h>
#include <time.h>

typedef struct {
  riak_connection* connection;
  riak_config* config;

  char* szhost;
  int port;

  zend_bool needs_reconnect;
  zend_bool persistent;
  time_t last_used_at;
} riak_context;

typedef struct _riak_connection_pool_entry {
  zend_bool in_use;
  riak_context context;
} riak_connection_pool_entry;

typedef struct _riak_connection_pool {
  int count;
  riak_connection_pool_entry *entries;
} riak_connection_pool;

zend_bool ensure_connected(riak_context *context TSRMLS_DC);
zend_bool ensure_connected_init(riak_context *context, char* host, int host_len, int port TSRMLS_DC);
void mark_for_reconnect(riak_context *context);

void release_context(riak_context *context TSRMLS_DC);
riak_context *take_connection(char* host, int host_len, int port TSRMLS_DC);

void le_riak_connections_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* Internal */
void release_context_from_pool(riak_connection_pool* pool, riak_context *context);
riak_connection_pool *pool_for_host_port(char* host, int host_len, int port TSRMLS_DC);
riak_connection_pool_entry *take_connection_entry_from_pool(riak_connection_pool *pool);
riak_connection_pool* initialize_pool(TSRMLS_D);

#endif
