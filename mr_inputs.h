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

#ifndef RIAK_MR_INPUTS__H__
#define RIAK_MR_INPUTS__H__

#include <php.h>

extern zend_class_entry *riak_mrinput_ce;

void riak_mrinputs_init(TSRMLS_D);

PHP_METHOD(RiakMrInput, toArray);

PHP_METHOD(RiakMrInputBucket, __construct);
PHP_METHOD(RiakMrInputBucket, getValue);

PHP_METHOD(RiakMrInputKeyList, __construct);

#endif
