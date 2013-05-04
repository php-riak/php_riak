--TEST--
Simple get test
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
try {
	$bucket->get("get_test");
} catch (RiakConflictedObjectException $ce) {
	// TODO Delete object
}
try {	
	$obj = new RiakObject("get_test");
	$obj->contentType = "text/plain";
	$obj->data = "test-get";
	$bucket->put($obj);
	sleep(2);
	$readdenObj = $bucket->get("get_test");
  echo "success!";
} catch (Exception $e) {
	var_dump($e);
	echo $e->getMessage();
}
?>
--EXPECT--
success!