/*
   Copyright 2014: Kaspar Bach Pedersen

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
#ifndef RIAK_C_HELPERS_H
#define RIAK_C_HELPERS_H

#include "php_riak_internal.h"
#include <riak_binary.h>

riak_binary* riak_binary_deep_from_stringl(riak_config *cfg, int len, const char* val);
riak_binary* riak_binary_shallow_from_property(riak_config *cfg, zend_class_entry *scope, zval* zobj, char* property_name TSRMLS_DC);

#endif // RIAK_C_HELPERS_H
