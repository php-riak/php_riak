--TEST--
Simple store test with no read back
--FILE--
<?php
include_once "connect.inc";
try {
	$client = new RiakClient($host, $port);
	$bucket = new RiakBucket($client, "test_bucket");
  $obj = new RiakObject("dummy");
  $obj->contentType = "text/plain";
	$obj->data = "test value that should get written";
	$client->store($bucket, $obj, "dummy");
	echo "success!";
} catch (Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
success!