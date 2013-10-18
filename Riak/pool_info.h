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

#ifndef RIAK_POOL_INFO__H__
#define RIAK_POOL_INFO__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_poolinfo_ce;

PHP_METHOD(RiakPoolInfo, getNumActiveConnection);
PHP_METHOD(RiakPoolInfo, getNumActivePersistentConnection);
PHP_METHOD(RiakPoolInfo, getNumReconnect);

void riak_poolinfo_init(TSRMLS_D);

#endif
