--TEST--
Basic riak connection
--FILE--
<?php
use \Riak\Exception\ConnectionException;
include_once "connect.inc";
try {
	$riak = new \Riak\Connection($host, $port);
	if (isset($riak)) {
		echo "done!".PHP_EOL;
	}
	$riak = new \Riak\Connection("255.255.255.255");
} catch (ConnectionException $e) {
	echo "Connection error!".PHP_EOL;
}
?>
--EXPECT--
done!
Connection error!
