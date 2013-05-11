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
#include "session.h"

#ifdef PHP_SESSION

ps_module ps_mod_riak = {
	PS_MOD(riak)
};

static int ps_riak_initialize(ps_riak *data, const char *path)
{
}

static void ps_riak_destroy(ps_riak *data)
{
}

// void **mod_data, const char *save_path, const char *session_name TSRMLS_DC
PS_OPEN_FUNC(riak) 
{
	// TODO get riak_connection here.
	return SUCCESS;
}

PS_CLOSE_FUNC(riak) 
{
	return SUCCESS;
}

PS_READ_FUNC(riak) 
{
	return SUCCESS;
}

PS_WRITE_FUNC(riak)
{
	return SUCCESS;
}

PS_DESTROY_FUNC(riak)
{
	return SUCCESS;
}

PS_GC_FUNC(riak)
{
	return SUCCESS;
}

#endif

