--TEST--
Test riak stats class
--INI--
riak.persistent.timeout=10000
riak.persistent.connections=20
--FILE--
<?php
include_once "connect.inc";
echo RiakStats::activeConnections().PHP_EOL;
$client = new \Riak\Connection($host, $port);
echo RiakStats::activeConnections().PHP_EOL;
echo RiakStats::activePersistentConnections().PHP_EOL;
$client2 = new \Riak\Connection($host, $port);
echo RiakStats::activeConnections().PHP_EOL;
echo RiakStats::activePersistentConnections().PHP_EOL;
$client = NULL;
$client2 = NULL;
echo RiakStats::activeConnections().PHP_EOL;
echo RiakStats::activePersistentConnections().PHP_EOL;
echo RiakStats::getNumReconnects().PHP_EOL;
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
