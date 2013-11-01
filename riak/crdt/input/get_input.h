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

#ifndef RIAK__CRDT__INPUT__GET_INPUT__H__
#define RIAK__CRDT__INPUT__GET_INPUT__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_crdt_input_get_ce;

void riak_crdt_input_get_input_init(TSRMLS_D);

PHP_METHOD(Riak_Crdt_Input_GetInput, getR);
PHP_METHOD(Riak_Crdt_Input_GetInput, setR);
PHP_METHOD(Riak_Crdt_Input_GetInput, getPR);
PHP_METHOD(Riak_Crdt_Input_GetInput, setPR);
PHP_METHOD(Riak_Crdt_Input_GetInput, getBasicQuorum);
PHP_METHOD(Riak_Crdt_Input_GetInput, setBasicQuorum);
PHP_METHOD(Riak_Crdt_Input_GetInput, getNotFoundOk);
PHP_METHOD(Riak_Crdt_Input_GetInput, setNotFoundOk);

#endif // RIAK__CRDT__INPUT__GET_INPUT__H__
