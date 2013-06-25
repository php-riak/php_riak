--TEST--
Basic riak connection
--FILE--
<?php
use \Riak\Exception\ConnectionException;
include_once "connect.inc";
try {
	$riak = new RiakClient($host, $port);
	if (isset($riak)) {
		echo "done!".PHP_EOL;
	}
	$riak = new RiakClient("255.255.255.255");
} catch (ConnectionException $e) {
	echo "Connection error!".PHP_EOL;
}
?>
--EXPECT--
done!
Connection error!
