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

#include "pool_info.h"

zend_class_entry *riak_poolinfo_ce;

static zend_function_entry riak_pool_info_methods[] = {
    PHP_ME(RiakPoolInfo, getNumActiveConnection, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    PHP_ME(RiakPoolInfo, getNumActivePersistentConnection, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    PHP_ME(RiakPoolInfo, getNumReconnect, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto int Riak\PoolInfo::getNumActiveConnection()
Returns number of active riak connections */
PHP_METHOD(RiakPoolInfo, getNumActiveConnection)
{
	RETURN_LONG(RIAK_GLOBAL(open_connections));
}
/* }}} */

/* {{{ proto int Riak\PoolInfo::getNumActivePersistentConnection()
Returns number of active persistent riak connections */
PHP_METHOD(RiakPoolInfo, getNumActivePersistentConnection)
{
	RETURN_LONG(RIAK_GLOBAL(open_connections_persistent));
}
/* }}} */

/* {{{ proto int Riak\PoolInfo::getNumReconnect()
How many reconnections has been performed, persistent and nonpersistent */
PHP_METHOD(RiakPoolInfo, getNumReconnect)
{
	long reconnects = RIAK_GLOBAL(reconnects);
	RETURN_LONG(reconnects);
}
/* }}} */

void riak_poolinfo_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;
 
    INIT_NS_CLASS_ENTRY(ce, "Riak", "PoolInfo", riak_pool_info_methods);
    riak_poolinfo_ce = zend_register_internal_class(&ce TSRMLS_CC);
}
/* }}} */
