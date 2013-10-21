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

#include "delete_input.h"
#include "input.h"

zend_class_entry *riak_delete_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_r, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, r)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_pr, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_w, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, w)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_dw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, dw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_pw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vclock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_set_rw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, rw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_del_config_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_delete_input_methods[] = {
    PHP_ME(Riak_Input_DeleteInput, setR, arginfo_riak_del_config_set_r, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getR, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setPR, arginfo_riak_del_config_set_pr, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getPR, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setRW, arginfo_riak_del_config_set_rw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getRW, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setW, arginfo_riak_del_config_set_w, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getW, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setDW, arginfo_riak_del_config_set_dw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getDW, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setPW, arginfo_riak_del_config_set_pw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getPW, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setVClock, arginfo_riak_del_config_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getVClock, arginfo_riak_del_config_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_input_delete_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "DeleteInput", riak_delete_input_methods);
    riak_delete_input_ce = zend_register_internal_class_ex(&ce, riak_input_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "r", sizeof("r")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "rw", sizeof("rw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "pr", sizeof("pr")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "w", sizeof("w")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "dw", sizeof("dw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "pw", sizeof("pw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_delete_input_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\Input\DeleteInput
*************************************************************/

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setR(int $r)
Set R value */
PHP_METHOD(Riak_Input_DeleteInput, setR)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "r");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getR()
Gets R value */
PHP_METHOD(Riak_Input_DeleteInput, getR)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "r")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setPR(int $pr)
Set PR value */
PHP_METHOD(Riak_Input_DeleteInput, setPR)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "pr");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getPR()
Gets PR value */
PHP_METHOD(Riak_Input_DeleteInput, getPR)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "pr")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setRW(int $rw)
Set RW value */
PHP_METHOD(Riak_Input_DeleteInput, setRW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "rw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getRW()
Gets RW value */
PHP_METHOD(Riak_Input_DeleteInput, getRW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "rw")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setW(int $w)
Set W value */
PHP_METHOD(Riak_Input_DeleteInput, setW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "w");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getW()
Gets W value */
PHP_METHOD(Riak_Input_DeleteInput, getW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "w")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setDW(int $dw)
Set DW value */
PHP_METHOD(Riak_Input_DeleteInput, setDW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "dw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getDW()
Gets DW value */
PHP_METHOD(Riak_Input_DeleteInput, getDW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "dw")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setPW(int $pw)
Set PW value */
PHP_METHOD(Riak_Input_DeleteInput, setPW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "pw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getPW()
Gets PW value */
PHP_METHOD(Riak_Input_DeleteInput, getPW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "pw")
}
/* }}} */

/* {{{ proto Riak\Input\DeleteInput Riak\Input\DeleteInput->setVClock(string $rw)
Set set vector clock */
PHP_METHOD(Riak_Input_DeleteInput, setVClock)
{
    RIAK_SETTER_STRING(riak_delete_input_ce, "vClock");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto null|string Riak\Input\PutInput->getVClock()
Gets vector clock */
PHP_METHOD(Riak_Input_DeleteInput, getVClock)
{
    RIAK_GETTER_STRING(riak_delete_input_ce, "vClock")
}
/* }}} */

