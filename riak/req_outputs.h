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

#ifndef RIAK_OUTPUTS__H__
#define RIAK_OUTPUTS__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_output_ce;
extern zend_class_entry *riak_get_output_ce;
extern zend_class_entry *riak_put_output_ce;

void riak_req_outputs_init(TSRMLS_D);

zval *get_output_from_riack_get_object(struct RIACK_GET_OBJECT* getobj, zval* zkey TSRMLS_DC);
zval *put_output_from_riack_object(struct RIACK_OBJECT* obj, zval* zkey TSRMLS_DC);

PHP_METHOD(Riak_Output_Output, getObjectList);
PHP_METHOD(Riak_Output_Output, getVClock);
PHP_METHOD(Riak_Output_Output, getKey);
PHP_METHOD(Riak_Output_Output, hasSiblings);
PHP_METHOD(Riak_Output_Output, hasObject);
PHP_METHOD(Riak_Output_Output, getFirstObject);

PHP_METHOD(Riak_Output_GetOutput, isUnchanged);

#endif
