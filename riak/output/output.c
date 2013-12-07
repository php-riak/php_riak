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
#include "riak/input/get_input.h"
#include "riak/input/put_input.h"
#include "get_output.h"
#include "put_output.h"
#include "riak/exception/exception.h"
#include "zend_interfaces.h"

zend_class_entry *riak_output_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_output_methods[] = {
    PHP_ME(Riak_Output_Output, getObjectList, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getVClock, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getKey, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, hasSiblings, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, hasObject, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Output_Output, getObject, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
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
    zend_declare_property_null(riak_output_ce, "bucket", sizeof("bucket")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "conflictResolver", sizeof("conflictResolver")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
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
    RIAK_CALL_METHOD(Riak_Object_List, __construct, NULL, zobjectlist);
    content_cnt = obj->content_count;
    for (i=0; i<content_cnt; ++i) {
        // Create a new Riak\Object for each content
        zval *zobject, *zoffset;
        MAKE_STD_ZVAL(zobject);
        object_init_ex(zobject, riak_object_ce);

        if (Z_TYPE_P(zkey) != IS_NULL) {
            RIAK_CALL_METHOD1(RiakObject, __construct, NULL, zobject, zkey);
        } else {
            RIAK_CALL_METHOD(RiakObject, __construct, NULL, zobject);
        }

        if (obj->vclock.len > 0) {
            zend_update_property_stringl(riak_object_ce, zobject, "vClock", sizeof("vClock")-1,
                                     (char*)obj->vclock.clock, obj->vclock.len TSRMLS_CC);
        }

        set_object_from_riak_content(zobject, &obj->content[i] TSRMLS_CC);

        //add_next_index_zval(zobjectlist, zobject);
        MAKE_STD_ZVAL(zoffset);
        ZVAL_LONG(zoffset, i);
        RIAK_CALL_METHOD2(Riak_Object_List, offsetSet, NULL, zobjectlist, zoffset, zobject);
        zval_ptr_dtor(&zobject);
        zval_ptr_dtor(&zoffset);
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

/* {{{ proto Riak\Object|null Riak\Output\Output->getObject()
Retrieves a unique riak object */
PHP_METHOD(Riak_Output_Output, getObject)
{
    zval *zCount, *zResolver, *zObjectList, *zObject;
    zObjectList = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zObjectList) != IS_OBJECT) {
        zval_ptr_dtor(&zObjectList);
        RETURN_NULL();
    }

    zend_call_method_with_0_params(&zObjectList, NULL, NULL, "count", &zCount);
    if (Z_LVAL_P(zCount) == 0) {
        zval_ptr_dtor(&zCount);
        RETURN_NULL();
    }

    if (Z_LVAL_P(zCount) == 1) {
        zval *zFirst;
        zend_call_method_with_0_params(&zObjectList, NULL, NULL, "first", &zFirst);
        zval_ptr_dtor(&zCount);
        RETURN_ZVAL(zFirst, 0, 1);
    }

    if (Z_LVAL_P(zCount) > 1) {
        zResolver = zend_read_property(riak_output_ce, getThis(), "conflictResolver", sizeof("conflictResolver")-1, 1 TSRMLS_CC);
        if (Z_TYPE_P(zResolver) != IS_OBJECT) {
            zval_ptr_dtor(&zCount);
            zend_throw_exception(riak_nonunique_exception_ce, "GetOutput contains unresolved siblings", 500 TSRMLS_CC);
            return;
        }
    }

    zval_ptr_dtor(&zCount);
    zend_call_method_with_1_params(&zResolver, NULL, NULL, "resolve", &zObject, zObjectList);

    if (Z_TYPE_P(zObject) != IS_OBJECT || ! instanceof_function(Z_OBJCE_P(zObject), riak_object_ce TSRMLS_CC)) {
        zval_ptr_dtor(&zObject);
        zend_throw_exception(riak_unresolvedconflict_exception_ce, "Conflict resolver should return a instance of Riak\\Object", 500 TSRMLS_CC);
        return;
    } else {
        zval *zObjectKey;

        zend_call_method_with_0_params(&zObject, NULL, NULL, "getKey", &zObjectKey);
        if (Z_TYPE_P(zObjectKey) != IS_STRING) {
            zval_ptr_dtor(&zObject);
            zval_ptr_dtor(&zObjectKey);
            zend_throw_exception(riak_unresolvedconflict_exception_ce, "The resolved Riak\\Object does not contain a valid key.", 500 TSRMLS_CC);
            return;
        }
        zval_ptr_dtor(&zObjectKey);
    }

    zval *zBucket = zend_read_property(riak_output_ce, getThis(), "bucket", sizeof("bucket")-1, 1 TSRMLS_CC);
    zval *zVClock = zend_read_property(riak_output_ce, getThis(), "vClock", sizeof("vClock")-1, 1 TSRMLS_CC);
    zval *zPutInput, *zOutput, *zBool;

    MAKE_STD_ZVAL(zPutInput);
    MAKE_STD_ZVAL(zBool);
    ZVAL_TRUE(zBool);
    object_init_ex(zPutInput, riak_put_input_ce);

    zend_call_method_with_1_params(&zPutInput, NULL, NULL, "setVClock", NULL, zVClock);
    zend_call_method_with_1_params(&zPutInput, NULL, NULL, "setReturnHead", NULL, zBool);
    zend_call_method_with_2_params(&zBucket, NULL, NULL, "put", &zOutput, zObject, zPutInput); // put the winner

    zval_ptr_dtor(&zBool);
    zval_ptr_dtor(&zPutInput);

    if (Z_TYPE_P(zOutput) == IS_NULL) {
        zval_ptr_dtor(&zOutput);
        zend_throw_exception(riak_unresolvedconflict_exception_ce, "Unable to put the conflict resolution", 500 TSRMLS_CC);
        return;
    }

    zend_call_method_with_0_params(&zOutput, NULL, NULL, "getVClock", &zVClock);
    zend_update_property(Z_OBJCE_P(zObject), zObject, "vClock", sizeof("vClock")-1, zVClock TSRMLS_CC);
    zval_ptr_dtor(&zVClock);
    zval_ptr_dtor(&zOutput);

    RETURN_ZVAL(zObject, 0, 1);
}
/* }}} */
