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
#include <php_ini.h>
#include <riack.h>
#include "php_riak.h"

#include "pool.h"
#include "client.h"
#include "object.h"
#include "bucket.h"
#include "bucket_properties.h"
#include "exceptions.h"
#include "stats.h"
#include "link.h"
#include "riak_session.h"
#include "mapreduce.h"
#include "mr_phase.h"
#include "mr_functions.h"
#include "mr_inputs.h"
#include "mr_result.h"
#include "streaming.h"
#include "req_configs.h"

#ifdef PHP_SESSION
  extern ps_module ps_mod_riak;
#endif

int le_riak_connection_list;

ZEND_DECLARE_MODULE_GLOBALS(riak)

zend_function_entry riak_functions[] = {
  { NULL, NULL, NULL }
};

struct RIACK_ALLOCATOR riack_php_persistent_allocator = 
{
  riack_php_persistent_alloc,
  riack_php_persistent_free,
};

struct RIACK_ALLOCATOR riack_php_allocator =
{
  riack_php_alloc,
  riack_php_free
};

zend_module_entry riak_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_RIAK_EXTNAME,
  riak_functions,
  PHP_MINIT(riak),
  PHP_MSHUTDOWN(riak),
  NULL,
  NULL,
  NULL,
  PHP_RIAK_VERSION,
  PHP_MODULE_GLOBALS(riak),
  PHP_GINIT(riak),
  PHP_GSHUTDOWN(riak),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

ZEND_GET_MODULE(riak)

PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("riak.persistent.connections", "20", PHP_INI_ALL, OnUpdateLong, persistent_connections, zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.persistent.timeout", "5", PHP_INI_ALL,   OnUpdateLong, persistent_timeout,     zend_riak_globals, riak_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(riak) /* {{{ */
{
    REGISTER_INI_ENTRIES();
    riack_init();
    le_riak_connection_list = zend_register_list_destructors_ex(NULL, le_riak_connections_pefree, "Persistent clients", module_number);
    riak_client_init(TSRMLS_C);
    riak_object_init(TSRMLS_C);
    riak_link_init(TSRMLS_C);
    riak_bucket_init(TSRMLS_C);
    riak_bucket_props_init(TSRMLS_C);

    riak_mapreduce_init(TSRMLS_C);
    riak_mrphase_init(TSRMLS_C);
    riak_mrfunctions_init(TSRMLS_C);
    riak_mrinputs_init(TSRMLS_C);
    riak_mrresult_init(TSRMLS_C);

    riak_exceptions_init(TSRMLS_C);
    riak_stats_init(TSRMLS_C);
    riak_streaming_init(TSRMLS_C);
    riak_req_configs_init(TSRMLS_C);

#ifdef PHP_SESSION
    php_session_register_module(&ps_mod_riak);
#endif
    return SUCCESS;
}
/* {{{ */

PHP_MSHUTDOWN_FUNCTION(riak) /* {{{ */
{
    riack_cleanup();
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}
/* {{{ */

PHP_GINIT_FUNCTION(riak) /* {{{ */
{
    riak_globals->persistent_connections = 20;
    riak_globals->persistent_timeout = 5;
    riak_globals->open_connections = 0;
    riak_globals->open_connections_persistent = 0;
    riak_globals->reconnects = 0;
#ifdef ZTS
    riak_globals->pool_mutex = tsrm_mutex_alloc();
#endif
}
/* {{{ */

PHP_GSHUTDOWN_FUNCTION(riak) /* {{{ */
{
#ifdef ZTS
    tsrm_mutex_free(riak_globals->pool_mutex);
#endif
}
/* {{{ */
void riak_throw_exception(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC)/* {{{ */
{
    if (errorStatus == RIACK_ERROR_COMMUNICATION) {
        zend_throw_exception(riak_communication_exception_ce, "Communcation error", 1001 TSRMLS_CC);
    } else if (errorStatus == RIACK_ERROR_RESPONSE) {
        if (client->last_error) {
            zend_throw_exception(riak_response_exception_ce, client->last_error, 1002 TSRMLS_CC);
        } else {
            zend_throw_exception(riak_response_exception_ce, "Unexpected response from riak", 1002 TSRMLS_CC);
        }
    }
}
/* }}} */

/*
 * Riack allocator
*/

void *riack_php_alloc(void* ptr, size_t size)/* {{{ */
{
    if (size == 0) {
        return 0;
    }
    return pemalloc(size, 0);
}
/* }}} */

void riack_php_free (void *ptr, void *data)/* {{{ */
{
    if (data) {
        pefree(data, 0);
    }
}
/* }}} */

void *riack_php_persistent_alloc(void *ptr, size_t size)/* {{{ */
{
    if (size == 0) {
        return 0;
    }
    return pemalloc(size, 1);
}
/* }}} */

void riack_php_persistent_free (void *ptr, void *data)/* {{{ */
{
    if (data) {
        pefree(data, 1);
    }
}
/* }}} */
