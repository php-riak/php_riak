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

#ifndef RIAK_MR_PHASE__H__
#define RIAK_MR_PHASE__H__

#include <php.h>

#define PHASE_TYPE_MAP    1
#define PHASE_TYPE_REDUCE 2
#define PHASE_TYPE_LINK   3

extern zend_class_entry *riak_mrphase_ce;

void riak_mrphase_init(TSRMLS_D);

PHP_METHOD(RiakMrPhase, __construct);
PHP_METHOD(RiakMrPhase, toArray);

#endif
