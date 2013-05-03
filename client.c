/*
   Copyright 2012 Trifork A/S
   Author: Kaspar Pedersen

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
#include <zend_exceptions.h>
#include "php_riak.h"
#include "client.h"
#include "object.h"
#include "exceptions.h"

#define GET_RIACK_CLIENT(VAR_NAME) struct RIACK_CLIENT* VAR_NAME = ((client_data*)zend_object_store_get_object(getThis() TSRMLS_CC))->client
#define CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(CLIENT, STATUS) if ( STATUS != RIACK_SUCCESS) { throwException(CLIENT,  STATUS TSRMLS_CC); return; }

zend_class_entry *riak_client_ce;

static zend_function_entry riak_client_methods[] = {
  PHP_ME(RiakClient, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(RiakClient, ping, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(RiakClient, fetch, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(RiakClient, store, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

void riak_client_init(TSRMLS_D) 
{
  zend_class_entry ce;
 
  INIT_CLASS_ENTRY(ce, "RiakClient", riak_client_methods);
  ce.create_object = create_client_data;
  riak_client_ce = zend_register_internal_class(&ce TSRMLS_CC);
  
  zend_declare_property_null(riak_client_ce, "host", sizeof("host")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_client_ce, "port", sizeof("port")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  
  zend_declare_property_null(riak_client_ce, "w", sizeof("w")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_client_ce, "dw", sizeof("dw")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_client_ce, "r", sizeof("r")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

zend_object_value create_client_data(zend_class_entry *class_type TSRMLS_DC) 
{
  zend_object_value retval;
  client_data *tobj;
 
  tobj = emalloc(sizeof(client_data));
  memset(tobj, 0, sizeof(client_data));

  zend_object_std_init((zend_object *) &tobj->std, class_type TSRMLS_CC);
  object_properties_init((zend_object*) &tobj->std, class_type);
 
  retval.handle = zend_objects_store_put(tobj, (zend_objects_store_dtor_t) zend_objects_destroy_object, 
    (zend_objects_free_object_storage_t) free_client_data, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();
  return retval;
}

void free_client_data(void *object TSRMLS_DC)
{
  client_data* data = (client_data*)object;
  zend_object_std_dtor(&data->std TSRMLS_CC);
  riack_free(data->client);
  efree(data);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakClient, __construct)
{
  int connResult;
  client_data *data;
  char *host, *szHost;
  int hostLen;
  long port = DEFAULT_PORT;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &hostLen, &port) == FAILURE) {
    return;
  }
  zend_update_property_stringl(riak_client_ce, getThis(), "host", sizeof("host")-1, host, hostLen TSRMLS_CC);
  zend_update_property_long(riak_client_ce, getThis(), "port", sizeof("port")-1, port TSRMLS_CC);
 
  data = (client_data*)zend_object_store_get_object(getThis() TSRMLS_CC);
  // TODO use allocater that uses PHP memory functions
  data->client = riack_new_client(&riack_default_allocator);

  szHost = estrndup(host, hostLen);
  connResult = riack_connect(data->client, szHost, port, NULL);
  efree(szHost);
  if (connResult != RIACK_SUCCESS) {
    zend_throw_exception(riak_connection_exception_ce, "Connection error", 1000 TSRMLS_CC);
  }
}

PHP_METHOD(RiakClient, ping)
{
  int pingStatus;
  GET_RIACK_CLIENT(client);
  
  pingStatus = riack_ping(client);
  CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, pingStatus);
}

PHP_METHOD(RiakClient, store)
{
  char *key, *contentType;
  int keyLen, contentTypeLen;
  zval *zBucket, *zObject, **zTmp;
  HashTable *zBucketProps, *zObjectProps;
  struct RIACK_OBJECT obj, returnedObj;
  struct RIACK_CONTENT riackContent;
  struct RIACK_PUT_PROPERTIES props;
  int riackResult;
  GET_RIACK_CLIENT(client);
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oos", &zBucket, &zObject, &key, &keyLen) == FAILURE) {
    return;
  }
  zBucketProps = zend_std_get_properties(zBucket TSRMLS_CC);
  zObjectProps = zend_std_get_properties(zObject TSRMLS_CC);
  
  memset(&obj, 0, sizeof(obj));
  memset(&returnedObj, 0, sizeof(returnedObj));
  memset(&riackContent, 0, sizeof(riackContent));
  memset(&props, 0, sizeof(props));

  //// fill content ////

  // Set content type
  HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "contentType", zTmp, riackContent.content_type) {
	  // Handle possible errors
  }
  // Set content encoding
  HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "contentEncoding", zTmp, riackContent.content_encoding) {
	  // Handle possible errors
  }
  
  // Set data
  if (zend_hash_find(zObjectProps, "data", sizeof("data"), (void**)&zTmp) == SUCCESS) {
    if (Z_TYPE_P(*zTmp) == IS_STRING) {
      riackContent.data_len = Z_STRLEN_P(*zTmp);
      riackContent.data = Z_STRVAL_P(*zTmp);
    }
  }
  
  //// fill obj ////
  
  // Set vclock
  if (zend_hash_find(zObjectProps, "vclock", sizeof("vclock"), (void**)&zTmp) == SUCCESS) {
    if (Z_TYPE_P(*zTmp) == IS_STRING) {
      obj.vclock.len = Z_STRLEN_P(*zTmp);
      obj.vclock.clock = Z_STRVAL_P(*zTmp);
    }
  }
  // Set bucket name
  HASH_GET_INTO_RIACK_STRING_OR_ELSE(zBucketProps, "name", zTmp, obj.bucket) {
    // Handle error
  }
  obj.content_count = 1;
  obj.content = &riackContent;
  obj.key.len = keyLen;
  obj.key.value = key;
  
  riackResult = riack_put(client, obj, &returnedObj, &props);
  CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, riackResult);

  // TODO Return updated object
  riack_free_object(client, &returnedObj);
}

PHP_METHOD(RiakClient, fetch)
{
  char *key;
  int keyLen, riackResult;
  size_t contentCount;
  zval *zBucket, *zObject, **zTmp;
  HashTable *zBucketProps;
  struct RIACK_GET_PROPERTIES props;
  struct RIACK_GET_OBJECT getResult;
  RIACK_STRING rsBucket, rsKey;
  
  GET_RIACK_CLIENT(client);

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oos", &zBucket, &zObject, &key, &keyLen) == FAILURE) {
    return;
  }
  zBucketProps = zend_std_get_properties(zBucket TSRMLS_CC);
  memset(&props, 0, sizeof(props));
  memset(&getResult, 0, sizeof(getResult));
  HASH_GET_INTO_RIACK_STRING_OR_ELSE(zBucketProps, "name", zTmp, rsBucket) {
    // Handle error
  }
  rsKey.len = keyLen;
  rsKey.value = key;
  riackResult = riack_get(client, rsBucket, rsKey, &props, &getResult);
  if (riackResult == RIACK_SUCCESS) {
    contentCount = getResult.object.content_count;
    if (contentCount > 1) {
      // TODO Handle siblings!
    } else if (contentCount == 0) {
      set_object_from_riak_content(zObject, &getResult.object.content[0] TSRMLS_CC);
      // TODO vclock
    }
    riack_free_get_object(client, &getResult);
  } else {
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, riackResult);
  }
}

void throwException(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC)
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


