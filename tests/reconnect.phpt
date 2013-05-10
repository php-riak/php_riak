--TEST--
Test that client auto reconnect if timeout passed
--FILE--
<?php
include_once "connect.inc";
$c = new RiakClient($host, $port);
$c->ping();
sleep(2);
$c->ping();
echo RiakStats::getNumReconnects().PHP_EOL;
?>
--EXPECT--
1