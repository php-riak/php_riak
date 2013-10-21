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
#include "erlang_function.h"
#include "base_function.h"

zend_class_entry *riak_mrfunction_erl_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_function_erl_toarray, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_erl_named_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, module)
    ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()


static zend_function_entry riak_mrfunction_erl_methods[] = {
    PHP_ME(RiakMrErlangFunction, __construct, arginfo_create_erl_named_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMrErlangFunction, toArray, arginfo_function_erl_toarray, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_map_reduce_functions_erlang_function_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Functions", "ErlangFunction", riak_mrfunction_erl_methods);
    riak_mrfunction_erl_ce = zend_register_internal_class_ex(&ce, riak_mrfunction_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_erl_ce, "module", sizeof("module")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_erl_ce, "function", sizeof("function")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\MapReduce\Function\ErlangFunction
*************************************************************/

/* {{{ proto void Riak\MapReduce\Function\ErlangFunction->__construct(string $module, string $function)
Creates a new ErlangFunction */
PHP_METHOD(RiakMrErlangFunction, __construct)
{
    char *module, *function;
    int modulelen, functionlen;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &module, &modulelen, &function, &functionlen) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_mrfunction_erl_ce, getThis(), "module", sizeof("module")-1, module, modulelen TSRMLS_CC);
    zend_update_property_stringl(riak_mrfunction_erl_ce, getThis(), "function", sizeof("function")-1, function, functionlen TSRMLS_CC);
}
/* }}} */

/* {{{ proto array Riak\MapReduce\Function\ErlangFunction->toArray()
Returns this Funtion as an array */
PHP_METHOD(RiakMrErlangFunction, toArray)
{
    zval *zarray, *zmodule, *zfunc;

    MAKE_STD_ZVAL(zarray);
    array_init(zarray);

    zmodule = zend_read_property(riak_mrfunction_erl_ce, getThis(), "module", sizeof("module")-1, 1 TSRMLS_CC);
    zfunc = zend_read_property(riak_mrfunction_erl_ce, getThis(), "function", sizeof("function")-1, 1 TSRMLS_CC);

    add_assoc_string_ex(zarray, "language", sizeof("language"), "erlang", 1);
    add_assoc_stringl_ex(zarray, "module", sizeof("module"), Z_STRVAL_P(zmodule), Z_STRLEN_P(zmodule), 1);
    add_assoc_stringl_ex(zarray, "function", sizeof("function"), Z_STRVAL_P(zfunc), Z_STRLEN_P(zfunc), 1);

    RETURN_ZVAL(zarray, 0, 1);
}
/* }}} */

