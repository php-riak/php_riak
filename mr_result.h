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

#ifndef RIAK_MR_RESULT__H__
#define RIAK_MR_RESULT__H__

#include <php.h>
#include <riack.h>

extern zend_class_entry *riak_mrresult_ce;

zval *riak_mrresult_from_riack_mapred(struct RIACK_MAPRED_RESPONSE *mapresult TSRMLS_DC);

void riak_mrresult_init(TSRMLS_D);

PHP_METHOD(RiakMrResult, __construct);

#endif
