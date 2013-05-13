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
#include "link.h"
#include "php_riak.h"

zend_class_entry *riak_link_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_link_ctor, 0, ZEND_RETURN_VALUE, 3)
    ZEND_ARG_INFO(0, tag)
    ZEND_ARG_INFO(0, bucket)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()


static zend_function_entry riak_link_methods[] = {
    PHP_ME(RiakLink, __construct, arginfo_link_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

void riak_link_init(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RiakLink", riak_link_methods);
    riak_link_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_link_ce, "tag", sizeof("tag")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_link_ce, "bucket", sizeof("bucket")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_link_ce, "key", sizeof("key")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

zval* create_link_object(const char* tag, const char *bucket, const char* key TSRMLS_DC)
{
    zval *zlink, *zbucket, *ztag, *zkey;

    MAKE_STD_ZVAL(zkey);
    ZVAL_STRING(zkey, key, 1);
    MAKE_STD_ZVAL(zbucket);
    ZVAL_STRING(zbucket, bucket, 1);
    MAKE_STD_ZVAL(ztag);
    ZVAL_STRING(ztag, tag, 1);

    MAKE_STD_ZVAL(zlink);
    object_init_ex(zlink, riak_link_ce);
    CALL_METHOD3(RiakLink, __construct, zlink, zlink, ztag, zbucket, zkey);

    zval_ptr_dtor(&zkey);
    zval_ptr_dtor(&zbucket);
    zval_ptr_dtor(&ztag);
    return zlink;
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakLink, __construct)
{
    char *key, *bucket, *tag;
    int keylen, bucketlen, taglen;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &tag, &taglen, &bucket, &bucketlen, &key, &keylen) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_link_ce, getThis(), "tag", sizeof("tag")-1, tag, taglen TSRMLS_CC);
    zend_update_property_stringl(riak_link_ce, getThis(), "bucket", sizeof("bucket")-1, bucket, bucketlen TSRMLS_CC);
    zend_update_property_stringl(riak_link_ce, getThis(), "key", sizeof("key")-1, key, keylen TSRMLS_CC);
}


