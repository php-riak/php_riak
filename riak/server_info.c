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
#include "server_info.h"
#include "exception/exception.h"
#include "connection.h"

zend_class_entry *riak_server_info_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_server_info_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_server_info_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_server_info_methods[] = {
    PHP_ME(Riak_Server_Info, __construct, arginfo_riak_server_info_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Server_Info, getNode, arginfo_riak_server_info_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Server_Info, getServerVersion, arginfo_riak_server_info_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_server_info_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak", "ServerInfo", riak_server_info_methods);
    riak_server_info_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_server_info_ce, "connection", sizeof("connection")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

PHP_METHOD(Riak_Server_Info, __construct)/* {{{ */
{
    zval* zconnection;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zconnection, riak_connection_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_server_info_ce, getThis(), "connection", sizeof("connection")-1, zconnection TSRMLS_CC);
}
/* }}} */

int riak_get_server_info_as_zvals(riak_connection **connection, zval** znode,
                                  zval **zserver_version, zval* zthis TSRMLS_DC)
{
    zval *zconn;
    int riackresult;
    RIACK_STRING node, server_version;
    *zserver_version = *znode = 0;
    node.len = server_version.len = 0;
    node.value = server_version.value = 0;

    zconn = zend_read_property(riak_server_info_ce, zthis, "connection", sizeof("connection")-1, 1 TSRMLS_CC);
    GET_RIAK_CONNECTION(zconn, *connection);
    riackresult = riack_server_info((*connection)->client, &node, &server_version);
    if (riackresult == RIACK_SUCCESS) {
        MAKE_STD_ZVAL(*znode);
        ZVAL_STRINGL(*znode, node.value, node.len, 1);
        MAKE_STD_ZVAL(*zserver_version);
        ZVAL_STRINGL(*zserver_version, server_version.value, server_version.len, 1);
    }
    RSTR_SAFE_FREE((*connection)->client, node);
    RSTR_SAFE_FREE((*connection)->client, server_version);
    return riackresult;
}

PHP_METHOD(Riak_Server_Info, getNode)/* {{{ */
{
    riak_connection *connection;
    zval *znode, *zserver;
    int riackresult;
    riackresult = riak_get_server_info_as_zvals(&connection, &znode, &zserver, getThis());
    if (zserver) {
        zval_ptr_dtor(&zserver);
    }
    if (riackresult != RIACK_SUCCESS) {
        CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackresult);
    } else {
        RETURN_ZVAL(znode, 0, 1);
    }
}
/* }}} */

PHP_METHOD(Riak_Server_Info, getServerVersion)/* {{{ */
{
    riak_connection *connection;
    zval *znode, *zserver;
    int riackresult;
    riackresult = riak_get_server_info_as_zvals(&connection, &znode, &zserver, getThis());
    if (znode) {
        zval_ptr_dtor(&znode);
    }
    if (riackresult != RIACK_SUCCESS) {
        CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackresult);
    } else {
        RETURN_ZVAL(zserver, 0, 1);
    }
}
/* }}} */
