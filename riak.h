#ifndef RIAK__H__
#define RIAK__H__

/////////////////////////////////////////////////
// Constants
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8087

/////////////////////////////////////////////////
// Functions

void riak_init_riak(TSRMLS_D);
 
PHP_METHOD(Riak, __construct);
 
#endif