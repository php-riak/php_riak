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
#include "req_inputs.h"
#include "php_riak.h"

zend_class_entry *riak_get_input_ce;
zend_class_entry *riak_put_input_ce;
zend_class_entry *riak_delete_input_ce;
zend_class_entry *riak_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_head, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, return_head)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_r, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, r)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_pr, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_bq, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, basicQuorum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_notfoundok, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, notFoundOk)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_ifnotmod, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, ifModifiedVClock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_deleted_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, deletedVClock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_input_methods[] = {
    {NULL, NULL, NULL}
};

static zend_function_entry riak_get_input_methods[] = {
    PHP_ME(Riak_Input_GetInput, setReturnHead, arginfo_riak_get_config_set_head, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getReturnHead, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setR, arginfo_riak_get_config_set_r, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getR, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setPR, arginfo_riak_get_config_set_pr, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getPR, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setBasicQuorum, arginfo_riak_get_config_set_bq, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getBasicQuorum, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setNotFoundOk, arginfo_riak_get_config_set_notfoundok, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getNotFoundOk, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setIfModifiedVClock, arginfo_riak_get_config_set_ifnotmod, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getIfModifiedVClock, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, setReturnDeletedVClock, arginfo_riak_get_config_set_deleted_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_GetInput, getReturnDeletedVClock, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_put_input_methods[] = {
    {NULL, NULL, NULL}
};

static zend_function_entry riak_delete_input_methods[] = {
    {NULL, NULL, NULL}
};


void riak_req_inputs_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input","Input", riak_input_methods);
    riak_input_ce = zend_register_internal_class(&ce TSRMLS_CC);


    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "GetInput", riak_get_input_methods);
    riak_get_input_ce = zend_register_internal_class_ex(&ce, riak_input_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "r", sizeof("r")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "pr", sizeof("pr")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "basicQuorum", sizeof("basicQuorum")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "notFoundOk", sizeof("notFoundOk")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "ifModifiedVClock", sizeof("ifModifiedVClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "returnHead", sizeof("returnHead")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_input_ce, "deletedVClock", sizeof("deletedVClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "PutInput", riak_put_input_methods);
    riak_put_input_ce = zend_register_internal_class_ex(&ce, riak_input_ce, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "DeleteInput", riak_delete_input_methods);
    riak_delete_input_ce = zend_register_internal_class_ex(&ce, riak_input_ce, NULL TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\Input\GetInput
*************************************************************/

/* {{{ proto void Riak_Input_Input->setReturnHead(bool $returnHead)
Sets return head option */
PHP_METHOD(Riak_Input_GetInput, setReturnHead)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->getReturnHead()
Gets return head option */
PHP_METHOD(Riak_Input_GetInput, getReturnHead)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setR(int $r)
Set R value */
PHP_METHOD(Riak_Input_GetInput, setR)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "r");
}
/* }}} */

/* {{{ proto int Riak_Input_Input->getR()
Get R value */
PHP_METHOD(Riak_Input_GetInput, getR)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "r");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setPR(int $r)
Set PR value */
PHP_METHOD(Riak_Input_GetInput, setPR)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "pr");
}
/* }}} */

/* {{{ proto int Riak_Input_Input->getPR()
Get PR value */
PHP_METHOD(Riak_Input_GetInput, getPR)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "pr");
}
/* }}} */

/* {{{ proto bool Riak_Input_Input->getBasicQuorum()
Get basicQuerum */
PHP_METHOD(Riak_Input_GetInput, getBasicQuorum)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "basicQuorum");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setBasicQuorum(bool $basicQuorum)
Set basicQuerum value */
PHP_METHOD(Riak_Input_GetInput, setBasicQuorum)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "basicQuorum");
}
/* }}} */

/* {{{ proto bool Riak_Input_Input->getNotFoundOk()
Get not found ok */
PHP_METHOD(Riak_Input_GetInput, getNotFoundOk)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "notFoundOk");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setNotFoundOk(bool $notFoundOk)
Set not found ok */
PHP_METHOD(Riak_Input_GetInput, setNotFoundOk)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "notFoundOk");
}
/* }}} */

/* {{{ proto string Riak_Input_Input->getIfModifiedVClock()
Get IfModifiedVClock */
PHP_METHOD(Riak_Input_GetInput, getIfModifiedVClock)
{
    RIAK_GETTER_STRING(riak_get_input_ce, "ifModifiedVClock");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setIfModifiedVClock(string $ifModifiedVClock)
Set IfModifiedVClock */
PHP_METHOD(Riak_Input_GetInput, setIfModifiedVClock)
{
    RIAK_SETTER_STRING(riak_get_input_ce, "ifModifiedVClock");
}

/* {{{ proto bool Riak_Input_Input->getReturnDeletedVClock()
Get returns deleted vclock */
PHP_METHOD(Riak_Input_GetInput, getReturnDeletedVClock)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "deletedVClock");
}
/* }}} */

/* {{{ proto void Riak_Input_Input->setReturnDeletedVClock(bool $returnDeletedVClock)
Set returnDeletedVClock */
PHP_METHOD(Riak_Input_GetInput, setReturnDeletedVClock)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "deletedVClock");
}
