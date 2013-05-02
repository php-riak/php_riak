/*
   Copyright 2012 Trifork A/S
   Author: Kaspar Pedersen

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

#ifndef RIAK__H__
#define RIAK__H__

#include <riack.h>

typedef struct _client_data {
    // required
    zend_object std;
    struct RIACK_CLIENT* client;
} client_data;

/////////////////////////////////////////////////
// Constants
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8087

/////////////////////////////////////////////////
// Functions

void riak_client_init(TSRMLS_D);
zend_object_value create_client_data(zend_class_entry *class_type TSRMLS_DC);
void free_client_data(void *object TSRMLS_DC);

PHP_METHOD(RiakClient, __construct);
 
#endif