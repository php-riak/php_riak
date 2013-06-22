[![Build Status](https://travis-ci.org/TriKaspar/php_riak.png)](https://travis-ci.org/TriKaspar/php_riak)

More documentation can be found at http://phpriak.bachpedersen.dk/  
Stubs for editor autocompletion at https://github.com/TriKaspar/php_riak_stub  

# Riak PHP extension
PHP extension for interfacing with Riak.  
## Features
- Protobuffer client
- Persistent connections
- Auto reconnects after errors or timeouts
- Riak PHP session store replacement, that can be enabled directly in php.ini no code needed.

## Building
First make sure you have the riack submodule checked out, if not do it:

	git submodule init
	git submodule update

In a terminal make sure you are in project root, then run the following commands

	phpize
	./configure
	sudo make install

Module is now installed but it needs to be added to the appropriate php.ini as well, this can be done by adding the following line to your php.ini  

	extension=riak.so
Migh vary depending on you php installation though.

## ini settings
You can override the following settings in your php.ini  

	; How many persistent connections pr. host:port so if you are connecting to 5 differenct servers there will be 20 persistent connections to each.  
	; If you do not want to reuse connections between request set this value to 0  
	riak.persistent.connections=20
	; If a connection has not been active within this time frame, automatically reconnect before using it again  
	riak.persistent.timeout=1000

## Usage

### Riak session
This extension includes a session module that can replace the default PHP session handler, no code changes needed just setup the following in your php.ini:

	session.save_handler=riak
	session.save_path=proto://HOST:PORT/SESSIONBUCKET

You can specify w, dw, pw, r, rw and pr values by adding them to the session.save_path.
For instance to use a local riak with w and r values of 2:

        proto://127.0.0.1:8087/session?w=2&r=2

Please note that the session handles does NOT set any bucket properties.
Bucket properties should either be set in app.config or applied manually on the bucket before using this session handler, conflicting writes are not handled so please disable siblings on the bucket.
Recommended bucket properties:

        n_val=3
        allow_mult=false
        last_write_wins=false

The session handler does not implement garbage collection, use riak´s key-expire setting instead
See http://docs.basho.com/riak/1.3.1/tutorials/choosing-a-backend/Bitcask/ for betails on how to enable automatic key expire.

### PHP Code
Examples of usage can be found in the .phpt files in the test folder...

## Test requirements
To make all tests succeed you need a running riak server with eleveldb backend and riak search enabled in app.config.
You also need to have seach enabled on the ´testsearch´ bucket, this can be done using the riak search-cmd like this:
```
bin/search-cmd install testsearch
```
