--TEST--
Test mapreduce
--FILE--
<?php
include_once "connect.inc";
try {
    $client = new RiakClient($host, $port);
    $mr = new RiakMapreduce($client);
    echo "success!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
success!
