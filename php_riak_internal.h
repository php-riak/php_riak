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


#ifndef PHP_RIAK_INTERNAL_H
#define PHP_RIAK_INTERNAL_H

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <php.h>
#include <riack.h>

/*************************************************
* Macros
*************************************************/

#define RIACK_RETRY_OP(RIACK_RESULT_VAR, CONN, OPERATION) { long retry_cnt; \
    retry_cnt = RIAK_GLOBAL(default_retries); \
    do {  \
        RIACK_RESULT_VAR = OPERATION; \
        if (RIACK_RESULT_VAR != RIACK_SUCCESS) { \
            CONN->needs_reconnect = 1; \
        } \
        retry_cnt--; \
    } while (RIACK_RESULT_VAR != RIACK_SUCCESS && retry_cnt >= 0 && ensure_connected(CONN TSRMLS_CC)); }

#define RIAK_PUSH_PARAM(arg) zend_vm_stack_push(arg TSRMLS_CC)
#define RIAK_POP_PARAM() (void)zend_vm_stack_pop(TSRMLS_C)
#define RIAK_PUSH_EO_PARAM()
#define RIAK_POP_EO_PARAM()

#define RIAK_RETURN_THIS RETURN_ZVAL(getThis(), 1, 0);

#define RIAK_GETTER_LONG(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_LONG) { RETURN_LONG(Z_LVAL_P(ztmp)); } \
    RETURN_NULL();

#define RIAK_GETTER_BOOL(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_BOOL) { RETURN_BOOL(Z_BVAL_P(ztmp)); } \
    RETURN_NULL();

#define RIAK_GETTER_ARRAY(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_ARRAY) { RETURN_ZVAL(ztmp, 1, 0); } \
    RETURN_NULL();

#define RIAK_GETTER_OBJECT(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_OBJECT) { RETURN_ZVAL(ztmp, 1, 0); } \
    RETURN_NULL();

#define RIAK_GETTER_MIXED(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    RETURN_ZVAL(ztmp, 1, 0);

#define RIAK_GETTER_DOUBLE(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_DOUBLE) { RETURN_DOUBLE(Z_DVAL_P(ztmp)); } \
    RETURN_NULL();

#define RIAK_GETTER_STRING(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_STRING) { RETURN_ZVAL(ztmp, 1, 0); } \
    RETURN_NULL();

#define RIAK_SETTER_LONG(CE, PROPERTY_NAME) \
    long val; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { return; } \
    zend_update_property_long(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val TSRMLS_CC);

#define RIAK_SETTER_BOOL(CE, PROPERTY_NAME) \
    zend_bool val; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val) == FAILURE) { return; } \
    zend_update_property_bool(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val TSRMLS_CC);

#define RIAK_SETTER_STRING(CE, PROPERTY_NAME) \
    char* val; int val_len; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &val_len) == FAILURE) { return; } \
    zend_update_property_stringl(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val, val_len TSRMLS_CC);

#define RIAK_SETTER_OPTIONAL_STRING(CE, PROPERTY_NAME) \
    char* val; int val_len; \
    val = 0; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &val, &val_len) == FAILURE) { return; } \
    if (val == 0) { \
        zend_update_property_null(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1 TSRMLS_CC); \
    } else { \
        zend_update_property_stringl(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val, val_len TSRMLS_CC); \
    }

#define RIAK_CALL_METHOD_BASE(classname, name) zim_##classname##_##name

#define RIAK_CALL_METHOD_HELPER(classname, name, retval, thisptr, num, param) \
  RIAK_PUSH_PARAM(param); RIAK_PUSH_PARAM((void*)num);                            \
  RIAK_PUSH_EO_PARAM();                                                      \
  RIAK_CALL_METHOD_BASE(classname, name)(num, retval, NULL, thisptr, 0 TSRMLS_CC); \
  RIAK_POP_EO_PARAM();                       \
  RIAK_POP_PARAM(); RIAK_POP_PARAM();

#define RIAK_CALL_METHOD(classname, name, retval, thisptr)                  \
  RIAK_CALL_METHOD_BASE(classname, name)(0, retval, NULL, thisptr, 0 TSRMLS_CC);

#define RIAK_CALL_METHOD1(classname, name, retval, thisptr, param1)         \
  RIAK_CALL_METHOD_HELPER(classname, name, retval, thisptr, 1, param1);

#define RIAK_CALL_METHOD2(classname, name, retval, thisptr, param1, param2) \
  RIAK_PUSH_PARAM(param1);                                                  \
  RIAK_CALL_METHOD_HELPER(classname, name, retval, thisptr, 2, param2);     \
  RIAK_POP_PARAM();

#define RIAK_CALL_METHOD3(classname, name, retval, thisptr, param1, param2, param3) \
  RIAK_PUSH_PARAM(param1); RIAK_PUSH_PARAM(param2);                              \
  RIAK_CALL_METHOD_HELPER(classname, name, retval, thisptr, 3, param3);     \
  RIAK_POP_PARAM(); RIAK_POP_PARAM();

#define CHECK_RIACK_STATUS_THROW_ON_ERROR(CONNECTION, STATUS) \
  if ( STATUS != RIACK_SUCCESS) { \
    CONNECTION->needs_reconnect = 1; \
    riak_throw_exception(CONNECTION->client,  STATUS TSRMLS_CC); \
  }

#define THROW_EXCEPTION_IF_CONNECTION_IS_NULL(CONN) \
  if ( ! CONN) { \
    zend_throw_exception(riak_connection_exception_ce, "Connection error", 1000 TSRMLS_CC); return; \
  }

#define THROW_EXCEPTION_IF_CONNECTION_IS_NULL_WITH_RETURN(CONN, RET) \
  if ( ! CONN) { \
    zend_throw_exception(riak_connection_exception_ce, "Connection error", 1000 TSRMLS_CC); return RET; \
  }

#define CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(CONNECTION, STATUS) \
  if ( STATUS != RIACK_SUCCESS) { \
    CONNECTION->needs_reconnect = 1; \
    riak_throw_exception(CONNECTION->client,  STATUS TSRMLS_CC); \
  return; }

#define GET_PROPERTY_INTO_RIACK_STR_OR_ELSE(CE, ZOBJ, KEY, ZVAL, RIACK_STR) ZVAL = zend_read_property(CE, ZOBJ, KEY, sizeof(KEY)-1, 1 TSRMLS_CC); \
     if (Z_TYPE_P(ZVAL) == IS_STRING) { \
       RIACK_STR.len = Z_STRLEN_P(ZVAL); \
       RIACK_STR.value = Z_STRVAL_P(ZVAL); } else

#ifdef ZTS
#include <TSRM.h>
# define RIAK_GLOBAL(v) TSRMG(riak_globals_id, zend_riak_globals *, v)
#else
# define RIAK_GLOBAL(v) (riak_globals.v)
#endif


/*************************************************
* Vars
*************************************************/

extern riack_allocator riack_php_allocator;
extern riack_allocator riack_php_persistent_allocator;

extern int le_riak_connection_list;

ZEND_BEGIN_MODULE_GLOBALS(riak)
  long persistent_connections;
  long persistent_timeout;

  long keep_alive;
  long recv_timeout;
  long send_timeout;

  long open_connections;
  long open_connections_persistent;

  long reconnects;
  long default_retries;
#ifdef ZTS
  MUTEX_T pool_mutex;
#endif
ZEND_END_MODULE_GLOBALS(riak)
ZEND_EXTERN_MODULE_GLOBALS(riak)

/*************************************************
* Functions
*************************************************/

void *riack_php_alloc(void *allocator_data, size_t size);
void riack_php_free (void *allocator_data, void *data);

void *riack_php_persistent_alloc(void *allocator_data, size_t size);
void riack_php_persistent_free (void *allocator_data, void *data);

void riak_throw_exception(riack_client* client, int errorStatus TSRMLS_DC);

#endif // PHP_RIAK_INTERNAL_H
