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
#include "index_query.h"
#include "../exception/exception.h"

zend_class_entry *riak_index_query_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_query_index_query_set_name, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_query_index_query_set_value, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_query_index_query_set_range, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_query_index_query_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_query_index_query_methods[] = {
    PHP_ME(Riak_Query_IndexQuery, __construct, arginfo_riak_query_index_query_set_name, ZEND_ACC_PUBLIC|ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, setName, arginfo_riak_query_index_query_set_name, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, getName, arginfo_riak_query_index_query_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, setExactValue, arginfo_riak_query_index_query_set_value, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, getExactValue, arginfo_riak_query_index_query_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, setRangeValue, arginfo_riak_query_index_query_set_range, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, getRangeValue, arginfo_riak_query_index_query_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Query_IndexQuery, isRangeQuery, arginfo_riak_query_index_query_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_query_index_query_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Query", "IndexQuery", riak_query_index_query_methods);
    riak_index_query_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_index_query_ce, "name", sizeof("name")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_index_query_ce, "exactValue", sizeof("exactValue")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_index_query_ce, "minValue", sizeof("minValue")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_index_query_ce, "maxValue", sizeof("maxValue")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_bool(riak_index_query_ce, "isRange", sizeof("isRange")-1, 0, ZEND_ACC_PRIVATE TSRMLS_CC);
}


PHP_METHOD(Riak_Query_IndexQuery, __construct)
{
    RIAK_SETTER_STRING(riak_index_query_ce, "name")
}

PHP_METHOD(Riak_Query_IndexQuery, setName)
{
    RIAK_SETTER_STRING(riak_index_query_ce, "name")
    RIAK_RETURN_THIS
}

PHP_METHOD(Riak_Query_IndexQuery, getName)
{
    RIAK_GETTER_STRING(riak_index_query_ce, "name")
}

PHP_METHOD(Riak_Query_IndexQuery, setExactValue)
{
    RIAK_SETTER_STRING(riak_index_query_ce, "exactValue")
    zend_update_property_bool(riak_index_query_ce, getThis(), "isRange", sizeof("isRange")-1, 0 TSRMLS_CC);
    RIAK_RETURN_THIS
}

PHP_METHOD(Riak_Query_IndexQuery, getExactValue)
{
    RIAK_GETTER_STRING(riak_index_query_ce, "exactValue")
}

PHP_METHOD(Riak_Query_IndexQuery, setRangeValue)
{
    char *min, *max;
    int minlen, maxlen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &min, &minlen, &max, &maxlen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property_stringl(riak_index_query_ce, getThis(), "minValue", sizeof("minValue")-1, min, minlen TSRMLS_CC);
    zend_update_property_stringl(riak_index_query_ce, getThis(), "maxValue", sizeof("maxValue")-1, max, maxlen TSRMLS_CC);
    zend_update_property_bool(riak_index_query_ce, getThis(), "isRange", sizeof("isRange")-1, 1 TSRMLS_CC);
}

PHP_METHOD(Riak_Query_IndexQuery, getRangeValue)
{
    zval *zrangearr, *zmin, *zmax;
    MAKE_STD_ZVAL(zrangearr);
    array_init(zrangearr);
    zmin = zend_read_property(riak_index_query_ce, getThis(), "minValue", sizeof("minValue")-1, 1 TSRMLS_CC);
    zval_addref_p(zmin);
    add_next_index_zval(zrangearr, zmin);
    zmax = zend_read_property(riak_index_query_ce, getThis(), "maxValue", sizeof("maxValue")-1, 1 TSRMLS_CC);
    zval_addref_p(zmax);
    add_next_index_zval(zrangearr, zmax);
    RETURN_ZVAL(zrangearr, 0, 1);
}

PHP_METHOD(Riak_Query_IndexQuery, isRangeQuery)
{
    RIAK_GETTER_BOOL(riak_index_query_ce, "isRange")
}
