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

#ifndef RIAK_OUTPUT__GET_OUTPUT__H__
#define RIAK_OUTPUT__GET_OUTPUT__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_get_output_ce;

void riak_output_get_output_init(TSRMLS_D);
zval *get_output_from_riack_get_object(riack_get_object* getobj, zval* zkey TSRMLS_DC);

PHP_METHOD(Riak_Output_GetOutput, isUnchanged);

#endif //RIAK_OUTPUT__GET_OUTPUT__H__
