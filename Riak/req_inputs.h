/*
   Copyright 2013 Kaspar Bach Pedersen

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

#ifndef RIAK_REQ_CONFIGS__H__
#define RIAK_REQ_CONFIGS__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_get_input_ce;
extern zend_class_entry *riak_put_input_ce;
extern zend_class_entry *riak_delete_input_ce;

void riak_req_inputs_init(TSRMLS_D);

/* Riak\Input\GetInput */
PHP_METHOD(Riak_Input_GetInput, setReturnHead);
PHP_METHOD(Riak_Input_GetInput, getReturnHead);
PHP_METHOD(Riak_Input_GetInput, getR);
PHP_METHOD(Riak_Input_GetInput, setR);
PHP_METHOD(Riak_Input_GetInput, getPR);
PHP_METHOD(Riak_Input_GetInput, setPR);
PHP_METHOD(Riak_Input_GetInput, getBasicQuorum);
PHP_METHOD(Riak_Input_GetInput, setBasicQuorum);
PHP_METHOD(Riak_Input_GetInput, getNotFoundOk);
PHP_METHOD(Riak_Input_GetInput, setNotFoundOk);
PHP_METHOD(Riak_Input_GetInput, getIfModifiedVClock);
PHP_METHOD(Riak_Input_GetInput, setIfModifiedVClock);
PHP_METHOD(Riak_Input_GetInput, getReturnDeletedVClock);
PHP_METHOD(Riak_Input_GetInput, setReturnDeletedVClock);

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

#endif
