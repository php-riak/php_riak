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

#include "ht_utils.h"

void foreach_in_hashtable(void* callingObj, void* custom_ptr, HashTable *ht, ht_entry_callback cb TSRMLS_DC)
{
    zval **data;
    HashPosition pointer;
    HashTable *hindex = ht;
    uint key_len, key_type;
    ulong index;
    int cnt = 0;
    char *key;
    for(zend_hash_internal_pointer_reset_ex(hindex, &pointer);
        zend_hash_get_current_data_ex(hindex, (void**)&data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(hindex, &pointer)) {
        key_type = zend_hash_get_current_key_ex(hindex, &key, &key_len, &index, 0, &pointer);
        if (key_type == HASH_KEY_IS_STRING) {
            (*cb)(callingObj, custom_ptr, key, key_len, 0, data, cnt++ TSRMLS_CC);
        } else if (key_type == HASH_KEY_IS_LONG) {
            (*cb)(callingObj, custom_ptr, NULL, 0, index, data, cnt++ TSRMLS_CC);
        }
    }
}
