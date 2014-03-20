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

#ifndef RIAK_SEARCH__H__
#define RIAK_SEARCH__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_search_ce;

/* helpers */

void riak_search_init(TSRMLS_D);
void riak_search_set_optional_params(struct RIACK_CLIENT *client, zval* zparams, struct RIACK_SEARCH_OPTIONAL_PARAMETERS* search_params TSRMLS_DC);
void riak_search_free_optional_params(struct RIACK_CLIENT *client, struct RIACK_SEARCH_OPTIONAL_PARAMETERS* search_params TSRMLS_DC);

zval* riak_search_output_from_riack_search_result(struct RIACK_SEARCH_RESULT *search_result TSRMLS_DC);

/* Search */

PHP_METHOD(Riak_Search, __construct);
PHP_METHOD(Riak_Search, search);

#endif
