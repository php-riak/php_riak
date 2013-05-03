[![Build Status](https://travis-ci.org/TriKaspar/php_riak.png)](https://travis-ci.org/TriKaspar/php_riak)

# Riak PHP extension
PHP extension for interfacing with Riak

## Dependencies
### Protobuf & protobuf-c
This extension uses protobuf-c libraries.

#### Linux
On ubuntu linux you can get them with apt-get like this:

	apt-get install libprotobuf-dev libprotobuf-c0-dev
Other linuxes probably have similar packages.

#### Mac OS
Homebrew have packages for protobuf-c and protobuf.
Install them like this:

	brew install protobuf
	

#### From source
If your system does not have access to a prebuild package you can always build from source.
Get more info and download here http://http://code.google.com/p/protobuf-c/

### Riack
Riack is bundled as git sub module, make sure it is checked out:

	git submodule init
	git submodule update

To build it go to a terminal and make sure you are in project root, then run the following

	./script/build_riack.sh
This will compile the needed library and php_riak will automatically find it, it is compiled as a static library so there is no runtime requirements besides protobuf-c.

## Building
First make sure the depencies above and PHP are installed properly.

In a terminal make sure you are in project root, then run the following scripts

	phpize
	./configure
	make install

To install the extension

	sudo make install

Module is now installed but it also needs to be added to the appropriate php.ini something like this(depending on install locations etc.):

	extension=riak.so
