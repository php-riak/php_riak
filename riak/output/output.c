/*
   Copyright 2013: Kaspar Pedersen

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

#include "output.h"
#include "riak/object.h"
#include "riak/object_list.h"
#include "get_output.h"
#include "put_output.h"

zend_class_entry *riak_output_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_output_methods[] = {
    PHP_ME(Riak_Output_Output, getObjectList, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getVClock, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getKey, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, hasSiblings, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, hasObject, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getFirstObject, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_output_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "Output", riak_output_methods);
    riak_output_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "objectList", sizeof("objectList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "key", sizeof("key")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

void riak_set_output_properties(zval* zoutput, zval* zkey, struct RIACK_OBJECT* obj TSRMLS_DC) /* {{{ */
{
    size_t content_cnt, i;
    zval *zobjectlist;
    if (obj->vclock.len > 0) {
        zend_update_property_stringl(riak_output_ce, zoutput, "vClock", sizeof("vClock")-1,
                                     (char*)obj->vclock.clock, obj->vclock.len TSRMLS_CC);
    }
    if (Z_TYPE_P(zkey) == IS_STRING) {
        zend_update_property_stringl(riak_output_ce, zoutput, "key", sizeof("key")-1,
                                     Z_STRVAL_P(zkey), Z_STRLEN_P(zkey) TSRMLS_CC);
    }
    MAKE_STD_ZVAL(zobjectlist);
    object_init_ex(zobjectlist, riak_output_object_list_ce);
    RIAK_CALL_METHOD(Riak_Object_List, __construct, zobjectlist, zobjectlist);
    content_cnt = obj->content_count;
    for (i=0; i<content_cnt; ++i) {
        // Create a new Riak\Object for each content
        zval *zobject, zoffset;
        MAKE_STD_ZVAL(zobject);
        object_init_ex(zobject, riak_object_ce);
        if (Z_TYPE_P(zkey) != IS_NULL) {
            RIAK_CALL_METHOD1(RiakObject, __construct, zobject, zobject, zkey);
        } else {
            RIAK_CALL_METHOD(RiakObject, __construct, zobject, zobject);
        }
        set_object_from_riak_content(zobject, &obj->content[i] TSRMLS_CC);
        //add_next_index_zval(zobjectlist, zobject);
        ZVAL_LONG(&zoffset, i);
        RIAK_CALL_METHOD2(Riak_Object_List, offsetSet, NULL, zobjectlist, &zoffset, zobject);
        zval_ptr_dtor(&zobject);
    }
    zend_update_property(riak_output_ce, zoutput, "objectList", sizeof("objectList")-1, zobjectlist TSRMLS_CC);
    zval_ptr_dtor(&zobjectlist);
}
/* }}} */

/*************************************************************
* Implementation: Riak\Output\Output
*************************************************************/

/* {{{ proto Riak\Output\ObjectList Riak\Output\Output->getObjectList()
Get list of returned objects */
PHP_METHOD(Riak_Output_Output, getObjectList)
{
    RIAK_GETTER_OBJECT(riak_output_ce, "objectList")
}
/* }}} */

/* {{{ proto string|null Riak\Output\Output->getVClock()
Get vclock from riak if present */
PHP_METHOD(Riak_Output_Output, getVClock)
{
    RIAK_GETTER_STRING(riak_output_ce, "vClock")
}
/* }}} */

/* {{{ proto string|null Riak\Output\Output->getKey()
Get key if present */
PHP_METHOD(Riak_Output_Output, getKey)
{
    RIAK_GETTER_STRING(riak_output_ce, "key")
}
/* }}} */


/* {{{ proto bool|null Riak\Output\Output->hasSiblings()
Does this output have multiple objects */
PHP_METHOD(Riak_Output_Output, hasSiblings)
{
    zval* zlist = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    RETVAL_BOOL(0);
    if (Z_TYPE_P(zlist) == IS_OBJECT) {
        zval *zcount;
        MAKE_STD_ZVAL(zcount);
        RIAK_CALL_METHOD(Riak_Object_List, count, zcount, zlist);
        if (Z_TYPE_P(zcount) == IS_LONG && Z_LVAL_P(zcount) > 1) {
            RETVAL_BOOL(1);
        }
        zval_ptr_dtor(&zcount);
    }
}
/* }}} */

/* {{{ proto bool Riak\Output\Output->hasObject()
Does this output have atleast one object */
PHP_METHOD(Riak_Output_Output, hasObject)
{
    zval* zlist = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    RETVAL_BOOL(0);
    if (Z_TYPE_P(zlist) == IS_OBJECT) {
        zval *zisempty;
        RETVAL_BOOL(1);
        MAKE_STD_ZVAL(zisempty);
        RIAK_CALL_METHOD(Riak_Object_List, isEmpty, zisempty, zlist);
        if (Z_TYPE_P(zisempty) == IS_BOOL && Z_BVAL_P(zisempty)) {
            RETVAL_BOOL(0);
        }
        zval_ptr_dtor(&zisempty);
    }
}
/* }}} */

/* {{{ proto Riak\Object|null Riak\Output\Output->getFirstObject()
Get first object in objectlist */
PHP_METHOD(Riak_Output_Output, getFirstObject)
{
    zval* zlist = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlist) == IS_OBJECT) {
        zval* zresult;
        MAKE_STD_ZVAL(zresult);
        RIAK_CALL_METHOD(Riak_Object_List, first, zresult, zlist);
        RETURN_ZVAL(zresult, 0, 1);
    }
    RETURN_NULL();
}
/* }}} */
