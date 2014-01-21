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
#include "link.h"
#include "ht_utils.h"
#include "exception/exception.h"

zend_class_entry *riak_object_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_ctor, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_key, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_value, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_add_index, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, index_name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_add_link, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_add_meta, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, metadata_name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_charset, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_content_type, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, content_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_content_encoding, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, content_encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_object_set_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vclock)
ZEND_END_ARG_INFO()

static zend_function_entry riak_object_methods[] = {
    PHP_ME(RiakObject, __construct, arginfo_riak_object_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakObject, getKey, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setKey, arginfo_riak_object_set_key, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getContent, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setContent, arginfo_riak_object_set_value, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, addIndex, arginfo_riak_object_add_index, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, addLink, arginfo_riak_object_add_link, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, addMetadata, arginfo_riak_object_add_meta, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setCharset, arginfo_riak_object_set_charset, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getCharset, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setContentType, arginfo_riak_object_set_content_type, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getContentType, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setContentEncoding, arginfo_riak_object_set_content_encoding, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getContentEncoding, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getVTag, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, isDeleted, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getIndexMap, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getMetadataMap, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getLinkList, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getVClock, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, setVClock, arginfo_riak_object_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getLastModifiedUSecs, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakObject, getLastModified, arginfo_riak_object_noargs, ZEND_ACC_PUBLIC)

    {NULL, NULL, NULL}
};

void riak_object_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak", "Object", riak_object_methods);
	riak_object_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "key", sizeof("key")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "vtag", sizeof("vtag")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "contentEncoding", sizeof("contentEncoding")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "charset", sizeof("charset")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_string(riak_object_ce, "contentType", sizeof("contentType")-1, "text/plain", ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "content", sizeof("content")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "isDeleted", sizeof("isDeleted")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "lastModified", sizeof("lastModified")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

    zend_declare_property_null(riak_object_ce, "metadata", sizeof("metadata")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "indexes", sizeof("indexes")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_object_ce, "links", sizeof("links")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

/*************************************************************
* Implementation: Riak\Object
*************************************************************/

/* {{{ proto void RiakObject->__construct([string $key])
Create a new RiakObject */
PHP_METHOD(RiakObject, __construct)
{
	char *key;
	int keyLen;
    zval *zarrmeta, *zindexes, *zarrlinks;
    keyLen = 0;
    key = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &keyLen) == FAILURE) {
		return;
	}
    if (keyLen > 0) {
        zend_update_property_stringl(riak_object_ce, getThis(), "key", sizeof("key")-1, key, keyLen TSRMLS_CC);
    }

	// Create empty array for metadata
    MAKE_STD_ZVAL(zarrmeta);
    array_init(zarrmeta);
    zend_update_property(riak_object_ce, getThis(), "metadata", sizeof("metadata")-1, zarrmeta TSRMLS_CC);
    zval_ptr_dtor(&zarrmeta);

    // Create empty array for indexes
    MAKE_STD_ZVAL(zindexes);
    array_init(zindexes);
    zend_update_property(riak_object_ce, getThis(), "indexes", sizeof("indexes")-1, zindexes TSRMLS_CC);
    zval_ptr_dtor(&zindexes);

    // Create empty array for links
    MAKE_STD_ZVAL(zarrlinks);
    array_init(zarrlinks);
    zend_update_property(riak_object_ce, getThis(), "links", sizeof("links")-1, zarrlinks TSRMLS_CC);
    zval_ptr_dtor(&zarrlinks);
}
/* }}} */

/* {{{ proto int Riak\Object->getLastModified()
 */
PHP_METHOD(RiakObject, getLastModified)
{
    RIAK_GETTER_LONG(riak_object_ce, "lastModified");
}
/* }}} */

/* {{{ proto int Riak\Object->getLastModified()
 */
PHP_METHOD(RiakObject, getLastModifiedUSecs)
{
    RIAK_GETTER_LONG(riak_object_ce, "lastModifiedUSecs");
}
/* }}} */

/* {{{ proto string Riak\Object->getKey()
 */
