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
#include "put_input.h"
#include "input.h"

zend_class_entry *riak_put_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_head, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, return_head)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_w, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, w)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_dw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, dw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_pw, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, pw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vclock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_ret_body, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, return_body)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_if_none_m, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, if_none_match)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_set_if_not_modifed, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, if_not_modified)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_put_config_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_put_input_methods[] = {
    PHP_ME(Riak_Input_PutInput, setReturnHead, arginfo_riak_put_config_set_head, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getReturnHead, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setW, arginfo_riak_put_config_set_w, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getW, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setDW, arginfo_riak_put_config_set_dw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getDW, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setPW, arginfo_riak_put_config_set_pw, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getPW, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setVClock, arginfo_riak_put_config_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getVClock, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setReturnBody, arginfo_riak_put_config_set_ret_body, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getReturnBody, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setIfNoneMatch, arginfo_riak_put_config_set_if_none_m, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getIfNoneMatch, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, setIfNotModified, arginfo_riak_put_config_set_if_not_modifed, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_PutInput, getIfNotModified, arginfo_riak_put_config_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_input_put_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
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
}
/* }}} */


/*************************************************************
* Implementation: Riak\Input\PutInput
*************************************************************/

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setReturnHead(bool $returnHead)
Sets return head option */
PHP_METHOD(Riak_Input_PutInput, setReturnHead)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "returnHead");
    RIAK_RETURN_THIS
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

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setW(int $w)
Set W value */
PHP_METHOD(Riak_Input_PutInput, setW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "w");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Input\PutInput->getDW()
Gets DW value */
PHP_METHOD(Riak_Input_PutInput, getDW)
{
    RIAK_GETTER_LONG(riak_put_input_ce, "dw");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setDW(int $dw)
Set DW value */
PHP_METHOD(Riak_Input_PutInput, setDW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "dw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto int|null Riak\Input\PutInput->getPW()
Gets PW value */
PHP_METHOD(Riak_Input_PutInput, getPW)
{
    RIAK_GETTER_LONG(riak_put_input_ce, "pw");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setPW(int $pw)
Set PW value */
PHP_METHOD(Riak_Input_PutInput, setPW)
{
    RIAK_SETTER_LONG(riak_put_input_ce, "pw");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string|null Riak\Input\PutInput->getVClock()
Get the vector clock value */
PHP_METHOD(Riak_Input_PutInput, getVClock)
{
    RIAK_GETTER_STRING(riak_put_input_ce, "vClock");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setVClock(string $vClock)
Set the vector clock value */
PHP_METHOD(Riak_Input_PutInput, setVClock)
{
    RIAK_SETTER_STRING(riak_put_input_ce, "vClock");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto bool|null Riak\Input\PutInput->getReturnBody()
Get return body value */
PHP_METHOD(Riak_Input_PutInput, getReturnBody)
{
    RIAK_GETTER_STRING(riak_put_input_ce, "returnBody");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setReturnBody(bool $returnBody)
Set return body value */
PHP_METHOD(Riak_Input_PutInput, setReturnBody)
{
    RIAK_SETTER_STRING(riak_put_input_ce, "returnBody");
    RIAK_RETURN_THIS
}
/* }}} */


/* {{{ proto bool|null Riak\Input\PutInput->getIfNoneMatch()
Get if none match value */
PHP_METHOD(Riak_Input_PutInput, getIfNoneMatch)
{
    RIAK_GETTER_BOOL(riak_put_input_ce, "ifNoneMatch");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setIfNoneMatch(bool $ifNoneMatch)
Set if none match value */
PHP_METHOD(Riak_Input_PutInput, setIfNoneMatch)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "ifNoneMatch");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto bool|null Riak\Input\PutInput->getIfNotModified()
Get if not modified */
PHP_METHOD(Riak_Input_PutInput, getIfNotModified)
{
    RIAK_GETTER_BOOL(riak_put_input_ce, "ifNotModified");
}
/* }}} */

/* {{{ proto Riak\Input\PutInput Riak\Input\PutInput->setIfNotModified(bool $ifNotModified)
Set if not modified */
PHP_METHOD(Riak_Input_PutInput, setIfNotModified)
{
    RIAK_SETTER_BOOL(riak_put_input_ce, "ifNotModified");
    RIAK_RETURN_THIS
}
/* }}} */


