--TEST--
Test Riak\PoolInfo class
--INI--
riak.persistent.timeout=10000
riak.persistent.connections=20
--FILE--
<?php
include_once "connect.inc";
echo \Riak\PoolInfo::getNumActiveConnection().PHP_EOL;
$client = new \Riak\Connection($host, $port);
echo \Riak\PoolInfo::getNumActiveConnection().PHP_EOL;
echo \Riak\PoolInfo::getNumActivePersistentConnection().PHP_EOL;
$client2 = new \Riak\Connection($host, $port);
echo \Riak\PoolInfo::getNumActiveConnection().PHP_EOL;
echo \Riak\PoolInfo::getNumActivePersistentConnection().PHP_EOL;
$client = NULL;
$client2 = NULL;
echo \Riak\PoolInfo::getNumActiveConnection().PHP_EOL;
echo \Riak\PoolInfo::getNumActivePersistentConnection().PHP_EOL;
echo \Riak\PoolInfo::getNumReconnect().PHP_EOL;
?>
--EXPECT--
0
1
1
2
2
0
0
0
