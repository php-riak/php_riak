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

#include "object.h"
#include "php_riak.h"
#include "link.h"
#include "ht_utils.h"

zend_class_entry *riak_object_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_object_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry riak_object_methods[] = {
	PHP_ME(RiakObject, __construct, arginfo_object_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

void riak_object_init(TSRMLS_D)/* {{{ */
{
	zend_class_entry ce;
	

	INIT_CLASS_ENTRY(ce, "RiakObject", riak_object_methods);
	riak_object_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(riak_object_ce, "key", sizeof("key")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "vclock", sizeof("vclock")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "contentEncoding", sizeof("contentEncoding")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "charset", sizeof("charset")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(riak_object_ce, "contentType", sizeof("contentType")-1, "text/plain", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "data", sizeof("data")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "isDeleted", sizeof("isDeleted")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "lastModified", sizeof("lastModified")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	zend_declare_property_null(riak_object_ce, "metadata", sizeof("metadata")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "indexes", sizeof("indexes")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "links", sizeof("links")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}
/* }}} */

zval* create_object_object(const char* key TSRMLS_DC)/* {{{ */
{
    zval *zobject, *zkey;
    MAKE_STD_ZVAL(zobject);
    MAKE_STD_ZVAL(zkey);

    ZVAL_STRING(zkey, key, 1);

	object_init_ex(zobject, riak_object_ce);
    CALL_METHOD1(RiakObject, __construct, zobject, zobject, zkey);

    zval_ptr_dtor(&zkey);
    return zobject;
}
/* }}} */

void riak_key_from_object(zval *zobject, char** key, int* keylen TSRMLS_DC)/* {{{ */
{
    zval *zkey = zend_read_property(riak_object_ce, zobject, "key", sizeof("key")-1, 1 TSRMLS_CC);
    *key = Z_STRVAL_P(zkey);
    *keylen = Z_STRLEN_P(zkey);
}
/* }}} */

/* {{{ proto void RiakObject->__construct(string $key)
Create a new RiakObject */
PHP_METHOD(RiakObject, __construct)
{
	char *key;
	int keyLen;
    zval *zarrmeta, *zarrlinks;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLen) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_object_ce, getThis(), "key", sizeof("key")-1, key, keyLen TSRMLS_CC);

	// Create empty array for metadata
    MAKE_STD_ZVAL(zarrmeta);
    array_init(zarrmeta);
    add_property_zval_ex(getThis(), "metadata", sizeof("metadata")-1, zarrmeta TSRMLS_CC);
    zval_ptr_dtor(&zarrmeta);

    // Create empty array for indexes
    MAKE_STD_ZVAL(zarrmeta);
    array_init(zarrmeta);
    add_property_zval_ex(getThis(), "indexes", sizeof("indexes")-1, zarrmeta TSRMLS_CC);
    zval_ptr_dtor(&zarrmeta);

    // Create empty array for links
    MAKE_STD_ZVAL(zarrlinks);
    array_init(zarrlinks);
    add_property_zval_ex(getThis(), "links", sizeof("links")-1, zarrlinks TSRMLS_CC);
    zval_ptr_dtor(&zarrlinks);
}
/* }}} */

zval *links_from_content(struct RIACK_CONTENT* content TSRMLS_DC)/* {{{ */
{
    struct RIACK_LINK *curr_link;
    char *tag, *bucket, *key;
    zval *zlinkarr, *zlink;
    size_t i;

    MAKE_STD_ZVAL(zlinkarr);
    array_init(zlinkarr);
    for (i=0; i<content->link_count; ++i) {
        curr_link = &content->links[i];
        if (curr_link) {
            bucket = pestrndup(curr_link->bucket.value, curr_link->bucket.len, 0);
            key = pestrndup(curr_link->key.value, curr_link->key.len, 0);
            tag = pestrndup(curr_link->tag.value, curr_link->tag.len, 0);
            zlink = create_link_object(tag, bucket, key TSRMLS_CC);
            add_next_index_zval(zlinkarr, zlink);
            pefree(bucket, 0); pefree(key, 0); pefree(tag, 0);
        }
    }
    return zlinkarr;
}
/* }}} */

/* Get metadata array from returned content */
zval *assoc_array_from_riack_pairs(struct RIACK_PAIR* pairs, size_t pairscnt TSRMLS_CC) /* {{{ */
{
    zval *zresultarr;
	size_t i;
    struct RIACK_PAIR* currentpair;
	char* currentKey;
    MAKE_STD_ZVAL(zresultarr);
    array_init(zresultarr);
    for (i=0; i<pairscnt; ++i) {
        currentpair = &(pairs[i]);
        if (currentpair) {
            currentKey = pestrndup(currentpair->key.value, currentpair->key.len, 0);
            if (currentpair->value_present) {
                add_assoc_stringl(zresultarr, currentKey, (char*)currentpair->value, currentpair->value_len, 1);
			} else {
                add_assoc_null(zresultarr, currentKey);
			}
			pefree(currentKey, 0);
		}
	}
    return zresultarr;
}
/* }}} */


/* Set object properties from returned content */
void set_object_from_riak_content(zval* object, struct RIACK_CONTENT* content TSRMLS_DC) /* {{{ */
{
    zval* zmetadata, *zlinks, *zindexes;
	zend_update_property_stringl(riak_object_ce, object, "data", sizeof("data")-1, 
		(const char*)content->data, content->data_len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentEncoding", sizeof("contentEncoding")-1, 
		(const char*)content->content_encoding.value, content->content_encoding.len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentType", sizeof("contentType")-1, 
		(const char*)content->content_type.value, content->content_type.len TSRMLS_CC);
	
	zend_update_property_stringl(riak_object_ce, object, "charset", sizeof("charset")-1, content->charset.value, content->charset.len TSRMLS_CC);

	if (content->deleted_present) {
		zend_update_property_bool(riak_object_ce, object, "isDeleted", sizeof("isDeleted")-1, content->deleted TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "isDeleted", sizeof("isDeleted")-1 TSRMLS_CC);
	}
	if (content->last_modified_present) {
		zend_update_property_long(riak_object_ce, object, "lastModified", sizeof("lastModified")-1, content->last_modified TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "lastModified", sizeof("lastModified")-1 TSRMLS_CC);
	}
	if (content->last_modified_usecs_present) {
		zend_update_property_long(riak_object_ce, object, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1, content->last_modified_usecs TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1 TSRMLS_CC);
	}
    zmetadata = assoc_array_from_riack_pairs(content->usermetas, content->usermeta_count TSRMLS_CC);
    zend_update_property(riak_object_ce, object, "metadata", sizeof("metadata")-1, zmetadata TSRMLS_CC);
    zval_ptr_dtor(&zmetadata);

    zindexes = assoc_array_from_riack_pairs(content->usermetas, content->usermeta_count TSRMLS_CC);
    zend_update_property(riak_object_ce, object, "indexes", sizeof("indexes")-1, zindexes TSRMLS_CC);
    zval_ptr_dtor(&zindexes);

    zlinks = links_from_content(content TSRMLS_CC);
    zend_update_property(riak_object_ce, object, "links", sizeof("links")-1, zlinks TSRMLS_CC);
    zval_ptr_dtor(&zlinks);
}
/* }}} */

/* Called once for each link in the links property of RiakObject */
void set_links_from_object_cb(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{ */
{
    zval *zbucket, *ztag, *zkey;
    RIACK_STRING rbucket, rtag, rkey;
    struct RIACK_CLIENT* client = (struct RIACK_CLIENT*)callingObj;
    struct RIACK_CONTENT* content = (struct RIACK_CONTENT*)custom_ptr;

    zbucket = zend_read_property(riak_link_ce, *data, "bucket", sizeof("bucket")-1, 1 TSRMLS_CC);
    rbucket.len = Z_STRLEN_P(zbucket);
    rbucket.value = Z_STRVAL_P(zbucket);
    content->links[cnt].bucket = riack_copy_string(client, rbucket);

    zkey = zend_read_property(riak_link_ce, *data, "key", sizeof("key")-1, 1 TSRMLS_CC);
    rkey.len = Z_STRLEN_P(zkey);
    rkey.value = Z_STRVAL_P(zkey);
    content->links[cnt].key = riack_copy_string(client, rkey);

    ztag = zend_read_property(riak_link_ce, *data, "tag", sizeof("tag")-1, 1 TSRMLS_CC);
    rtag.len = Z_STRLEN_P(ztag);
    rtag.value = Z_STRVAL_P(ztag);
    content->links[cnt].tag = riack_copy_string(client, rtag);
}
/* }}} */

/* Copy all links from link array to a content structure */
void set_links_from_object(struct RIACK_CONTENT* content, zval* zlinksarr, struct RIACK_CLIENT* client TSRMLS_DC)/* {{{ */
{
    if (zlinksarr && Z_TYPE_P(zlinksarr) == IS_ARRAY) {
        content->link_count = zend_hash_num_elements(Z_ARRVAL_P(zlinksarr));
        if (content->link_count > 0) {
            content->links = RMALLOC(client, sizeof(struct RIACK_LINK) * content->link_count);
            memset(content->links, 0, sizeof(struct RIACK_LINK) * content->link_count);
            foreach_in_hashtable(client, content, Z_ARRVAL_P(zlinksarr), &set_links_from_object_cb TSRMLS_CC);
        }
    }
}
/* }}} */

/* Called once for each metadata or index entry in the RiakObject */
void set_pairs_from_object_cb(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{ */
{
    zval *tmp;
    RIACK_STRING rkey;
    struct RIACK_CLIENT* client = (struct RIACK_CLIENT*)callingObj;
    struct RIACK_PAIR* pairs = (struct RIACK_PAIR*)custom_ptr;
    if (key) {
        rkey.value = key;
        rkey.len = keylen;
        pairs[cnt].key = riack_copy_string(client, rkey);
    } else {
        MAKE_STD_ZVAL(tmp);
        ZVAL_LONG(tmp, index);
        convert_to_string(tmp);
        rkey.value = Z_STRVAL_P(tmp);
        rkey.len = Z_STRLEN_P(tmp);
        pairs[cnt].key = riack_copy_string(client, rkey);
        zval_ptr_dtor(&tmp);
    }
    ALLOC_ZVAL(tmp);
    *tmp = **data;
    INIT_PZVAL(tmp);
    zval_copy_ctor(tmp);
    if (Z_TYPE_P(tmp) != IS_NULL) {
        convert_to_string(tmp);
        pairs[cnt].value_present = 1;
        RMALLOCCOPY(client, pairs[cnt].value, pairs[cnt].value_len, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
    }
    zval_ptr_dtor(&tmp);
}
/* }}} */

/* Copy all metadata from metadata array to a content structure */
void set_metadata_from_object(struct RIACK_CONTENT* content, zval* zMetadata, struct RIACK_CLIENT* client TSRMLS_DC) /* {{{ */
{
    if (zMetadata && Z_TYPE_P(zMetadata) == IS_ARRAY) {
        content->usermeta_count = zend_hash_num_elements(Z_ARRVAL_P(zMetadata));
        if (content->usermeta_count > 0) {
			content->usermetas = RMALLOC(client, sizeof(struct RIACK_PAIR) * content->usermeta_count);
			memset(content->usermetas, 0, sizeof(struct RIACK_PAIR) * content->usermeta_count);
            foreach_in_hashtable(client, content->usermetas, Z_ARRVAL_P(zMetadata), &set_pairs_from_object_cb TSRMLS_CC);
		}
	}
}
/* }}} */

/* Copy all indexes from array to a content structure */
void set_indexes_from_object(struct RIACK_CONTENT* content, zval* zindexsarray, struct RIACK_CLIENT* client TSRMLS_DC) /* {{{ */
{
    if (zindexsarray && Z_TYPE_P(zindexsarray)) {
        content->index_count = zend_hash_num_elements(Z_ARRVAL_P(zindexsarray));
        if (content->index_count > 0) {
            content->indexes = RMALLOC(client, sizeof(struct RIACK_PAIR) * content->index_count);
            memset(content->indexes, 0, sizeof(struct RIACK_PAIR) * content->index_count);
            foreach_in_hashtable(client, content->indexes, Z_ARRVAL_P(zindexsarray), &set_pairs_from_object_cb TSRMLS_CC);
        }
    }
}
/* }}} */

/* Fill out members of a content struct with this objects values */
void set_riak_content_from_object(struct RIACK_CONTENT* content, zval* object, struct RIACK_CLIENT* client TSRMLS_DC)/* {{{ */
{
	zval* zTmp;
	zTmp = zend_read_property(riak_object_ce, object, "data", sizeof("data")-1, 1 TSRMLS_CC);
	if (Z_TYPE_P(zTmp) == IS_STRING) {
		content->data_len = Z_STRLEN_P(zTmp);
		content->data = (uint8_t*)Z_STRVAL_P(zTmp);
	}
	// Set content type
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(riak_object_ce, object, "contentType", zTmp, content->content_type) ;
	// Set content encoding
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(riak_object_ce, object, "contentEncoding", zTmp, content->content_encoding) ;

    zTmp = zend_read_property(riak_object_ce, object, "indexes", sizeof("indexes")-1, 1 TSRMLS_CC);
    set_indexes_from_object(content, zTmp, client TSRMLS_CC);

	zTmp = zend_read_property(riak_object_ce, object, "metadata", sizeof("metadata")-1, 1 TSRMLS_CC);
	set_metadata_from_object(content, zTmp, client TSRMLS_CC);

    zTmp = zend_read_property(riak_object_ce, object, "links", sizeof("links")-1, 1 TSRMLS_CC);
    set_links_from_object(content, zTmp, client TSRMLS_CC);
}
/* }}} */
