/*
   Copyright 2013: Kaspar Bach Pedersen

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
#ifndef RIAK_PROPERTY__COMMIT_HOOK_LIST__H__
#define RIAK_PROPERTY__COMMIT_HOOK_LIST__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_commit_hook_list_ce;

void riak_property_commit_hook_list_init(TSRMLS_D);

PHP_METHOD(RiakCommitHookList, __construct);
PHP_METHOD(RiakCommitHookList, offsetExists);
PHP_METHOD(RiakCommitHookList, offsetGet);
PHP_METHOD(RiakCommitHookList, offsetSet);
PHP_METHOD(RiakCommitHookList, offsetUnset);
PHP_METHOD(RiakCommitHookList, count);
PHP_METHOD(RiakCommitHookList, getIterator);

#endif
