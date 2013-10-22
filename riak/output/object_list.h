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

#ifndef RIAK_OUTPUT__OBJECT_LIST__H__
#define RIAK_OUTPUT__OBJECT_LIST__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_output_object_list_ce;

void riak_output_object_list_init(TSRMLS_D);

PHP_METHOD(Riak_Output_Object_List, __construct);
PHP_METHOD(Riak_Output_Object_List, first);
PHP_METHOD(Riak_Output_Object_List, isEmpty);
PHP_METHOD(Riak_Output_Object_List, offsetExists);
PHP_METHOD(Riak_Output_Object_List, offsetGet);
PHP_METHOD(Riak_Output_Object_List, offsetSet);
PHP_METHOD(Riak_Output_Object_List, offsetUnset);
PHP_METHOD(Riak_Output_Object_List, count);
PHP_METHOD(Riak_Output_Object_List, getIterator);


#endif //RIAK_OUTPUT__OBJECT_LIST__H__