PHP_METHOD(RiakObject, getKey)
{
    RIAK_GETTER_STRING(riak_object_ce, "key")
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->setKey(string $key)
 */
PHP_METHOD(RiakObject, setKey)
{
    RIAK_SETTER_STRING(riak_object_ce, "key");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string Riak\Object->getContent()
 */
PHP_METHOD(RiakObject, getContent)
{
    RIAK_GETTER_STRING(riak_object_ce, "content")
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->setContent(string $value)
 */
PHP_METHOD(RiakObject, setContent)
{
    RIAK_SETTER_STRING(riak_object_ce, "content");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->addIndex(string $indexName [, int|string $value])
 */
PHP_METHOD(RiakObject, addIndex)
{
    zval *zindexarr, *zidxvalue;
    char* idxname;
    int idxname_len;
    zidxvalue = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &idxname, &idxname_len, &zidxvalue) == FAILURE) {
        return;
    }
    zindexarr = zend_read_property(riak_object_ce, getThis(), "indexes", sizeof("indexes")-1, 1 TSRMLS_CC);
    if (zidxvalue != NULL && Z_TYPE_P(zidxvalue) != IS_NULL) {
        zval_addref_p(zidxvalue);
        add_assoc_zval_ex(zindexarr, idxname, idxname_len+1, zidxvalue);
    } else {
        add_assoc_null_ex(zindexarr, idxname, idxname_len+1);
    }
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->addMetadata(string $name [, mixed $value]) */
PHP_METHOD(RiakObject, addMetadata)
{
    char* metaname;
    int metaname_len;
    zval* zmetaarr, *zmetaval;
    zmetaval = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &metaname, &metaname_len, &zmetaval) == FAILURE) {
        return;
    }
    zmetaarr = zend_read_property(riak_object_ce, getThis(), "metadata", sizeof("metadata")-1, 1 TSRMLS_CC);
    if (zmetaval != NULL && Z_TYPE_P(zmetaval) != IS_NULL) {
        zval_addref_p(zmetaval);
        add_assoc_zval_ex(zmetaarr, metaname, metaname_len+1, zmetaval);
    } else {
        add_assoc_null_ex(zmetaarr, metaname, metaname_len+1);
    }
    RIAK_RETURN_THIS
}
/* }}} */


/* {{{ proto Riak\Object Riak\Object->addLink(Riak\Link $link)
 */
PHP_METHOD(RiakObject, addLink)
{
    zval *zlink, *zlinksarr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zlink) == FAILURE) {
        return;
    }
    zlinksarr = zend_read_property(riak_object_ce, getThis(), "links", sizeof("links")-1, 1 TSRMLS_CC);
    zval_addref_p(zlink);
    add_next_index_zval(zlinksarr, zlink);
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->setCharset(string $charset) */
PHP_METHOD(RiakObject, setCharset)
{
    RIAK_SETTER_STRING(riak_object_ce, "charset");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string Riak\Object->getCharset() */
PHP_METHOD(RiakObject, getCharset)
{
    RIAK_GETTER_STRING(riak_object_ce, "charset");
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->setContentType(string $contentType) */
PHP_METHOD(RiakObject, setContentType)
{
    RIAK_SETTER_STRING(riak_object_ce, "contentType");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string Riak\Object->getContentType() */
PHP_METHOD(RiakObject, getContentType)
{
    RIAK_GETTER_STRING(riak_object_ce, "contentType");
}
/* }}} */

/* {{{ proto Riak\Object Riak\Object->setContentEncoding(string $contentEncoding) */
PHP_METHOD(RiakObject, setContentEncoding)
{
    RIAK_SETTER_STRING(riak_object_ce, "contentEncoding");
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto string Riak\Object->getContentEncoding() */
PHP_METHOD(RiakObject, getContentEncoding)
{
    RIAK_GETTER_STRING(riak_object_ce, "contentEncoding");
}
/* }}} */

/* {{{ proto string Riak\Object->getVTag() */
PHP_METHOD(RiakObject, getVTag)
{
    RIAK_GETTER_STRING(riak_object_ce, "vtag");
}
/* }}} */

/* {{{ proto bool Riak\Object->isDeleted() */
PHP_METHOD(RiakObject, isDeleted)
{
    zval* zdeleted = zend_read_property(riak_object_ce, getThis(), "isDeleted", sizeof("isDeleted")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zdeleted) == IS_BOOL && Z_BVAL_P(zdeleted)) {
        RETURN_BOOL(1);
    }
    RETURN_BOOL(0);
}
/* }}} */

/* {{{ proto array Riak\Object->getIndexMap() */
PHP_METHOD(RiakObject, getIndexMap)
{
    RIAK_GETTER_ARRAY(riak_object_ce, "indexes");
}
/* }}} */

/* {{{ proto array Riak\Object->getMetadataMap() */
PHP_METHOD(RiakObject, getMetadataMap)
{
    RIAK_GETTER_ARRAY(riak_object_ce, "metadata");
}
/* }}} */

/* {{{ proto Riak\Link[] Riak\Object->getLinkList() */
PHP_METHOD(RiakObject, getLinkList)
{
    RIAK_GETTER_ARRAY(riak_object_ce, "links");
}
/* }}} */

/* {{{ proto string|null Riak\Object->getVClock() */
PHP_METHOD(RiakObject, getVClock)
{
    RIAK_GETTER_STRING(riak_object_ce, "vClock");
}
/* }}} */

/* {{{ proto void Riak\Object->setVClock(string $vclock) */
PHP_METHOD(RiakObject, setVClock)
{
    RIAK_SETTER_STRING(riak_object_ce, "vClock");
    RIAK_RETURN_THIS
}
/* }}} */

/*************************************************************
* Implementation: Riak\Object Helpers
*************************************************************/

zval* create_object_object(const char* key TSRMLS_DC)/* {{{ */
{
    zval *zobject, *zkey;
    MAKE_STD_ZVAL(zobject);
    MAKE_STD_ZVAL(zkey);

    ZVAL_STRING(zkey, key, 1);

    object_init_ex(zobject, riak_object_ce);
    RIAK_CALL_METHOD1(RiakObject, __construct, zobject, zobject, zkey);

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

// zval *links_from_content(struct RIACK_CONTENT* content TSRMLS_DC)/* {{{ */
// {
   /* struct RIACK_LINK *curr_link;
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
    return zlinkarr;*/
// }
/* }}} */

/* Get metadata array from returned content */
// zval *assoc_array_from_riack_pairs(struct RIACK_PAIR* pairs, size_t pairscnt TSRMLS_DC) /* {{{ */
// {
//     zval *zresultarr;
// 	size_t i;
//     struct RIACK_PAIR* currentpair;
// 	char* currentKey;
//     MAKE_STD_ZVAL(zresultarr);
//     array_init(zresultarr);
//     for (i=0; i<pairscnt; ++i) {
//         currentpair = &(pairs[i]);
//         if (currentpair) {
//             currentKey = pestrndup(currentpair->key.value, currentpair->key.len, 0);
//             if (currentpair->value_present) {
//                 add_assoc_stringl(zresultarr, currentKey, (char*)currentpair->value, currentpair->value_len, 1);
// 			} else {
//                 add_assoc_null(zresultarr, currentKey);
// 			}
// 			pefree(currentKey, 0);
// 		}
// 	}
//     return zresultarr;
// }
/* }}} */


/* Set object properties from returned content */
// void set_object_from_riak_content(zval* object, struct RIACK_CONTENT* content TSRMLS_DC) /* {{{ */
// {
/*
    zval* zmetadata, *zlinks, *zindexes;

    zend_update_property_stringl(riak_object_ce, object, "content", sizeof("content")-1,
		(const char*)content->data, content->data_len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentEncoding", sizeof("contentEncoding")-1, 
		(const char*)content->content_encoding.value, content->content_encoding.len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentType", sizeof("contentType")-1, 
		(const char*)content->content_type.value, content->content_type.len TSRMLS_CC);
    if (content->vtag.len > 0 && content->vtag.value != NULL) {
        zend_update_property_stringl(riak_object_ce, object, "vtag", sizeof("vtag")-1,
            (const char*)content->vtag.value, content->vtag.len TSRMLS_CC);
    }
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

    zindexes = assoc_array_from_riack_pairs(content->indexes, content->index_count TSRMLS_CC);
    zend_update_property(riak_object_ce, object, "indexes", sizeof("indexes")-1, zindexes TSRMLS_CC);
    zval_ptr_dtor(&zindexes);

    zlinks = links_from_content(content TSRMLS_CC);
    zend_update_property(riak_object_ce, object, "links", sizeof("links")-1, zlinks TSRMLS_CC);
    zval_ptr_dtor(&zlinks);*/
// }
/* }}} */

/* Called once for each link in the links property of RiakObject */
// void set_links_from_object_cb(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{ */
// {
  /*  zval *zbucket, *ztag, *zkey;
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
    content->links[cnt].tag = riack_copy_string(client, rtag);*/
// }
/* }}} */

/* Copy all links from link array to a content structure */
// void set_links_from_object(struct RIACK_CONTENT* content, zval* zlinksarr, struct RIACK_CLIENT* client TSRMLS_DC)/* {{{ */
// {
/*    if (zlinksarr && Z_TYPE_P(zlinksarr) == IS_ARRAY) {
        content->link_count = zend_hash_num_elements(Z_ARRVAL_P(zlinksarr));
        if (content->link_count > 0) {
            content->links = RMALLOC(client, sizeof(struct RIACK_LINK) * content->link_count);
            memset(content->links, 0, sizeof(struct RIACK_LINK) * content->link_count);
            foreach_in_hashtable(client, content, Z_ARRVAL_P(zlinksarr), &set_links_from_object_cb TSRMLS_CC);
        }
    } */
// }
/* }}} */

/* Called once for each metadata or index entry in the RiakObject */
// void set_pairs_from_object_cb(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{ */
// {
/*    zval *tmp;
    RIACK_STRING rkey;
    struct RIACK_CLIENT* client = (struct RIACK_CLIENT*)callingObj;
    struct RIACK_PAIR* pairs = (struct RIACK_PAIR*)custom_ptr;
    if (key) {
        rkey.value = key;
        rkey.len = keylen-1;
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
    if (Z_TYPE_P(tmp) != IS_NULL) {
        INIT_PZVAL(tmp);
        zval_copy_ctor(tmp);
        if (Z_TYPE_P(tmp) != IS_STRING) {
            convert_to_string(tmp);
        }
        pairs[cnt].value_present = 1;
        RMALLOCCOPY(client, pairs[cnt].value, pairs[cnt].value_len, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
        zval_ptr_dtor(&tmp);
    }
*/
// }
/* }}} */

/* Copy all metadata from metadata array to a content structure */
// void set_metadata_from_object(struct RIACK_CONTENT* content, zval* zMetadata, struct RIACK_CLIENT* client TSRMLS_DC) /* {{{ */
// {
/*    if (zMetadata && Z_TYPE_P(zMetadata) == IS_ARRAY) {
        content->usermeta_count = zend_hash_num_elements(Z_ARRVAL_P(zMetadata));
        if (content->usermeta_count > 0) {
			content->usermetas = RMALLOC(client, sizeof(struct RIACK_PAIR) * content->usermeta_count);
			memset(content->usermetas, 0, sizeof(struct RIACK_PAIR) * content->usermeta_count);
            foreach_in_hashtable(client, content->usermetas, Z_ARRVAL_P(zMetadata), &set_pairs_from_object_cb TSRMLS_CC);
		}
    }*/
// }
/* }}} */

/* Copy all indexes from array to a content structure */
// void set_indexes_from_object(struct RIACK_CONTENT* content, zval* zindexsarray, struct RIACK_CLIENT* client TSRMLS_DC) /* {{{ */
// {
  /*  if (zindexsarray && Z_TYPE_P(zindexsarray)) {
        content->index_count = zend_hash_num_elements(Z_ARRVAL_P(zindexsarray));
        if (content->index_count > 0) {
            content->indexes = RMALLOC(client, sizeof(struct RIACK_PAIR) * content->index_count);
            memset(content->indexes, 0, sizeof(struct RIACK_PAIR) * content->index_count);
            foreach_in_hashtable(client, content->indexes, Z_ARRVAL_P(zindexsarray), &set_pairs_from_object_cb TSRMLS_CC);
        }
    }*/
// }
/* }}} */

/* Fill out members of a content struct with this objects values */
// void set_riak_content_from_object(struct RIACK_CONTENT* content, zval* object, struct RIACK_CLIENT* client TSRMLS_DC)/* {{{ */
// {
    /*zval* zTmp;
    zTmp = zend_read_property(riak_object_ce, object, "content", sizeof("content")-1, 1 TSRMLS_CC);
	if (Z_TYPE_P(zTmp) == IS_STRING) {
		content->data_len = Z_STRLEN_P(zTmp);
		content->data = (uint8_t*)Z_STRVAL_P(zTmp);
	}
	// Set content type
	GET_PROPERTY_INTO_RIACK_STR_OR_ELSE(riak_object_ce, object, "contentType", zTmp, content->content_type) ;
	// Set content encoding
	GET_PROPERTY_INTO_RIACK_STR_OR_ELSE(riak_object_ce, object, "contentEncoding", zTmp, content->content_encoding) ;

    zTmp = zend_read_property(riak_object_ce, object, "indexes", sizeof("indexes")-1, 1 TSRMLS_CC);
    set_indexes_from_object(content, zTmp, client TSRMLS_CC);

	zTmp = zend_read_property(riak_object_ce, object, "metadata", sizeof("metadata")-1, 1 TSRMLS_CC);
	set_metadata_from_object(content, zTmp, client TSRMLS_CC);

    zTmp = zend_read_property(riak_object_ce, object, "links", sizeof("links")-1, 1 TSRMLS_CC);
    set_links_from_object(content, zTmp, client TSRMLS_CC); */
// }
/* }}} */
