/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

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

#include "connection.h"
#include <zend_exceptions.h>
#include "object.h"
#include "bucket.h"
#include "pool.h"
#include "exception/exception.h"

zend_class_entry *riak_connection_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_connection_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_connection_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_connection_get_bucket, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static zend_function_entry riak_connection_methods[] = {
    PHP_ME(RiakConnection, __construct, arginfo_riak_connection_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakConnection, ping, arginfo_riak_connection_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakConnection, getHost, arginfo_riak_connection_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakConnection, getPort, arginfo_riak_connection_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakConnection, getBucket, arginfo_riak_connection_get_bucket, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};



void riak_connection_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;
 
    INIT_NS_CLASS_ENTRY(ce, "Riak", "Connection", riak_connection_methods);

	ce.create_object = create_client_data;
    riak_connection_ce = zend_register_internal_class(&ce TSRMLS_CC);
  
    zend_declare_property_null(riak_connection_ce, "host", sizeof("host")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_connection_ce, "port", sizeof("port")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

zend_object_value create_client_data(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	client_data *tobj;
 
	tobj = emalloc(sizeof(client_data));

	memset(tobj, 0, sizeof(client_data));
	zend_object_std_init((zend_object *) &tobj->std, class_type TSRMLS_CC);

#if ZEND_MODULE_API_NO >= 20100525
	object_properties_init((zend_object*) &tobj->std, class_type);
#else
	{
		zval *tmp;
		zend_hash_copy(tobj->std.properties, &class_type->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
	}
#endif

	retval.handle = zend_objects_store_put(tobj, (zend_objects_store_dtor_t) zend_objects_destroy_object, 
		(zend_objects_free_object_storage_t) free_client_data, NULL TSRMLS_CC);
	retval.handlers = zend_get_std_object_handlers();

	return retval;
}
/* }}} */

void free_client_data(void *object TSRMLS_DC) /* {{{ */
{
	client_data* data = (client_data*)object;

	zend_object_std_dtor(&data->std TSRMLS_CC);

	if (data->connection) {
		release_connection(data->connection TSRMLS_CC);
	}

	efree(data);
}
/* }}} */

zval* create_client_object(char* host, long port TSRMLS_DC) /* {{{ */
{
    zval *zclient, *zhost, *zport;

    MAKE_STD_ZVAL(zhost);
    MAKE_STD_ZVAL(zport);
    ZVAL_STRING(zhost, host, 1);
    ZVAL_LONG(zport, port);

	MAKE_STD_ZVAL(zclient);
    object_init_ex(zclient, riak_connection_ce);
    RIAK_CALL_METHOD2(RiakConnection, __construct, zclient, zclient, zhost, zport);

    zval_ptr_dtor(&zhost);
    zval_ptr_dtor(&zport);

	return zclient;
}
/* }}} */

/* {{{ proto void RiakConnection->__construct(string $host, [int $port])
Create a new RiakConnection */
PHP_METHOD(RiakConnection, __construct)
{
	client_data *data;
    char *host;
	int hostLen;
	long port = DEFAULT_PORT;
    zval* zbucketarr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &hostLen, &port) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
		return;
	}

    zend_update_property_stringl(riak_connection_ce, getThis(), "host", sizeof("host")-1, host, hostLen TSRMLS_CC);
    zend_update_property_long(riak_connection_ce, getThis(), "port", sizeof("port")-1, port TSRMLS_CC);

    MAKE_STD_ZVAL(zbucketarr);
    array_init(zbucketarr);
    zend_update_property(riak_connection_ce, getThis(), "buckets", sizeof("buckets")-1, zbucketarr TSRMLS_CC);
    zval_ptr_dtor(&zbucketarr);

	data = (client_data*)zend_object_store_get_object(getThis() TSRMLS_CC);

	data->connection = take_connection(host, hostLen, port TSRMLS_CC);

	if (!data->connection) {
		zend_throw_exception(riak_connection_exception_ce, "Connection error", 1000 TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto void RiakConnection->ping()
Ping riak to see if it is alive, an exception is thrown if no response is received */
PHP_METHOD(RiakConnection, ping)
{
	int pingStatus;
	riak_connection *connection;

	GET_RIAK_CONNECTION(getThis(), connection);
	ensure_connected(connection TSRMLS_CC);

	pingStatus = riack_ping(connection->client);

	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, pingStatus);
}
/* }}} */

/* {{{ proto string RiakConnection->getHost()
Get this connections hostname */
PHP_METHOD(RiakConnection, getHost)
{
    RIAK_GETTER_STRING(riak_connection_ce, "host");
}
/* }}} */

/* {{{ proto int RiakConnection->getPort()
Get this connections port number */
PHP_METHOD(RiakConnection, getPort)
{
    RIAK_GETTER_LONG(riak_connection_ce, "port");
}
/* }}} */

/* {{{ proto Riak\Bucket RiakConnection->getBucket(string $bucketName)
Get a Riak\Bucket */
PHP_METHOD(RiakConnection, getBucket)
{
    char* name;
    int name_len;
    zval* zbucketarr, *zbucket;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }

    // Se if we have a bucket with that name already
    zbucketarr = zend_read_property(riak_connection_ce, getThis(), "buckets", sizeof("buckets")-1, 1 TSRMLS_CC);

    if (Z_TYPE_P(zbucketarr) == IS_ARRAY) {
        zval **ztmp;

        if (zend_hash_find(Z_ARRVAL_P(zbucketarr), name, name_len+1, (void**)&ztmp) == SUCCESS) {
            RETURN_ZVAL(*ztmp, 1, 0);
        }
    }

    // If we are here we did not find an existing bucket, create a new
    zbucket = create_bucket_object(getThis(), name, name_len TSRMLS_CC);
    
    RETURN_ZVAL(zbucket, 0, 1);
}
/* }}} */
