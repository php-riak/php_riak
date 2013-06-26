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
    ZEND_ARG_INFO(0, basic_quorum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_notfoundok, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, not_found_ok)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_ifnotmod, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, ifModified_vclock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_deleted_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, deleted_vclock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_w, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, w)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_dw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, dw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_pw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vclock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_ret_body, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, return_body)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_if_none_m, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, if_none_match)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_if_not_modifed, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, if_not_modified)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_xxx_config_set_rw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, rw)
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
    PHP_ME(Riak_Input_PutInput, setReturnHead, arginfo_riak_get_config_set_head, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getReturnHead, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setW, arginfo_riak_xxx_config_set_w, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setDW, arginfo_riak_xxx_config_set_dw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getDW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setPW, arginfo_riak_xxx_config_set_pw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getPW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setVClock, arginfo_riak_xxx_config_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getVClock, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setReturnBody, arginfo_riak_xxx_config_set_ret_body, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getReturnBody, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setIfNoneMatch, arginfo_riak_xxx_config_set_if_none_m, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getIfNoneMatch, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setIfNotModified, arginfo_riak_xxx_config_set_if_not_modifed, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getIfNotModified, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_delete_input_methods[] = {
    PHP_ME(Riak_Input_DeleteInput, setR, arginfo_riak_get_config_set_r, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getR, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setPR, arginfo_riak_get_config_set_pr, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getPR, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setRW, arginfo_riak_xxx_config_set_rw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getRW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setW, arginfo_riak_xxx_config_set_w, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setDW, arginfo_riak_xxx_config_set_dw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getDW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setPW, arginfo_riak_xxx_config_set_pw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getPW, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, setVClock, arginfo_riak_xxx_config_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_DeleteInput, getVClock, arginfo_riak_get_config_noargs, ZEND_ACC_PUBLIC)
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
    zend_declare_property_null(riak_put_input_ce, "w", sizeof("w")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "dw", sizeof("dw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "pw", sizeof("pw")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "returnHead", sizeof("returnHead")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "returnBody", sizeof("returnBody")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "ifNoneMatch", sizeof("ifNoneMatch")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_put_input_ce, "ifNotModified", sizeof("ifNotModified")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

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

/* {{{ proto void Riak\Input\DeleteInput->setR(int $r)
Set R value */
PHP_METHOD(Riak_Input_DeleteInput, setR)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "r")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getR()
Gets R value */
PHP_METHOD(Riak_Input_DeleteInput, getR)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "r")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setPR(int $pr)
Set PR value */
PHP_METHOD(Riak_Input_DeleteInput, setPR)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "pr")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getPR()
Gets PR value */
PHP_METHOD(Riak_Input_DeleteInput, getPR)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "pr")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setRW(int $rw)
Set RW value */
PHP_METHOD(Riak_Input_DeleteInput, setRW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "rw")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getRW()
Gets RW value */
PHP_METHOD(Riak_Input_DeleteInput, getRW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "rw")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setW(int $w)
Set W value */
PHP_METHOD(Riak_Input_DeleteInput, setW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "w")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getW()
Gets W value */
PHP_METHOD(Riak_Input_DeleteInput, getW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "w")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setDW(int $dw)
Set DW value */
PHP_METHOD(Riak_Input_DeleteInput, setDW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "dw")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getDW()
Gets DW value */
PHP_METHOD(Riak_Input_DeleteInput, getDW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "dw")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setPW(int $pw)
Set PW value */
PHP_METHOD(Riak_Input_DeleteInput, setPW)
{
    RIAK_SETTER_LONG(riak_delete_input_ce, "pw")
}
/* }}} */

/* {{{ proto null|int Riak\Input\PutInput->getPW()
Gets PW value */
PHP_METHOD(Riak_Input_DeleteInput, getPW)
{
    RIAK_GETTER_LONG(riak_delete_input_ce, "pw")
}
/* }}} */

/* {{{ proto void Riak\Input\DeleteInput->setVClock(string $rw)
Set set vector clock */
PHP_METHOD(Riak_Input_DeleteInput, setVClock)
{
    RIAK_SETTER_STRING(riak_delete_input_ce, "vClock")
}
/* }}} */

/* {{{ proto null|string Riak\Input\PutInput->getVClock()
Gets vector clock */
PHP_METHOD(Riak_Input_DeleteInput, getVClock)
{
    RIAK_GETTER_STRING(riak_delete_input_ce, "vClock")
}
/* }}} */

/*************************************************************
* Implementation: Riak\Input\PutInput
*************************************************************/

