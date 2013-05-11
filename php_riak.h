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

#include <riack.h>

#define PHP_RIAK_EXTNAME "riak"
#define PHP_RIAK_VERSION "0.1"

/////////////////////////////////////////////////
// Macros

#define PUSH_PARAM(arg) zend_vm_stack_push(arg TSRMLS_CC)
#define POP_PARAM() (void)zend_vm_stack_pop(TSRMLS_C)
#define PUSH_EO_PARAM()
#define POP_EO_PARAM()
 
#define CALL_METHOD_BASE(classname, name) zim_##classname##_##name
 
#define CALL_METHOD_HELPER(classname, name, retval, thisptr, num, param) \
  PUSH_PARAM(param); PUSH_PARAM((void*)num);                            \
  PUSH_EO_PARAM();                                                      \
  CALL_METHOD_BASE(classname, name)(num, retval, NULL, thisptr, 0 TSRMLS_CC); \
  POP_EO_PARAM();                       \
  POP_PARAM(); POP_PARAM();
 
#define CALL_METHOD(classname, name, retval, thisptr)                  \
  CALL_METHOD_BASE(classname, name)(0, retval, NULL, thisptr, 0 TSRMLS_CC);
 
#define CALL_METHOD1(classname, name, retval, thisptr, param1)         \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 1, param1);
 
#define CALL_METHOD2(classname, name, retval, thisptr, param1, param2) \
  PUSH_PARAM(param1);                                                  \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 2, param2);     \
  POP_PARAM();
 
#define CALL_METHOD3(classname, name, retval, thisptr, param1, param2, param3) \
  PUSH_PARAM(param1); PUSH_PARAM(param2);                              \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 3, param3);     \
  POP_PARAM(); POP_PARAM();

#define CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(CONNECTION, STATUS) \
  if ( STATUS != RIACK_SUCCESS) { \
    CONNECTION->needs_reconnect = 1; \
    throw_exception(CONNECTION->client,  STATUS TSRMLS_CC); \
  return; }

#define HASH_GET_INTO_RIACK_STRING_OR_ELSE(CE, ZOBJ, KEY, ZVAL, RIACK_STR) ZVAL = zend_read_property(CE, ZOBJ, KEY, sizeof(KEY)-1, 1 TSRMLS_CC); \
     if (Z_TYPE_P(ZVAL) == IS_STRING) { \
       RIACK_STR.len = Z_STRLEN_P(ZVAL); \
       RIACK_STR.value = Z_STRVAL_P(ZVAL); } else

#ifdef ZTS
#include <TSRM.h>
# define RIAK_GLOBAL(v) TSRMG(riak_globals_id, zend_riak_globals *, v)
#else
# define RIAK_GLOBAL(v) (riak_globals.v)
#endif


/////////////////////////////////////////////////
// Vars
/////////////////////////////////////////////////

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

ZEND_EXTERN_MODULE_GLOBALS(riak);

/////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////

void *riack_php_alloc(void *allocator_data, size_t size);
void riack_php_free (void *allocator_data, void *data);

void *riack_php_persistent_alloc(void *allocator_data, size_t size);
void riack_php_persistent_free (void *allocator_data, void *data);

/////////////////////////////////////////////////

PHP_MINIT_FUNCTION(riak);
PHP_MSHUTDOWN_FUNCTION(riak);

PHP_GINIT_FUNCTION(riak);
PHP_GSHUTDOWN_FUNCTION(riak);

void throw_exception(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC);
