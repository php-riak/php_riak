--TEST--
Simple store test with no read back
--FILE--
<?php
use \Riak\Object;
include_once "connect.inc";
try {
  $client = new \Riak\Connection($host, $port);
  $bucket = new \Riak\Bucket($client, "test_bucket");

  // No key provieded
  $obj = new Object();
  $obj->setContent("dummy");
  $obj = $bucket->put($obj);
  if (is_null($obj->getKey()) || strlen($obj->getKey()) == 0) {
    var_dump($obj);
  }

  $obj = new Object("dummy");
  $obj->setContentType("text/plain");
  $obj->setContent("test value that should get written");
  $bucket->put($obj);
  echo "done!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
done!
