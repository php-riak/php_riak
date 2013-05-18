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

#ifndef RIAK_MR_FUNCTION__H__
#define RIAK_MR_FUNCTION__H__

#include <php.h>

extern zend_class_entry *riak_mrfunction_ce;

void riak_mrfunctions_init(TSRMLS_D);

void create_named_mr_function(zend_class_entry *classentry, zval* result,
                               zend_bool named, const char* source, int sourcelen TSRMLS_DC);

PHP_METHOD(RiakMrFunction, __construct);
PHP_METHOD(RiakMrFunction, toArray);

PHP_METHOD(RiakMrJavascriptFunction, named);
PHP_METHOD(RiakMrJavascriptFunction, anon);
PHP_METHOD(RiakMrJavascriptFunction, getLanguage);

PHP_METHOD(RiakMrErlangFunction, named);
PHP_METHOD(RiakMrErlangFunction, anon);
PHP_METHOD(RiakMrErlangFunction, getLanguage);

#endif
