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
#include "php_riak.h"

zend_class_entry *riak_mrfunction_ce;
zend_class_entry *riak_mrfunction_js_ce;
zend_class_entry *riak_mrfunction_erl_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_getlang, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_toarray, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_js_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, named)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_anon_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_named_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrfunction_methods[] = {
    PHP_ME(RiakMrFunction, __construct, arginfo_mrfunction_js_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMrFunction, toArray, arginfo_mrfunction_toarray, ZEND_ACC_PUBLIC)
    ZEND_ABSTRACT_ME(RiakMrFunction, getLanguage, arginfo_mrfunction_getlang)

    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrfunction_js_methods[] = {

    PHP_ME(RiakMrJavascriptFunction, getLanguage, arginfo_mrfunction_getlang, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMrJavascriptFunction, anon,  arginfo_create_anon_ctor,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakMrJavascriptFunction, named, arginfo_create_named_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrfunction_erl_methods[] = {
    PHP_ME(RiakMrErlangFunction, getLanguage, arginfo_mrfunction_getlang, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMrErlangFunction, anon,  arginfo_create_anon_ctor,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakMrErlangFunction, named, arginfo_create_named_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};

void riak_mrfunctions_init(TSRMLS_D)
{
    zend_class_entry ce, jsce, erlce;

    INIT_CLASS_ENTRY(ce, "RiakMrFunction", riak_mrfunction_methods);
    riak_mrfunction_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_mrfunction_ce, "named", sizeof("named")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_ce, "source", sizeof("source")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_CLASS_ENTRY(jsce, "RiakMrJavascriptFunction", riak_mrfunction_js_methods);
    riak_mrfunction_js_ce = zend_register_internal_class_ex(&jsce, riak_mrfunction_ce, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(erlce, "RiakMrErlangFunction", riak_mrfunction_erl_methods);
    riak_mrfunction_erl_ce = zend_register_internal_class_ex(&erlce, riak_mrfunction_ce, NULL TSRMLS_CC);
}

zval* create_named_mr_function(zend_class_entry *classentry, zend_bool named, const char* source TSRMLS_DC)
{
    zval *znamed, *zsource, *zfunc;
    MAKE_STD_ZVAL(zfunc);
    MAKE_STD_ZVAL(znamed);
    MAKE_STD_ZVAL(zsource);

    ZVAL_STRING(zsource, source, 1);
    ZVAL_BOOL(znamed, named);

    object_init_ex(zfunc, classentry);
    CALL_METHOD2(RiakMrFunction, __construct, zfunc, zfunc, znamed, zsource);

    zval_ptr_dtor(&zsource);
    zval_ptr_dtor(&znamed);
    return zfunc;
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakMrFunction, __construct)
{
    zend_bool named;
    char* source;
    int sourcelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "bs", &named, &source, &sourcelen) == FAILURE) {
        return;
    }
    zend_update_property_bool(riak_mrfunction_js_ce, getThis(), "named", sizeof("named")-1, named TSRMLS_CC);
    zend_update_property_stringl(riak_mrfunction_js_ce, getThis(), "source", sizeof("source")-1, source, sourcelen TSRMLS_CC);
}


PHP_METHOD(RiakMrFunction, toArray)
{
    zend_bool named;
    zval *zarray, *znamed, *zsource, zlang, zfuncname;

    MAKE_STD_ZVAL(zarray);
    array_init(zarray);

    ZVAL_STRING(&zfuncname, "getLanguage", 0);
    call_user_function(NULL, &getThis(), &zfuncname, &zlang, 0, NULL TSRMLS_CC);

    add_assoc_stringl_ex(zarray, "language", sizeof("language"), Z_STRVAL(zlang), Z_STRLEN(zlang), 1);
    zval_dtor(&zlang);

    znamed = zend_read_property(riak_mrfunction_ce, getThis(), "named", strlen("named"), 1 TSRMLS_CC);
    named = Z_BVAL_P(znamed);
    zval_ptr_dtor(&znamed);

    zsource = zend_read_property(riak_mrfunction_ce, getThis(), "source", strlen("source"), 1 TSRMLS_CC);
    if (named) {
        add_assoc_stringl_ex(zarray, "name", sizeof("name"), Z_STRVAL_P(zsource), Z_STRLEN_P(zsource), 1);
    } else  {
        add_assoc_stringl_ex(zarray, "source", sizeof("source"), Z_STRVAL_P(zsource), Z_STRLEN_P(zsource), 1);
    }
    zval_ptr_dtor(&zsource);

    RETURN_ZVAL(zarray, 0, 1);
}

////////////////////////////////////////
// Javascript function

PHP_METHOD(RiakMrJavascriptFunction, named)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_mr_function(riak_mrfunction_js_ce, 1, szsource TSRMLS_CC);
    pefree(szsource, 0);
    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakMrJavascriptFunction, anon)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_mr_function(riak_mrfunction_js_ce, 0, szsource TSRMLS_CC);
    pefree(szsource, 0);

    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakMrJavascriptFunction, getLanguage)
{
    RETURN_STRING("javascript", 1);
}

////////////////////////////////////////
// Erlang function

PHP_METHOD(RiakMrErlangFunction, named)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_mr_function(riak_mrfunction_erl_ce, 1, szsource TSRMLS_CC);
    pefree(szsource, 0);
    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakMrErlangFunction, anon)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_mr_function(riak_mrfunction_erl_ce, 0, szsource TSRMLS_CC);
    pefree(szsource, 0);
    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakMrErlangFunction, getLanguage)
{
    RETURN_STRING("erlang", 1);
}
