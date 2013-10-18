/*
   Copyright 2013 Kaspar Bach Pedersen

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
#include "document_output.h"

zend_class_entry *riak_search_doc_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_output_document_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_doc_methods[] = {
    PHP_ME(Riak_Search_Output_DocumentOutput, getFields, arginfo_search_output_document_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_search_output_document_output_init(TSRMLS_D) /* {{{  */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Output", "DocumentOutput", riak_search_doc_methods);
    riak_search_doc_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_doc_ce, "fields", sizeof("fields")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

zval *riak_search_document_from_riack_document(struct RIACK_SEARCH_DOCUMENT* document TSRMLS_DC) /* {{{  */
{
    int cnt, i;
    zval *zresult, *zarr;
    MAKE_STD_ZVAL(zarr)
    array_init(zarr);

    MAKE_STD_ZVAL(zresult)
    object_init_ex(zresult, riak_search_doc_ce);

    cnt = document->field_count;
    for (i=0; i<cnt; ++i) {
        char* szkey;
        size_t key_len;
        struct RIACK_PAIR *current_pair = &document->fields[i];
        key_len = current_pair->key.len;
        // Silly... we need to make a copy because add_assoc relies on zero terminated string
        // riack will not zero terminate since it includes a length.
        // However estrndup will be nice enough to add a zero termination for us...
        szkey = estrndup(current_pair->key.value, key_len);
        if (current_pair->value_present) {
            add_assoc_stringl_ex(zarr, szkey, key_len + 1,
                                 (char*)current_pair->value, current_pair->value_len, 1);
        } else {
            add_assoc_null_ex(zarr,  szkey, key_len+1);
        }
        efree(szkey);
    }
    zend_update_property(riak_search_doc_ce, zresult, "fields", sizeof("fields")-1, zarr TSRMLS_CC);
    zval_ptr_dtor(&zarr);
    return zresult;
}
/* }}} */


/*************************************************************
* Implementation: Riak\Search\Output\DocumentOutput
*************************************************************/

PHP_METHOD(Riak_Search_Output_DocumentOutput, getFields)
{
    RIAK_GETTER_ARRAY(riak_search_doc_ce, "fields");
}

