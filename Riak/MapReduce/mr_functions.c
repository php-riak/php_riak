/*
   Copyright 2012 Trifork A/S
   Author: Kaspar Pedersen

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
#include "mr_functions.h"

zend_class_entry *riak_mrfunction_ce;
zend_class_entry *riak_mrfunction_js_ce;
zend_class_entry *riak_mrfunction_erl_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_toarray, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_js_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, named)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_anon_js, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_erl_named_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, module)
    ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrfunction_methods[] = {
    ZEND_ABSTRACT_ME(RiakMrFunction, toArray, arginfo_mrfunction_toarray)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrfunction_js_methods[] = {
    PHP_ME(RiakMrJavascriptFunction, __construct, arginfo_mrfunction_js_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMrJavascriptFunction, anon,  arginfo_create_anon_js,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakMrJavascriptFunction, named, arginfo_create_anon_js, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakMrJavascriptFunction, toArray, arginfo_mrfunction_toarray, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrfunction_erl_methods[] = {
    PHP_ME(RiakMrErlangFunction, __construct, arginfo_create_erl_named_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMrErlangFunction, toArray, arginfo_mrfunction_toarray, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_mrfunctions_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce, jsce, erlce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Functions", "BaseFunction", riak_mrfunction_methods);
    riak_mrfunction_ce = zend_register_internal_class(&ce TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(jsce, "Riak\\MapReduce\\Functions", "JavascriptFunction", riak_mrfunction_js_methods);
    riak_mrfunction_js_ce = zend_register_internal_class_ex(&jsce, riak_mrfunction_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_js_ce, "named", sizeof("named")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_js_ce, "source", sizeof("source")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(erlce, "Riak\\MapReduce\\Functions", "ErlangFunction", riak_mrfunction_erl_methods);
    riak_mrfunction_erl_ce = zend_register_internal_class_ex(&erlce, riak_mrfunction_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_erl_ce, "module", sizeof("module")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_erl_ce, "function", sizeof("function")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

void create_named_mr_js_function(zend_class_entry *classentry, zval* result,
                               zend_bool named, const char* source, int sourcelen TSRMLS_DC)/* {{{ */
{
    zval znamed, *zsource;
    MAKE_STD_ZVAL(zsource);
    ZVAL_STRINGL(zsource, source, sourcelen, 1);
    ZVAL_BOOL(&znamed, named);
    object_init_ex(result, classentry);
    RIAK_CALL_METHOD2(RiakMrJavascriptFunction, __construct, result, result, &znamed, zsource);
    zval_ptr_dtor(&zsource);
}
/* }}} */

/*************************************************************
* Implementation: Riak\MapReduce\Function\JavascriptFunction
*************************************************************/

/* {{{ proto void Riak\MapReduce\Function\JavascriptFunction->__construct(bool $named, string $source)
Create a new JavascriptFunction */
PHP_METHOD(RiakMrJavascriptFunction, __construct)
{
    zend_bool named;
    char* source;
    int sourcelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "bs", &named, &source, &sourcelen) == FAILURE) {
        return;
    }
    zend_update_property_bool(riak_mrfunction_ce, getThis(), "named", sizeof("named")-1, named TSRMLS_CC);
    zend_update_property_stringl(riak_mrfunction_ce, getThis(), "source", sizeof("source")-1, source, sourcelen TSRMLS_CC);
}
/* }}} */

/* {{{ proto array Riak\MapReduce\Function\JavascriptFunction->toArray()
Returns this Function as an array */
PHP_METHOD(RiakMrJavascriptFunction, toArray)
{
    zend_bool named;
    zval *zarray, *znamed, *zsource;

    MAKE_STD_ZVAL(zarray);
    array_init(zarray);

    add_assoc_string_ex(zarray, "language", sizeof("language"), "javascript", 1);

    znamed = zend_read_property(riak_mrfunction_ce, getThis(), "named", strlen("named"), 1 TSRMLS_CC);
    named = Z_BVAL_P(znamed);

    zsource = zend_read_property(riak_mrfunction_ce, getThis(), "source", strlen("source"), 1 TSRMLS_CC);
    if (named) {
        add_assoc_stringl_ex(zarray, "name", sizeof("name"), Z_STRVAL_P(zsource), Z_STRLEN_P(zsource), 1);
    } else  {
        add_assoc_stringl_ex(zarray, "source", sizeof("source"), Z_STRVAL_P(zsource), Z_STRLEN_P(zsource), 1);
    }

    RETURN_ZVAL(zarray, 0, 1);
}
/* }}} */

/* {{{ proto Riak\MapReduce\Function\JavascriptFunction Riak\MapReduce\Function\JavascriptFunction::named(string $name)
Creates a named JavascriptFunction */
PHP_METHOD(RiakMrJavascriptFunction, named)
{
    char* source; int sourcelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    create_named_mr_js_function(riak_mrfunction_js_ce, return_value, 1, source, sourcelen TSRMLS_CC);
}
/* }}} */

/* {{{ proto Riak\MapReduce\Function\JavascriptFunction Riak\MapReduce\Function\JavascriptFunction::anon(string $source)
Creates an anonymous JavascriptFunction */
PHP_METHOD(RiakMrJavascriptFunction, anon)
{
    char* source; int sourcelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    create_named_mr_js_function(riak_mrfunction_js_ce, return_value, 0, source, sourcelen TSRMLS_CC);
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
