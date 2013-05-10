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
#include <php.h>
#include "php_riak.h"
#include "stats.h"

zend_class_entry *riak_stats_ce;


static zend_function_entry riak_stats_methods[] = {
	PHP_ME(RiakStats, activeConnections, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(RiakStats, activePersistentConnections, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(RiakStats, getNumReconnects, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHP_METHOD(RiakStats, activeConnections)
{
	RETURN_LONG(RIAK_GLOBAL(open_connections));
}

PHP_METHOD(RiakStats, activePersistentConnections)
{
	RETURN_LONG(RIAK_GLOBAL(open_connections_persistent));
}

PHP_METHOD(RiakStats, getNumReconnects)
{
	long reconnects = RIAK_GLOBAL(reconnects);
	RETURN_LONG(reconnects);
}

void riak_stats_init(TSRMLS_D)
{
	zend_class_entry ce;
 
	INIT_CLASS_ENTRY(ce, "RiakStats", riak_stats_methods);
	riak_stats_ce = zend_register_internal_class(&ce TSRMLS_CC);

}