/* {{{ proto void Riak\Input\PutInput->setReturnHead(bool $returnHead)
Sets return head option */
PHP_METHOD(Riak_Input_PutInput, setReturnHead)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto bool|null Riak\Input\PutInput->getReturnHead()
Gets return head option */
PHP_METHOD(Riak_Input_PutInput, getReturnHead)
{
    RIAK_GETTER_BOOL(riak_put_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto int|null Riak\Input\PutInput->getW()
Gets W value */
PHP_METHOD(Riak_Input_PutInput, getW)
{
    RIAK_GETTER_LONG(riak_put_input_ce, "w");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setW(int $w)
Set W value */
PHP_METHOD(Riak_Input_PutInput, setW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "w");
}
/* }}} */

/* {{{ proto int|null Riak\Input\PutInput->getDW()
Gets DW value */
PHP_METHOD(Riak_Input_PutInput, getDW)
{
    RIAK_GETTER_LONG(riak_put_input_ce, "dw");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setDW(int $dw)
Set DW value */
PHP_METHOD(Riak_Input_PutInput, setDW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "dw");
}
/* }}} */

/* {{{ proto int|null Riak\Input\PutInput->getPW()
Gets PW value */
PHP_METHOD(Riak_Input_PutInput, getPW)
{
    RIAK_GETTER_LONG(riak_put_input_ce, "pw");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setPW(int $pw)
Set PW value */
PHP_METHOD(Riak_Input_PutInput, setPW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "pw");
}
/* }}} */

/* {{{ proto string|null Riak\Input\PutInput->getVClock()
Get the vector clock value */
PHP_METHOD(Riak_Input_PutInput, getVClock)
{
    RIAK_GETTER_STRING(riak_put_input_ce, "vClock");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setVClock(string $vClock)
Set the vector clock value */
PHP_METHOD(Riak_Input_PutInput, setVClock)
{
    RIAK_SETTER_STRING(riak_put_input_ce, "vClock");
}
/* }}} */

/* {{{ proto bool|null Riak\Input\PutInput->getReturnBody()
Get return body value */
PHP_METHOD(Riak_Input_PutInput, getReturnBody)
{
    RIAK_GETTER_STRING(riak_put_input_ce, "returnBody");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setReturnBody(bool $returnBody)
Set return body value */
PHP_METHOD(Riak_Input_PutInput, setReturnBody)
{
    RIAK_SETTER_STRING(riak_put_input_ce, "returnBody");
}
/* }}} */


/* {{{ proto bool|null Riak\Input\PutInput->getIfNoneMatch()
Get if none match value */
PHP_METHOD(Riak_Input_PutInput, getIfNoneMatch)
{
    RIAK_GETTER_BOOL(riak_put_input_ce, "ifNoneMatch");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setIfNoneMatch(bool $ifNoneMatch)
Set if none match value */
PHP_METHOD(Riak_Input_PutInput, setIfNoneMatch)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "ifNoneMatch");
}
/* }}} */

/* {{{ proto bool|null Riak\Input\PutInput->getIfNotModified()
Get if not modified */
PHP_METHOD(Riak_Input_PutInput, getIfNotModified)
{
    RIAK_GETTER_BOOL(riak_put_input_ce, "ifNotModified");
}
/* }}} */

/* {{{ proto void Riak\Input\PutInput->setIfNotModified(bool $ifNotModified)
Set if not modified */
PHP_METHOD(Riak_Input_PutInput, setIfNotModified)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "ifNotModified");
}
/* }}} */

/*************************************************************
* Implementation: Riak\Input\GetInput
*************************************************************/

/* {{{ proto void Riak\Input\GetInput->setReturnHead(bool $returnHead)
Sets return head option */
PHP_METHOD(Riak_Input_GetInput, setReturnHead)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->getReturnHead()
Gets return head option */
PHP_METHOD(Riak_Input_GetInput, getReturnHead)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "returnHead");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setR(int $r)
Set R value */
PHP_METHOD(Riak_Input_GetInput, setR)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "r");
}
/* }}} */

/* {{{ proto int Riak\Input\GetInput->getR()
Get R value */
PHP_METHOD(Riak_Input_GetInput, getR)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "r");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setPR(int $r)
Set PR value */
PHP_METHOD(Riak_Input_GetInput, setPR)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "pr");
}
/* }}} */

/* {{{ proto int Riak\Input\GetInput->getPR()
Get PR value */
PHP_METHOD(Riak_Input_GetInput, getPR)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "pr");
}
/* }}} */

/* {{{ proto bool Riak\Input\GetInput->getBasicQuorum()
Get basicQuerum */
PHP_METHOD(Riak_Input_GetInput, getBasicQuorum)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "basicQuorum");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setBasicQuorum(bool $basicQuorum)
Set basicQuerum value */
PHP_METHOD(Riak_Input_GetInput, setBasicQuorum)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "basicQuorum");
}
/* }}} */

/* {{{ proto bool Riak\Input\GetInput->getNotFoundOk()
Get not found ok */
PHP_METHOD(Riak_Input_GetInput, getNotFoundOk)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "notFoundOk");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setNotFoundOk(bool $notFoundOk)
Set not found ok */
PHP_METHOD(Riak_Input_GetInput, setNotFoundOk)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "notFoundOk");
}
/* }}} */

/* {{{ proto string Riak\Input\GetInput->getIfModifiedVClock()
Get IfModifiedVClock */
PHP_METHOD(Riak_Input_GetInput, getIfModifiedVClock)
{
    RIAK_GETTER_STRING(riak_get_input_ce, "ifModifiedVClock");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setIfModifiedVClock(string $ifModifiedVClock)
Set IfModifiedVClock */
PHP_METHOD(Riak_Input_GetInput, setIfModifiedVClock)
{
    RIAK_SETTER_STRING(riak_get_input_ce, "ifModifiedVClock");
}

/* {{{ proto bool Riak\Input\GetInput->getReturnDeletedVClock()
Get returns deleted vclock */
PHP_METHOD(Riak_Input_GetInput, getReturnDeletedVClock)
{
    RIAK_GETTER_BOOL(riak_get_input_ce, "deletedVClock");
}
/* }}} */

/* {{{ proto void Riak\Input\GetInput->setReturnDeletedVClock(bool $returnDeletedVClock)
Set returnDeletedVClock */
PHP_METHOD(Riak_Input_GetInput, setReturnDeletedVClock)
{
    RIAK_SETTER_BOOL(riak_get_input_ce, "deletedVClock");
}
