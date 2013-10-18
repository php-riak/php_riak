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

#ifndef RIAK_SEARCH__INPUT__PARAMETER_BAG__H__
#define RIAK_SEARCH__INPUT__PARAMETER_BAG__H__

#include "php_riak_internal.h"

extern zend_class_entry *riak_search_input_ce;

void riak_search_input_parameterbag_init(TSRMLS_D);

/* Search\Input */
PHP_METHOD(Riak_Search_Input, setRowLimit);
PHP_METHOD(Riak_Search_Input, getRowLimit);
PHP_METHOD(Riak_Search_Input, setStartOffset);
PHP_METHOD(Riak_Search_Input, getStartOffset);
PHP_METHOD(Riak_Search_Input, setSort);
PHP_METHOD(Riak_Search_Input, getSort);
PHP_METHOD(Riak_Search_Input, setFilter);
PHP_METHOD(Riak_Search_Input, getFilter);
PHP_METHOD(Riak_Search_Input, setDefaultField);
PHP_METHOD(Riak_Search_Input, getDefaultField);
PHP_METHOD(Riak_Search_Input, setDefaultOperation);
PHP_METHOD(Riak_Search_Input, getDefaultOperation);
PHP_METHOD(Riak_Search_Input, setPresort);
PHP_METHOD(Riak_Search_Input, getPresort);
PHP_METHOD(Riak_Search_Input, setFieldLimits);
PHP_METHOD(Riak_Search_Input, getFieldLimits);

#endif // RIAK_SEARCH__INPUT__PARAMETER_BAG__H__
