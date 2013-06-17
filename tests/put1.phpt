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
  $obj = $bucket->put($obj, "dummy");
  if ($obj->key !== "dummy" || strlen($obj->vclock) == 0) {
    var_dump($obj);
  }

  echo "done!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
done!
