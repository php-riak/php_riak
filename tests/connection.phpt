--TEST--
Test Riak\Connection
--FILE--
<?php
include_once "connect.inc";
$connection = new \Riak\Connection($host, $port);
// Get host
if (strcmp($connection->getHost(), $host) !== 0) {
    echo "host differs".PHP_EOL;
    var_dump($connection);
}
// Get port
if ($connection->getPort() !== $port) {
    echo "port differs".PHP_EOL;
    var_dump($connection);
}
$b1_a = $connection->getBucket("bucket1");
$b2 = $connection->getBucket("bucket2");
$b1_b = $connection->getBucket("bucket1");
if ($b1_a != $b1_b) {
    echo "1a:".PHP_EOL;
    var_dump($b1_a);
    echo "1b:".PHP_EOL;
    var_dump($b1_b);
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
