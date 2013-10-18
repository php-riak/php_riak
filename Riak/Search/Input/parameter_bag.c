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
#include "parameter_bag.h"
#include "php_riak.h"

zend_class_entry *riak_search_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setrowlimit, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, rowlimit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setstart, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setsort, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, sort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setfilter, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, sort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setdefaultfield, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, defaultfield)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setdefaultop, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, defaultop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setpresort, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, presort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setfieldlimit, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, fieldlimit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_input_methods[] = {
    PHP_ME(Riak_Search_Input, setRowLimit, arginfo_search_input_setrowlimit, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getRowLimit, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setStartOffset, arginfo_search_input_setstart, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getStartOffset, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setSort, arginfo_search_input_setsort, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getSort, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setFilter, arginfo_search_input_setfilter, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getFilter, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setDefaultField, arginfo_search_input_setdefaultfield, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getDefaultField, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setDefaultOperation, arginfo_search_input_setdefaultop, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getDefaultOperation, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setPresort, arginfo_search_input_setpresort, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getPresort, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setFieldLimits, arginfo_search_input_setfieldlimit, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getFieldLimits, arginfo_search_input_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_search_input_parameterbag_init(TSRMLS_D)/* {{{  */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Input", "ParameterBag", riak_search_input_methods);
    riak_search_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "rowLimit", sizeof("rowLimit")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "start", sizeof("start")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "sort", sizeof("sort")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "filter", sizeof("filter")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "defaultField", sizeof("defaultField")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "defaultOperation", sizeof("defaultOperation")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "presort", sizeof("presort")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "fieldLimits", sizeof("fieldLimits")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

/*************************************************************
* Implementation: Riak\Search\Input\ParameterBag
*************************************************************/

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setRowLimit(int $rowLimit)
Set row limit for search */
PHP_METHOD(Riak_Search_Input, setRowLimit)
{
    RIAK_SETTER_LONG(riak_search_input_ce, "rowLimit");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input\ParameterBag->getRowLimit()
Set row limit for search */
PHP_METHOD(Riak_Search_Input, getRowLimit)
{
    RIAK_GETTER_LONG(riak_search_input_ce, "rowLimit");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setStartOffset(int $startOffset)
Set start offset for this search */
PHP_METHOD(Riak_Search_Input, setStartOffset)
{
    RIAK_SETTER_LONG(riak_search_input_ce, "start");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input\ParameterBag->getStartOffset()
Get start offset */
PHP_METHOD(Riak_Search_Input, getStartOffset)
{
    RIAK_GETTER_LONG(riak_search_input_ce, "start");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setSort(string $sort)
Set the sort string */
PHP_METHOD(Riak_Search_Input, setSort)
{
    RIAK_SETTER_STRING(riak_search_input_ce, "sort");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\ParameterBag->getSort()
Get the sort string */
PHP_METHOD(Riak_Search_Input, getSort)
{
    RIAK_GETTER_STRING(riak_search_input_ce, "sort");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setFilter(string $filter)
Set the filter string */
PHP_METHOD(Riak_Search_Input, setFilter)
{
    RIAK_SETTER_STRING(riak_search_input_ce, "filter");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\ParameterBag->getFilter()
Get the filter string */
PHP_METHOD(Riak_Search_Input, getFilter)
{
    RIAK_GETTER_STRING(riak_search_input_ce, "filter");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setDefaultField(string $defaultField)
Set the default field name */
PHP_METHOD(Riak_Search_Input, setDefaultField)
{
    RIAK_SETTER_STRING(riak_search_input_ce, "defaultField");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\ParameterBag->getDefaultField()
Get the default field name */
PHP_METHOD(Riak_Search_Input, getDefaultField)
{
    RIAK_GETTER_STRING(riak_search_input_ce, "defaultField");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setDefaultOperation(string $defaultOperation)
Set the default opereration */
PHP_METHOD(Riak_Search_Input, setDefaultOperation)
{
    RIAK_SETTER_STRING(riak_search_input_ce, "defaultOperation");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\ParameterBag->getDefaultOperation()
Get the default operation */
PHP_METHOD(Riak_Search_Input, getDefaultOperation)
{
    RIAK_GETTER_STRING(riak_search_input_ce, "defaultOperation");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setPresort(string $presort)
Set the presort string */
PHP_METHOD(Riak_Search_Input, setPresort)
{
    RIAK_SETTER_STRING(riak_search_input_ce, "presort");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\ParameterBag->getPresort()
Get the presort string */
PHP_METHOD(Riak_Search_Input, getPresort)
{
    RIAK_GETTER_STRING(riak_search_input_ce, "presort");
}
/* }}} */

/* {{{ proto Riak\Search\Input\ParameterBag Riak\Search\Input\ParameterBag->setFieldLimits(array $fieldLimits)
Set field limits */
PHP_METHOD(Riak_Search_Input, setFieldLimits)
{
    zval* zlimits;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zlimits) == FAILURE) {
        return;
    }
    zend_update_property(riak_search_input_ce, getThis(), "fieldLimits", sizeof("fieldLimits")-1, zlimits TSRMLS_CC);
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto array|null Riak\Search\Input\ParameterBag->getFieldLimits()
Get the field limits array */
PHP_METHOD(Riak_Search_Input, getFieldLimits)
{
    zval* zlimits = zend_read_property(riak_search_input_ce, getThis(), "fieldLimits", sizeof("fieldLimits")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlimits) == IS_ARRAY) {
        RETURN_ZVAL(zlimits, 1, 0);
    }
    RETURN_NULL();
}
/* }}} */

