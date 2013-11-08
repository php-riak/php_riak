--TEST--
Test server info
--FILE--
<?php
include_once "connect.inc";

$conn = new \Riak\Connection($host, $port);
$serverInfo = $conn->getServerInfo();
$node = $serverInfo->getNode();
$serverVersion = $serverInfo->getServerVersion();
if (is_string($node) && is_string($serverVersion)) {
    echo "done!".PHP_EOL;
} else {
    var_dump($node);
    var_dump($serverVersion);
}
?>
--EXPECT--
done!
