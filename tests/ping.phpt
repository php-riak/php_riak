--TEST--
Simple ping test
--FILE--
<?php
include_once "connect.inc";
try {
	$riak = new \Riak\Connection($host, $port);
	if (isset($riak)) {
		echo "connected!".PHP_EOL;
	}
	$riak->ping();
	echo "success!";
} catch (\Riak\ConnectionException $e) {
	echo "Connection error!".PHP_EOL;
} catch (RiakCommunicationException $e1) {
	echo "Communication error!".PHP_EOL;
}
?>
--EXPECT--
connected!
success!