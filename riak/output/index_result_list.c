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
#include "index_result_list.h"
#include "index_result.h"
#include "../exception/exception.h"
#include <zend_interfaces.h>
#include <ext/spl/spl_iterators.h>
#include <ext/spl/spl_array.h>

zend_class_entry *riak_index_result_list_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_offsetexists, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_offsetset, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_list_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_output_index_result_list_methods[] = {
    PHP_ME(Riak_Index_Result_List, __construct, arginfo_riak_output_index_result_list_noargs, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)

    PHP_ME(Riak_Index_Result_List, offsetExists, arginfo_riak_output_index_result_offsetexists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Result_List, offsetGet, arginfo_riak_output_index_result_offsetexists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Result_List, offsetSet, arginfo_riak_output_index_result_offsetset, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Result_List, offsetUnset, arginfo_riak_output_index_result_offsetexists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Result_List, count, arginfo_riak_output_index_result_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Result_List, getIterator, arginfo_riak_output_index_result_list_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_output_index_result_list_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "IndexResultList", riak_output_index_result_list_methods);
    riak_index_result_list_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_index_result_list_ce, "resultList", sizeof("resultList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}

PHP_METHOD(Riak_Index_Result_List, __construct)
{
    zval* zresultlist;
    MAKE_STD_ZVAL(zresultlist);
    object_init_ex(zresultlist, spl_ce_ArrayObject);
    zend_update_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, zresultlist TSRMLS_CC);
    zval_ptr_dtor(&zresultlist);
}

/* {{{ proto bool Riak\Output\IndexResultList->offsetExists($offset)
Does the offset exists */
PHP_METHOD(Riak_Index_Result_List, offsetExists)
{
    zval *zoffset, *zresultlist, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zresultlist, spl_ce_ArrayObject, NULL, "offsetexists", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto IndexResult Riak\Output\IndexResultList->offsetGet($offset)
retrieve an object at offset */
PHP_METHOD(Riak_Index_Result_List, offsetGet)
{
    zval *zoffset, *zresultlist, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zresultlist, spl_ce_ArrayObject, NULL, "offsetget", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto void Riak\Output\IndexResultList->offsetSet($offset, IndexResult $value)
Set an IndexResult at given offset */
PHP_METHOD(Riak_Index_Result_List, offsetSet)
{
    zval *zoffset, *zvalue, *zresultlist;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zO", &zoffset, &zvalue, riak_index_result_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_2_params(&zresultlist, spl_ce_ArrayObject, NULL, "offsetset", NULL, zoffset, zvalue);
}
/* }}} */

/* {{{ proto void Riak\Output\IndexResultList->offsetUnset($offset)
Removes a result at offset from list */
PHP_METHOD(Riak_Index_Result_List, offsetUnset)
{
    zval *zoffset, *zresultlist;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zresultlist, spl_ce_ArrayObject, NULL, "offsetunset", NULL, zoffset);
}
/* }}} */

/* {{{ proto int Riak\Output\IndexResultList->count()
How many objects in the list */
PHP_METHOD(Riak_Index_Result_List, count)
{
    zval *zresultlist, *zresult;
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zresultlist, spl_ce_ArrayObject, NULL, "count", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto TraversableRiak\Output\IndexResultList->getIterator()
Retrieve an external iterator */
PHP_METHOD(Riak_Index_Result_List, getIterator)
{
    zval *zresultlist, *zresult;
    zresultlist = zend_read_property(riak_index_result_list_ce, getThis(), "resultList", sizeof("resultList")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zresultlist, spl_ce_ArrayObject, NULL, "getiterator", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */
