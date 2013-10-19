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

#include "req_outputs.h"
#include "object.h"

zend_class_entry *riak_output_ce;
zend_class_entry *riak_get_output_ce;
zend_class_entry *riak_put_output_ce;

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

static zend_function_entry riak_get_output_methods[] = {
    PHP_ME(Riak_Output_GetOutput, isUnchanged, arginfo_riak_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_put_output_methods[] = {
    {NULL, NULL, NULL}
};

void riak_req_outputs_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "Output", riak_output_methods);
    riak_output_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "objectList", sizeof("objectList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_output_ce, "key", sizeof("key")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "GetOutput", riak_get_output_methods);
    riak_get_output_ce = zend_register_internal_class_ex(&ce, riak_output_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_get_output_ce, "unchanged", sizeof("unchanged")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "PutOutput", riak_put_output_methods);
    riak_put_output_ce = zend_register_internal_class_ex(&ce, riak_output_ce, NULL TSRMLS_CC);
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
    array_init(zobjectlist);
    content_cnt = obj->content_count;
    for (i=0; i<content_cnt; ++i) {
        // Create a new Riak\Object for each content
        zval *zobject;
        MAKE_STD_ZVAL(zobject);
        object_init_ex(zobject, riak_object_ce);
        if (Z_TYPE_P(zkey) != IS_NULL) {
            RIAK_CALL_METHOD1(RiakObject, __construct, zobject, zobject, zkey);
        } else {
            RIAK_CALL_METHOD(RiakObject, __construct, zobject, zobject);
        }
        set_object_from_riak_content(zobject, &obj->content[i] TSRMLS_CC);
        add_next_index_zval(zobjectlist, zobject);
    }
    zend_update_property(riak_output_ce, zoutput, "objectList", sizeof("objectList")-1, zobjectlist TSRMLS_CC);
    zval_ptr_dtor(&zobjectlist);
}
/* }}} */

zval *put_output_from_riack_object(struct RIACK_OBJECT* obj, zval* zkey TSRMLS_DC) /* {{{ */
{
    zval *zoutput;
    MAKE_STD_ZVAL(zoutput);
    object_init_ex(zoutput, riak_put_output_ce);
    riak_set_output_properties(zoutput, zkey, obj TSRMLS_CC);
    return zoutput;
}
/* }}} */

zval *get_output_from_riack_get_object(struct RIACK_GET_OBJECT* getobj, zval* zkey TSRMLS_DC) /* {{{ */
{
    zval *zoutput;
    MAKE_STD_ZVAL(zoutput);
    object_init_ex(zoutput, riak_get_output_ce);
    if (getobj->unchanged_present) {
        zend_update_property_bool(riak_get_output_ce, zoutput, "unchanged", sizeof("unchanged")-1, getobj->unchanged TSRMLS_CC);
    }
    riak_set_output_properties(zoutput, zkey, &getobj->object TSRMLS_CC);
    return zoutput;
}
/* }}} */

/*************************************************************
* Implementation: Riak\Output\Output
*************************************************************/

/* {{{ proto Riak\Object[] Riak\Output\Output->getObjectList()
Get list of returned objects */
PHP_METHOD(Riak_Output_Output, getObjectList)
{
    RIAK_GETTER_ARRAY(riak_output_ce, "objectList")
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
    if (Z_TYPE_P(zlist) == IS_ARRAY) {
        int elems = zend_hash_num_elements(Z_ARRVAL_P(zlist));
        if (elems > 1) {
            RETURN_BOOL(1);
        }
    }
    RETURN_BOOL(0);
}
/* }}} */

/* {{{ proto bool Riak\Output\Output->hasObject()
Does this output have atleast one object */
PHP_METHOD(Riak_Output_Output, hasObject)
{
    zval* zlist = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlist) == IS_ARRAY) {
        int elems = zend_hash_num_elements(Z_ARRVAL_P(zlist));
        if (elems > 0) {
            RETURN_BOOL(1);
        }
    }
    RETURN_BOOL(0);
}
/* }}} */

/* {{{ proto Riak\Object|null Riak\Output\Output->getFirstObject()
Get first object in objectlist */
PHP_METHOD(Riak_Output_Output, getFirstObject)
{
    // TODO
    zval* zlist = zend_read_property(riak_output_ce, getThis(), "objectList", sizeof("objectList")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlist) == IS_ARRAY) {
        int elems = zend_hash_num_elements(Z_ARRVAL_P(zlist));
        if (elems > 0) {
            zval **zfirst;
            if (zend_hash_index_find(Z_ARRVAL_P(zlist), 0, (void**)&zfirst) == SUCCESS) {
                RETURN_ZVAL(*zfirst, 1, 0);
            }
        }
    }
    RETURN_NULL();
}
/* }}} */


/*************************************************************
* Implementation: Riak\Output\GetOutput
*************************************************************/

/* {{{ proto bool|null Riak\Output\GetOutput->isUnchanged()
Is unchanged */
PHP_METHOD(Riak_Output_GetOutput, isUnchanged)
{
    zval* zunc = zend_read_property(riak_get_output_ce, getThis(), "unchanged", sizeof("unchanged")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zunc) == IS_BOOL) {
        RETURN_BOOL(Z_BVAL_P(zunc));
    }
    RETURN_BOOL(0);
}
/* }}} */
