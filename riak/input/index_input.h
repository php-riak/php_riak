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

#ifndef RIAK_INPUT__INDEX_INPUT__H__
#define RIAK_INPUT__INDEX_INPUT__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_index_input_ce;

void riak_input_index_input_init(TSRMLS_D);

PHP_METHOD(Riak_Input_IndexInput, setContinuation);
PHP_METHOD(Riak_Input_IndexInput, getContinuation);

PHP_METHOD(Riak_Input_IndexInput, getMaxResults);
PHP_METHOD(Riak_Input_IndexInput, setMaxResults);


#endif //RIAK_INPUT__INDEX_INPUT__H__
