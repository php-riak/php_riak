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

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrfunction_getlang, 0, ZEND_RETURN_VALUE, 0)
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
    ZEND_ABSTRACT_ME(RiakFunction, getLanguage, arginfo_mrfunction_getlang)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrfunction_js_methods[] = {
    PHP_ME(RiakJavascriptFunction, __construct, arginfo_mrfunction_js_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakJavascriptFunction, anon,  arginfo_create_anon_ctor,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakJavascriptFunction, named, arginfo_create_named_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(RiakJavascriptFunction, getLanguage, arginfo_mrfunction_getlang, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_mrfunctions_init(TSRMLS_D)
{
    zend_class_entry ce, jsce;

    INIT_CLASS_ENTRY(ce, "RiakFunction", riak_mrfunction_methods);
    riak_mrfunction_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_CLASS_ENTRY(jsce, "RiakJavascriptFunction", riak_mrfunction_js_methods);
    riak_mrfunction_js_ce = zend_register_internal_class_ex(&jsce, riak_mrfunction_ce, NULL TSRMLS_CC);

    zend_declare_property_null(riak_mrfunction_js_ce, "named", sizeof("named")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_mrfunction_js_ce, "source", sizeof("source")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

zval* create_named_js_function(zend_bool named, const char* source TSRMLS_DC)
{
    zval *znamed, *zsource, *zfunc;
    MAKE_STD_ZVAL(zfunc);
    MAKE_STD_ZVAL(znamed);
    MAKE_STD_ZVAL(zsource);

    ZVAL_STRING(zsource, source, 1);
    ZVAL_BOOL(znamed, named);

    object_init_ex(zfunc, riak_mrfunction_js_ce);
    CALL_METHOD2(RiakJavascriptFunction, __construct, zfunc, zfunc, znamed, zsource);

    zval_ptr_dtor(&zsource);
    zval_ptr_dtor(&znamed);
    return zfunc;
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakJavascriptFunction, __construct)
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

PHP_METHOD(RiakJavascriptFunction, named)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_js_function(0, szsource TSRMLS_CC);
    pefree(szsource, 0);
    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakJavascriptFunction, anon)
{
    char* source, *szsource;
    int sourcelen;
    zval* zfunc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &sourcelen) == FAILURE) {
        return;
    }
    szsource = pestrndup(source, sourcelen, 0);
    zfunc = create_named_js_function(1, szsource TSRMLS_CC);
    pefree(szsource, 0);
    RETURN_ZVAL(zfunc, 0, 1);
}

PHP_METHOD(RiakJavascriptFunction, getLanguage)
{
    RETURN_STRING("javascript", 1);
}
