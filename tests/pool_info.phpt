--TEST--
Test Riak\PoolInfo class
--INI--
riak.persistent.timeout=10000
riak.persistent.connections=20
--FILE--
<?php
include_once "connect.inc";

use Riak\PoolInfo;
use Riak\Connection;


echo 'active connection     : ' . PoolInfo::getNumActiveConnection().PHP_EOL;

$client = new Connection($host, $port);
$client->ping();

echo 'active connection     : ' . PoolInfo::getNumActiveConnection().PHP_EOL;
echo 'persistent connection : ' . PoolInfo::getNumActivePersistentConnection().PHP_EOL;

$client2 = new Connection($host, $port);
$client2->ping();

echo 'active connection     : ' . PoolInfo::getNumActiveConnection().PHP_EOL;
echo 'persistent connection : ' . PoolInfo::getNumActivePersistentConnection().PHP_EOL;

$client = NULL;
$client2 = NULL;

echo 'active connection     : ' . PoolInfo::getNumActiveConnection().PHP_EOL;
echo 'persistent connection : ' . PoolInfo::getNumActivePersistentConnection().PHP_EOL;
echo 'reconnect             : ' . PoolInfo::getNumReconnect().PHP_EOL;
?>
--EXPECT--
active connection     : 0
active connection     : 1
persistent connection : 1
active connection     : 2
persistent connection : 2
active connection     : 0
persistent connection : 0
reconnect             : 0
