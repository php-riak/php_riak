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

#ifndef RIAK_INPUT__DELETE_INPUT__H__
#define RIAK_INPUT__DELETE_INPUT__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_delete_input_ce;

void riak_input_delete_input_init(TSRMLS_D);

/* Riak\Input\DeleteInput */
PHP_METHOD(Riak_Input_DeleteInput, setR);
PHP_METHOD(Riak_Input_DeleteInput, getR);
PHP_METHOD(Riak_Input_DeleteInput, setPR);
PHP_METHOD(Riak_Input_DeleteInput, getPR);
PHP_METHOD(Riak_Input_DeleteInput, setRW);
PHP_METHOD(Riak_Input_DeleteInput, getRW);
PHP_METHOD(Riak_Input_DeleteInput, setW);
PHP_METHOD(Riak_Input_DeleteInput, getW);
PHP_METHOD(Riak_Input_DeleteInput, setDW);
PHP_METHOD(Riak_Input_DeleteInput, getDW);
PHP_METHOD(Riak_Input_DeleteInput, setPW);
PHP_METHOD(Riak_Input_DeleteInput, getPW);
PHP_METHOD(Riak_Input_DeleteInput, setVClock);
PHP_METHOD(Riak_Input_DeleteInput, getVClock);


#endif //RIAK_INPUT__DELETE_INPUT__H__
