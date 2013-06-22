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
#include <php.h>
#include <riack.h>
#include "php_riak.h"
#include "search.h"

zend_class_entry *riak_search_ce;
zend_class_entry *riak_search_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, client)
ZEND_END_ARG_INFO()


static zend_function_entry riak_search_methods[] = {
    ZEND_RAW_FENTRY("__construct", zim_Riak_Search__construct, arginfo_search_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_search_input_methods[] = {
    {NULL, NULL, NULL}
};

void riak_search_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Search", riak_search_methods);
    riak_search_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_ce, "client", sizeof("client")-1, ZEND_ACC_PROTECTED TSRMLS_CC);


    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Input", riak_search_input_methods);
    riak_search_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Riak\Search->__construct(RiakClient $client)
Create a new Search object */
ZEND_NAMED_FUNCTION(zim_Riak_Search__construct)
{
    zval *zclient;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zclient) == FAILURE) {
        return;
    }
    zend_update_property(riak_search_ce, getThis(), "client", sizeof("client")-1, zclient TSRMLS_CC);
}
/* }}} */
