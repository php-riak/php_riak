--TEST--
Simple store test with no read back
--FILE--
<?php
include_once "connect.inc";
try {
  $client = new RiakClient($host, $port);
  $bucket = new RiakBucket($client, "test_bucket");

  // No key provieded
  $obj = new RiakObject();
  $obj->data = "dummy";
  $obj = $bucket->put($obj);
  if (is_null($obj->key) || strlen($obj->key) == 0) {
    var_dump($obj);
  }

  $obj = new RiakObject("dummy");
  $obj->contentType = "text/plain";
  $obj->data = "test value that should get written";
  $bucket->put($obj, "dummy");
  echo "done!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
done!
