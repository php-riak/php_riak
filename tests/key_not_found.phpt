--TEST--
Test key not found exception getting thrown
--FILE--
<?php
include_once "connect.inc";
try {
	$client = new RiakClient($host, $port);
	$bucket = new RiakBucket($client, "test_bucket");
	$readdenObj = $bucket->getObject("Long_key_that_should_never_exists_hopefully");
	var_dump($readdenObj);
} catch (RiakNotFoundException $e) {
	echo "success!";
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!