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

#ifndef RIAK_HT__UTILS__H__
#define RIAK_HT__UTILS__H__

#include <php.h>

typedef void (*ht_entry_callback)(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC);

void foreach_in_hashtable(void* callingObj, void* custom_ptr, HashTable* ht, ht_entry_callback cb TSRMLS_DC);

#endif
