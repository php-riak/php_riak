--TEST--
Test connection pool accepts limits
--INI--
riak.persistent.connections=20
riak.persistent.timeout=2000
--FILE--
<?php
include_once "connect.inc";

echo RiakStats::activeConnections().PHP_EOL;
$clients = array();
for ($i=0; $i<25; ++$i) {
	$clients[] = new RiakClient($host, $port);
}
echo RiakStats::activeConnections().PHP_EOL;
echo RiakStats::activePersistentConnections().PHP_EOL;

$clients = NULL;

echo RiakStats::activeConnections().PHP_EOL;
echo RiakStats::activePersistentConnections().PHP_EOL;
echo RiakStats::getNumReconnects().PHP_EOL;
?>
--EXPECT--
0
25
20
0
0
0
