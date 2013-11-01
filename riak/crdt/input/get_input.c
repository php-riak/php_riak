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
#include "get_input.h"

zend_class_entry *riak_crdt_input_get_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_get_set_r, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, r)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_get_set_pr, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_get_set_not_foundok, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, notFoundOk)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_get_set_bq, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, basicQuorum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_input_get_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_crdt_input_get_methods[] = {
    PHP_ME(Riak_Crdt_Input_GetInput, setR, arginfo_riak_crdt_input_get_set_r, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, getR, arginfo_riak_crdt_input_get_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, setPR, arginfo_riak_crdt_input_get_set_pr, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, getPR, arginfo_riak_crdt_input_get_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, setBasicQuorum, arginfo_riak_crdt_input_get_set_bq, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, getBasicQuorum, arginfo_riak_crdt_input_get_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, setNotFoundOk, arginfo_riak_crdt_input_get_set_not_foundok, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Input_GetInput, getNotFoundOk, arginfo_riak_crdt_input_get_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_crdt_input_get_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Crdt\\Input", "GetInput", riak_crdt_input_get_methods);
    riak_crdt_input_get_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_crdt_input_get_ce, "r", sizeof("r")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_get_ce, "pr", sizeof("pr")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_get_ce, "basicQuorum", sizeof("basicQuorum")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_input_get_ce, "notFoundOk", sizeof("notFoundOk")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/* {{{ proto Riak\Input\Crdt\GetInput Riak\Crdt\Input\GetInput->setR(int $r)
Set R value */
PHP_METHOD(Riak_Crdt_Input_GetInput, setR)
{
    RIAK_SETTER_BOOL(riak_crdt_input_get_ce, "r");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int Riak\Crdt\Input\GetInput->getR()
Get R value */
PHP_METHOD(Riak_Crdt_Input_GetInput, getR)
{
    RIAK_GETTER_BOOL(riak_crdt_input_get_ce, "r");
}
/* }}} */

/* {{{ proto Riak\Crdt\Input\GetInput Riak\Crdt\Input\GetInput->setPR(int $r)
Set PR value */
PHP_METHOD(Riak_Crdt_Input_GetInput, setPR)
{
    RIAK_SETTER_BOOL(riak_crdt_input_get_ce, "pr");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int Riak\Crdt\Input\GetInput->getPR()
Get PR value */
PHP_METHOD(Riak_Crdt_Input_GetInput, getPR)
{
    RIAK_GETTER_BOOL(riak_crdt_input_get_ce, "pr");
}
/* }}} */


/* {{{ proto bool Riak\Crdt\Input\GetInput->getBasicQuorum()
Get basicQuerum */
PHP_METHOD(Riak_Crdt_Input_GetInput, getBasicQuorum)
{
    RIAK_GETTER_BOOL(riak_crdt_input_get_ce, "basicQuorum");
}
/* }}} */

/* {{{ proto Riak\Input\Crdt\GetInput Riak\Input\Crdt\GetInput->setBasicQuorum(bool $basicQuorum)
Set basicQuerum value */
PHP_METHOD(Riak_Crdt_Input_GetInput, setBasicQuorum)
{
    RIAK_SETTER_BOOL(riak_crdt_input_get_ce, "basicQuorum");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto bool Riak\Crdt\Input\GetInput->getNotFoundOk()
Get not found ok */
PHP_METHOD(Riak_Crdt_Input_GetInput, getNotFoundOk)
{
    RIAK_GETTER_BOOL(riak_crdt_input_get_ce, "notFoundOk");
}
/* }}} */

/* {{{ proto Riak\Crdt\Input\GetInput Riak\Crdt\Input\GetInput->setNotFoundOk(bool $notFoundOk)
Set not found ok */
PHP_METHOD(Riak_Crdt_Input_GetInput, setNotFoundOk)
{
    RIAK_SETTER_BOOL(riak_crdt_input_get_ce, "notFoundOk");
    RIAK_RETURN_THIS
}
/* }}} */

