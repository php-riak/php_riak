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
#include "object_list.h"
#include "exception/exception.h"
#include "object.h"
#include <zend_interfaces.h>
#include <ext/spl/spl_iterators.h>
#include <ext/spl/spl_array.h>

zend_class_entry *riak_output_object_list_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_object_list_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_object_list_offset_exists, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_object_list_offset_set, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


static zend_function_entry riak_output_object_list_methods[] = {
    PHP_ME(Riak_Object_List, __construct, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Object_List, first, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, last, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, isEmpty, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, offsetExists, arginfo_riak_output_object_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, offsetGet, arginfo_riak_output_object_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, offsetSet, arginfo_riak_output_object_list_offset_set, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, offsetUnset, arginfo_riak_output_object_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, count, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Object_List, getIterator, arginfo_riak_output_object_list_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_output_object_list_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak", "ObjectList", riak_output_object_list_methods);
    riak_output_object_list_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_output_object_list_ce TSRMLS_CC, 3, spl_ce_ArrayAccess, spl_ce_Aggregate, spl_ce_Countable);
    zend_declare_property_null(riak_output_object_list_ce, "objects", sizeof("objects")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Riak\ObjectList->__construct()
Creates a new RiakObjectList */
PHP_METHOD(Riak_Object_List, __construct)
{
    zval* zobjects;
    // Start with an empty array
    MAKE_STD_ZVAL(zobjects);
    object_init_ex(zobjects, spl_ce_ArrayObject);
    zend_update_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, zobjects TSRMLS_CC);
    zval_ptr_dtor(&zobjects);
}
/* }}} */

/* {{{ proto Object|null Riak\ObjectList->first()
Get the first object in this list or null if list is empty*/
PHP_METHOD(Riak_Object_List, first)
{
    zval *zobjects, *ztmp, *zoffset;
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);

    MAKE_STD_ZVAL(zoffset);
    ZVAL_LONG(zoffset, 0);
    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD1(Riak_Object_List, offsetExists, ztmp, getThis(), zoffset);
    RETVAL_NULL();
    if (Z_TYPE_P(ztmp) == IS_BOOL && Z_BVAL_P(ztmp)) {
        // Read offset 0
        zval_ptr_dtor(&ztmp);
        ztmp = NULL;
        zend_call_method_with_1_params(&zobjects, spl_ce_ArrayObject, NULL, "offsetget", &ztmp, zoffset);
        RETVAL_ZVAL(ztmp, 0, 1);
    } else {
        zval_ptr_dtor(&ztmp);
        zend_call_method_with_0_params(&zobjects, spl_ce_ArrayObject, NULL, "getiterator", &ztmp);
        if (Z_TYPE_P(ztmp) == IS_OBJECT) {
            zval zvalidname, zcurrname, *zvalid;
            ZVAL_STRING(&zvalidname, "valid", 0);
            ZVAL_STRING(&zcurrname, "current", 0);
            MAKE_STD_ZVAL(zvalid);
            call_user_function(NULL, &ztmp, &zvalidname, zvalid, 0, NULL TSRMLS_CC);
            if (Z_TYPE_P(zvalid) == IS_BOOL && Z_BVAL_P(zvalid)) {
                zval *zresult;
                MAKE_STD_ZVAL(zresult);
                call_user_function(NULL, &ztmp, &zcurrname, zresult, 0, NULL TSRMLS_CC);
                if (Z_TYPE_P(zresult) == IS_OBJECT) {
                    RETVAL_ZVAL(zresult, 0, 1);
                }
            }
            zval_ptr_dtor(&zvalid);
        }
        zval_ptr_dtor(&ztmp);
    }
    zval_ptr_dtor(&zoffset);
}
/* }}} */


/* {{{ proto Object|null Riak\ObjectList->last()
Get the last object in this list or null if list is empty*/
PHP_METHOD(Riak_Object_List, last)
{
    zval *zList, *zArray, **zObject;

    zList = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);

    zend_call_method_with_0_params(&zList, NULL, NULL, "getArrayCopy", &zArray);
    zend_hash_internal_pointer_end(Z_ARRVAL_P(zArray));

    if (zend_hash_get_current_data(Z_ARRVAL_P(zArray), (void **) &zObject) == FAILURE) {

        zval_ptr_dtor(&zArray);
        RETVAL_NULL();

        return;
    }

    RETVAL_ZVAL(*zObject, 1, 1);
}
/* }}} */

/* {{{ proto bool Riak\ObjectList->isEmpty()
Is the list empty */
PHP_METHOD(Riak_Object_List, isEmpty)
{
    zval *zobjects, *zcount;
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zobjects, spl_ce_ArrayObject, NULL, "count", &zcount);
    if (Z_TYPE_P(zcount) == IS_LONG && Z_LVAL_P(zcount) > 0) {
        RETVAL_BOOL(0);
    } else {
        RETVAL_BOOL(1);
    }
    zval_ptr_dtor(&zcount);
}
/* }}} */

/* {{{ proto bool Riak\ObjectList->offsetExists($offset)
Whether a offset exists */
PHP_METHOD(Riak_Object_List, offsetExists)
{
    zval *zoffset, *zobjects, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zobjects, spl_ce_ArrayObject, NULL, "offsetexists", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto Object Riak\ObjectList->offsetGet($offset)
retrieve an object at offset */
PHP_METHOD(Riak_Object_List, offsetGet)
{
    zval *zoffset, *zobjects, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zobjects, spl_ce_ArrayObject, NULL, "offsetget", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto void Riak\ObjectList->offsetGet($offset)
Set an object at given offset */
PHP_METHOD(Riak_Object_List, offsetSet)
{
    zval *zoffset, *zvalue, *zobjects;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zO", &zoffset, &zvalue, riak_object_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_2_params(&zobjects, spl_ce_ArrayObject, NULL, "offsetset", NULL, zoffset, zvalue);
}

/* }}} */

/* {{{ proto void Riak\ObjectList->offsetUnset($offset)
Removes an object at offset from list */
PHP_METHOD(Riak_Object_List, offsetUnset)
{
    zval *zoffset, *zobjects;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zobjects, spl_ce_ArrayObject, NULL, "offsetunset", NULL, zoffset);
}
/* }}} */

/* {{{ proto int Riak\ObjectList->count()
How many objects in the list */
PHP_METHOD(Riak_Object_List, count)
{
    zval *zobjects, *zresult;
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zobjects, spl_ce_ArrayObject, NULL, "count", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ proto Traversable Riak\ObjectList->getIterator()
Retrieve an external iterator */
PHP_METHOD(Riak_Object_List, getIterator)
{
    zval *zobjects, *zresult;
    zobjects = zend_read_property(riak_output_object_list_ce, getThis(), "objects", sizeof("objects")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zobjects, spl_ce_ArrayObject, NULL, "getiterator", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */
