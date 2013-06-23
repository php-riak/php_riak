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

#ifndef PHP_RIAK__H__
#define PHP_RIAK__H__

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#define PHP_RIAK_EXTNAME "riak"
#define PHP_RIAK_VERSION "0.1"

/*************************************************
* Macros
*************************************************/

#define RIAK_PUSH_PARAM(arg) zend_vm_stack_push(arg TSRMLS_CC)
#define RIAK_POP_PARAM() (void)zend_vm_stack_pop(TSRMLS_C)
#define RIAK_PUSH_EO_PARAM()
#define RIAK_POP_EO_PARAM()

#define RIAK_SETTER_LONG(CE, PROPERTY_NAME) \
    long val; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { return; } \
    zend_update_property_long(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val TSRMLS_CC);

#define RIAK_GETTER_LONG(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_LONG) { RETURN_LONG(Z_LVAL_P(ztmp)); } \
    RETURN_NULL();

#define RIAK_SETTER_STRING(CE, PROPERTY_NAME) \
    char* val; int val_len; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &val_len) == FAILURE) { return; } \
    zend_update_property_stringl(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val, val_len TSRMLS_CC);

#define RIAK_GETTER_STRING(CE, PROPERTY_NAME) \
    zval* ztmp = zend_read_property(CE, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_STRING) { RETURN_ZVAL(ztmp, 1, 0); } \
    RETURN_NULL();

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

extern struct RIACK_ALLOCATOR riack_php_allocator;
extern struct RIACK_ALLOCATOR riack_php_persistent_allocator;

extern int le_riak_connection_list;

ZEND_BEGIN_MODULE_GLOBALS(riak)
  long persistent_connections;
  long persistent_timeout;

  long open_connections;
  long open_connections_persistent;

  long reconnects;
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

PHP_MINIT_FUNCTION(riak);
PHP_MSHUTDOWN_FUNCTION(riak);

PHP_GINIT_FUNCTION(riak);
PHP_GSHUTDOWN_FUNCTION(riak);

void riak_throw_exception(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC);

#endif
