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
#include "req_configs.h"
#include "php_riak.h"

zend_class_entry *riak_get_config_ce;
zend_class_entry *riak_put_config_ce;
zend_class_entry *riak_req_config_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_set_head, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, return_head)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_config_get_head, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_req_config_methods[] = {
    PHP_ME(RiakRequestConfiguration, setReturnHead, arginfo_riak_get_config_set_head, ZEND_ACC_PUBLIC)
    PHP_ME(RiakRequestConfiguration, getReturnHead, arginfo_riak_get_config_get_head, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_get_config_methods[] = {
    {NULL, NULL, NULL}
};

static zend_function_entry riak_put_config_methods[] = {
    {NULL, NULL, NULL}
};

void riak_req_configs_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RiakRequestConfiguration", riak_req_config_methods);
    riak_req_config_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_bool(riak_req_config_ce, "returnHead", sizeof("returnHead")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "RiakGetRequestConfiguration", riak_get_config_methods);
    riak_get_config_ce = zend_register_internal_class_ex(&ce, riak_req_config_ce, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "RiakPutRequestConfiguration", riak_put_config_methods);
    riak_put_config_ce = zend_register_internal_class_ex(&ce, riak_req_config_ce, NULL TSRMLS_CC);
}
/* }}} */

zend_bool get_return_head(zval* zreqconfig TSRMLS_DC) /* {{{ */
{
    zval *zreturn_head;
    zreturn_head = zend_read_property(riak_req_config_ce, zreqconfig, "returnHead", sizeof("returnHead")-1, 1 TSRMLS_CC);
    return Z_BVAL_P(zreturn_head);
}
/* }}} */

/* {{{ proto void RiakRequestConfiguration->setReturnHead(bool $returnHead)
Sets return head option */
PHP_METHOD(RiakRequestConfiguration, setReturnHead)
{
    zend_bool return_head;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &return_head) == FAILURE) {
        return;
    }
    zend_update_property_bool(riak_req_config_ce, getThis(), "returnHead", sizeof("returnHead")-1, return_head TSRMLS_CC);
}
/* }}} */

/* {{{ proto void RiakRequestConfiguration->getReturnHead()
Gets return head option */
PHP_METHOD(RiakRequestConfiguration, getReturnHead)
{
    zend_bool return_head = get_return_head(getThis() TSRMLS_CC);
    RETURN_BOOL(return_head);
}
/* }}} */
