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

#include "php_riak_internal.h"
#include "php_riak.h"
#include <php_ini.h>

#include "riak/pool.h"
#include "riak/connection.h"
#include "riak/object.h"
#include "riak/bucket.h"
#include "riak/bucket_properties.h"
#include "riak/pool_info.h"
#include "riak/link.h"
#include "riak_session.h"
#include "riak/exception/exception.h"
#include "riak/map_reduce/mapreduce.h"
#include "riak/map_reduce/phase/phase.h"
#include "riak/map_reduce/phase/map_phase.h"
#include "riak/map_reduce/phase/reduce_phase.h"
#include "riak/map_reduce/output/output.h"
#include "riak/map_reduce/output/stream_output.h"
#include "riak/map_reduce/input/input.h"
#include "riak/map_reduce/input/bucket_input.h"
#include "riak/map_reduce/input/key_data_list_input.h"
#include "riak/map_reduce/input/key_list_input.h"
#include "riak/map_reduce/functions/base_function.h"
#include "riak/map_reduce/functions/erlang_function.h"
#include "riak/map_reduce/functions/javascript_function.h"
#include "riak/input/input.h"
#include "riak/input/index_input.h"
#include "riak/input/delete_input.h"
#include "riak/input/get_input.h"
#include "riak/input/put_input.h"
#include "riak/query/index_query.h"
#include "riak/output/output.h"
#include "riak/output/get_output.h"
#include "riak/output/put_output.h"
#include "riak/output/key_stream_output.h"
#include "riak/output/index_result.h"
#include "riak/output/index_result_list.h"
#include "riak/output/index_output.h"
#include "riak/object_list.h"
#include "riak/search/search.h"
#include "riak/search/input/parameter_bag.h"
#include "riak/search/output/output.h"
#include "riak/search/output/document_output.h"
#include "riak/property/module_function.h"
#include "riak/property/commit_hook.h"
#include "riak/property/commit_hook_list.h"
#include "riak/property/replication_mode/replication_mode.h"

#ifdef PHP_RIAK_SESSION
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
  STD_PHP_INI_ENTRY("riak.persistent.connections", "20",   PHP_INI_ALL, OnUpdateLong, persistent_connections, zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.persistent.timeout",     "1800", PHP_INI_ALL, OnUpdateLong, persistent_timeout,     zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.socket.keep_alive",      "1",    PHP_INI_ALL, OnUpdateBool, keep_alive,             zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.socket.recv_timeout",    "10000",PHP_INI_ALL, OnUpdateLong, recv_timeout,           zend_riak_globals, riak_globals)
  STD_PHP_INI_ENTRY("riak.socket.send_timeout",    "10000",PHP_INI_ALL, OnUpdateLong, send_timeout,           zend_riak_globals, riak_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(riak) /* {{{ */
{
    REGISTER_INI_ENTRIES();
    riack_init();
    le_riak_connection_list = zend_register_list_destructors_ex(NULL, le_riak_connections_pefree, "Persistent clients", module_number);
    riak_connection_init(TSRMLS_C);
    riak_object_init(TSRMLS_C);
    riak_link_init(TSRMLS_C);
    riak_bucket_init(TSRMLS_C);
    riak_bucket_props_init(TSRMLS_C);
    riak_property_module_function_init(TSRMLS_C);
    riak_property_commit_hook_init(TSRMLS_C);
    riak_property_commit_hook_list_init(TSRMLS_C);
    riak_property_replication_mode_init(TSRMLS_C);

    riak_mapreduce_init(TSRMLS_C);
    riak_map_reduce_phase_phase_init(TSRMLS_C);
    riak_map_reduce_phase_map_phase_init(TSRMLS_C);
    riak_map_reduce_phase_reduce_phase_init(TSRMLS_C);
    riak_map_reduce_input_input_init(TSRMLS_C);
    riak_map_reduce_input_bucket_input_init(TSRMLS_C);
    riak_map_reduce_input_key_data_list_input_init(TSRMLS_C);
    riak_map_reduce_input_key_list_input_init(TSRMLS_C);
    riak_map_reduce_output_output_init(TSRMLS_C);
    riak_map_reduce_output_stream_output_init(TSRMLS_C);
    riak_map_reduce_functions_base_function_init(TSRMLS_C);
    riak_map_reduce_functions_erlang_function_init(TSRMLS_C);
    riak_map_reduce_functions_javascript_function_init(TSRMLS_C);

    riak_input_input_init(TSRMLS_C);
    riak_input_delete_input_init(TSRMLS_C);
    riak_input_get_input_init(TSRMLS_C);
    riak_input_put_input_init(TSRMLS_C);
    riak_input_index_input_init(TSRMLS_C);
    riak_query_index_query_init(TSRMLS_C);

    riak_output_init(TSRMLS_C);
    riak_output_get_output_init(TSRMLS_C);
    riak_output_put_output_init(TSRMLS_C);
    riak_output_stream_output_init(TSRMLS_C);
    riak_output_object_list_init(TSRMLS_C);
    riak_output_index_result_init(TSRMLS_C);
    riak_output_index_result_list_init(TSRMLS_C);
    riak_output_index_output_init(TSRMLS_C);

    riak_exceptions_init(TSRMLS_C);
    riak_poolinfo_init(TSRMLS_C);

    riak_search_init(TSRMLS_C);
    riak_search_input_parameterbag_init(TSRMLS_C);
    riak_search_output_output_init(TSRMLS_C);
    riak_search_output_document_output_init(TSRMLS_C);

#ifdef PHP_RIAK_SESSION
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
    riak_globals->keep_alive = 1;
    riak_globals->recv_timeout = 10000;
    riak_globals->send_timeout = 10000;
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
        zend_throw_exception(riak_communication_exception_ce, "Communication error", 1001 TSRMLS_CC);
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

void riack_php_free(void *ptr, void *data)/* {{{ */
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
