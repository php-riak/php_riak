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

#ifndef RIAK_INPUT__PUT_INPUT__H__
#define RIAK_INPUT__PUT_INPUT__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_put_input_ce;

void riak_input_put_input_init(TSRMLS_D);

/* Riak\Input\PutInput */
PHP_METHOD(Riak_Input_PutInput, setReturnHead);
PHP_METHOD(Riak_Input_PutInput, getReturnHead);
PHP_METHOD(Riak_Input_PutInput, getW);
PHP_METHOD(Riak_Input_PutInput, setW);
PHP_METHOD(Riak_Input_PutInput, getDW);
PHP_METHOD(Riak_Input_PutInput, setDW);
PHP_METHOD(Riak_Input_PutInput, getPW);
PHP_METHOD(Riak_Input_PutInput, setPW);
PHP_METHOD(Riak_Input_PutInput, getVClock);
PHP_METHOD(Riak_Input_PutInput, setVClock);
PHP_METHOD(Riak_Input_PutInput, getReturnBody);
PHP_METHOD(Riak_Input_PutInput, setReturnBody);
PHP_METHOD(Riak_Input_PutInput, getIfNoneMatch);
PHP_METHOD(Riak_Input_PutInput, setIfNoneMatch);
PHP_METHOD(Riak_Input_PutInput, getIfNotModified);
PHP_METHOD(Riak_Input_PutInput, setIfNotModified);


#endif //RIAK_INPUT__PUT_INPUT__H__
