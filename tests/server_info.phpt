--TEST--
Test server info
--FILE--
<?php
include_once "connect.inc";

$conn = new \Riak\Connection($host, $port);
$serverInfo = new \Riak\ServerInfo($conn);
$node = $serverInfo->getNode();
$serverVersion = $serverInfo->getServerVersion();
var_dump($node);
var_dump($serverVersion);
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
