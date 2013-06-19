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

#include <php.h>
#include <riack.h>

extern zend_class_entry *riak_get_config_ce;

void riak_req_configs_init(TSRMLS_D);

zend_bool get_return_head(zval* zreqconfig TSRMLS_DC);

PHP_METHOD(RiakRequestConfiguration, setReturnHead);
PHP_METHOD(RiakRequestConfiguration, getReturnHead);

#endif
