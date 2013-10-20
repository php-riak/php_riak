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
#include "module_function.h"
#include "riak/exception/exception.h"

zend_class_entry *riak_module_function_ce;


ZEND_BEGIN_ARG_INFO_EX(arginfo_module_function_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, module)
    ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_module_function_set, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_module_function_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_module_function_methods[] = {
    PHP_ME(RiakModuleFunction, __construct, arginfo_module_function_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakModuleFunction, getModule, arginfo_module_function_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakModuleFunction, setModule, arginfo_module_function_set, ZEND_ACC_PUBLIC)
    PHP_ME(RiakModuleFunction, getFunction, arginfo_module_function_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakModuleFunction, setFunction, arginfo_module_function_set, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_property_module_function_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property", "ModuleFunction", riak_module_function_methods);
    riak_module_function_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_module_function_ce, "module", sizeof("module")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_module_function_ce, "function", sizeof("function")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */


/* {{{ proto void Riak\Property\ModuleFunction->__construct(string $module, string $function)
Creates a new Riak\Property\ModuleFunction */
PHP_METHOD(RiakModuleFunction, __construct)
{
    char *module, *efunction;
    int module_len, efunction_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &module, &module_len, &efunction, &efunction_len) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }

    zend_update_property_stringl(riak_module_function_ce, getThis(), "module", sizeof("module")-1, module, module_len TSRMLS_CC);
    zend_update_property_stringl(riak_module_function_ce, getThis(), "function", sizeof("function")-1, efunction, efunction_len TSRMLS_CC);
}
/* }}} */

PHP_METHOD(RiakModuleFunction, getModule)
{
    RIAK_GETTER_STRING(riak_module_function_ce, "module")
}

PHP_METHOD(RiakModuleFunction, setModule)
{
    RIAK_SETTER_STRING(riak_module_function_ce, "module")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakModuleFunction, getFunction)
{
    RIAK_GETTER_STRING(riak_module_function_ce, "function")
}

PHP_METHOD(RiakModuleFunction, setFunction)
{
    RIAK_SETTER_STRING(riak_module_function_ce, "function")
    RIAK_RETURN_THIS
}

