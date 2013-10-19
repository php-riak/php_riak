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
#include "commit_hook.h"
#include "riak/exceptions.h"

zend_class_entry *riak_commit_hook_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, moduleOrName)
    ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_set, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_commit_hook_methods[] = {
    PHP_ME(RiakCommitHook, __construct, arginfo_commit_hook_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakCommitHook, getJsName, arginfo_commit_hook_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHook, getErlModule, arginfo_commit_hook_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHook, getErlFunction, arginfo_commit_hook_set, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHook, isJavascript, arginfo_commit_hook_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHook, isErlang, arginfo_commit_hook_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_property_commit_hook_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property", "CommitHook", riak_commit_hook_methods);
    riak_commit_hook_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_commit_hook_ce, "moduleOrName", sizeof("moduleOrName")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_commit_hook_ce, "function", sizeof("function")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

}
/* }}} */


/* {{{ proto void Riak\Property\CommitHook->__construct(string $name, ModuleFunction $moduleFunction)
Creates a new Riak\Property\CommitHook */
PHP_METHOD(RiakCommitHook, __construct)
{
    char *mod_or_name, *fun;
    int mod_or_name_len, fun_len;
    fun_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &mod_or_name, &mod_or_name_len, &fun, &fun_len) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zend_update_property_stringl(riak_commit_hook_ce, getThis(), "moduleOrName", sizeof("moduleOrName")-1, mod_or_name, mod_or_name_len TSRMLS_CC);
    if (fun_len > 0) {
        zend_update_property_stringl(riak_commit_hook_ce, getThis(), "function", sizeof("function")-1, fun, fun_len TSRMLS_CC);
    }
}
/* }}} */

PHP_METHOD(RiakCommitHook, getJsName)
{
    RIAK_GETTER_STRING(riak_commit_hook_ce, "moduleOrName")
}

PHP_METHOD(RiakCommitHook, getErlModule)
{
    RIAK_GETTER_STRING(riak_commit_hook_ce, "moduleOrName")
}

PHP_METHOD(RiakCommitHook, getErlFunction)
{
    RIAK_GETTER_STRING(riak_commit_hook_ce, "function")
}

PHP_METHOD(RiakCommitHook, isJavascript)
{
    zval* ztmp = zend_read_property(riak_commit_hook_ce, getThis(), "function", sizeof("function")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(ztmp) == IS_STRING) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

PHP_METHOD(RiakCommitHook, isErlang)
{
    zval* ztmp = zend_read_property(riak_commit_hook_ce, getThis(), "function", sizeof("function")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(ztmp) == IS_STRING) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

