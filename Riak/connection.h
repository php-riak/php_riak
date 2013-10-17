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

#ifndef RIAK_CONNECTION__H__
#define RIAK_CONNECTION__H__

#include <riack.h>
#include "pool.h"

extern zend_class_entry *riak_connection_ce;

typedef struct _client_data {
  /* required */
  zend_object std;

  riak_connection *connection;
} client_data;

#define GET_RIAK_CONNECTION(ZOBJ, VAR) VAR = ((client_data*)zend_object_store_get_object(ZOBJ TSRMLS_CC))->connection

/*************************************************
* Constants
*************************************************/
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8087

/*************************************************
* Functions
*************************************************/

zval* create_client_object(char* host, long port TSRMLS_DC);

void riak_connection_init(TSRMLS_D);
zend_object_value create_client_data(zend_class_entry *class_type TSRMLS_DC);
void free_client_data(void *object TSRMLS_DC);

PHP_METHOD(RiakConnection, __construct);
PHP_METHOD(RiakConnection, ping);
PHP_METHOD(RiakConnection, getHost);
PHP_METHOD(RiakConnection, getPort);
PHP_METHOD(RiakConnection, getBucket);

#endif
