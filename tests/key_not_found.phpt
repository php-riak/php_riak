--TEST--
Test key not found exception getting thrown
--FILE--
<?php
use \Riak\Exception\NotFoundException;
include_once "connect.inc";
try {
	$client = new \Riak\Connection($host, $port);
	$bucket = new RiakBucket($client, "test_bucket");
        $readdenObj = $bucket->get("Long_key_that_should_never_exists_hopefully");
	var_dump($readdenObj);
} catch (NotFoundException $e) {
	echo "success!";
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!
