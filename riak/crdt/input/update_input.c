/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

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
#include "update_input.h"

zend_class_entry *riak_crdt_input_update_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_update_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_update_set_w, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, w)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_update_set_dw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, dw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_update_set_pw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pw)
ZEND_END_ARG_INFO()

static zend_function_entry riak_crdt_input_update_methods[] = {
    PHP_ME(Riak_Crdt_Input_UpdateInput, setW, arginfo_riak_crdt_input_update_set_w, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_UpdateInput, getW, arginfo_riak_crdt_input_update_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_UpdateInput, setDW, arginfo_riak_crdt_input_update_set_dw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_UpdateInput, getDW, arginfo_riak_crdt_input_update_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_UpdateInput, setPW, arginfo_riak_crdt_input_update_set_pw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_UpdateInput, getPW, arginfo_riak_crdt_input_update_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_crdt_input_update_input_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Crdt\\Input", "UpdateInput", riak_crdt_input_update_methods);
    riak_crdt_input_update_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_update_ce, "w", sizeof("w")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_update_ce, "dw", sizeof("dw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_update_ce, "pw", sizeof("pw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}/* }}} */


/* {{{ proto int|null Riak\Crdt\Input\UpdateInput->getW()
Gets W value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, getW)
{
    RIAK_GETTER_LONG(riak_crdt_input_update_ce, "w");
}
/* }}} */

/* {{{ proto Riak\Crdt\Input\UpdateInput Riak\Crdt\Input\UpdateInput->setW(int $w)
Set W value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, setW)
{
    RIAK_SETTER_LONG(riak_crdt_input_update_ce, "w");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Crdt\Input\UpdateInput->getDW()
Gets DW value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, getDW)
{
    RIAK_GETTER_LONG(riak_crdt_input_update_ce, "dw");
}
/* }}} */

/* {{{ proto Riak\Crdt\Input\UpdateInput Riak\Crdt\Input\UpdateInput->setDW(int $dw)
Set DW value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, setDW)
{
    RIAK_SETTER_LONG(riak_crdt_input_update_ce, "dw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Crdt\Input\UpdateInput->getPW()
Gets PW value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, getPW)
{
    RIAK_GETTER_LONG(riak_crdt_input_update_ce, "pw");
}
/* }}} */

/* {{{ proto Riak\Crdt\Input\UpdateInput Riak\Crdt\Input\UpdateInput->setPW(int $pw)
Set PW value */
PHP_METHOD(Riak_Crdt_Input_UpdateInput, setPW)
{
    RIAK_SETTER_LONG(riak_crdt_input_update_ce, "pw");
    RIAK_RETURN_THIS
}
/* }}} */

