--TEST--
Simple ping test
--FILE--
<?php
include_once "connect.inc";
try {
	$riak = new RiakClient($host, $port);
	if (isset($riak)) {
		echo "connected!".PHP_EOL;
	}
	$riak->ping();
	echo "success!";
} catch (RiakConnectionException $e) {
	echo "Connection error!".PHP_EOL;
} catch (RiakCommunicationException $e1) {
	echo "Communication error!".PHP_EOL;
}
?>
--EXPECT--
connected!
success!