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
  $output = $bucket->put($obj);
  if (is_null($output->getKey()) || strlen($output->getKey()) == 0) {
    var_dump($output);
  }
  $bucket->delete($output->getKey());

  $obj = new Object("dummy");
  $obj->setContentType("text/plain");
  $obj->setContent("test value that should get written");
  $bucket->put($obj);
  $bucket->delete($obj);
  echo "done!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
done!
