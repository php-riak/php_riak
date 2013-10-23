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

#include "get_output.h"
#include "output.h"

zend_class_entry *riak_get_output_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_get_output_methods[] = {
    PHP_ME(Riak_Output_GetOutput, isUnchanged, arginfo_riak_get_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_output_get_output_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "GetOutput", riak_get_output_methods);
    riak_get_output_ce = zend_register_internal_class_ex(&ce, riak_output_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_get_output_ce, "unchanged", sizeof("unchanged")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
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

