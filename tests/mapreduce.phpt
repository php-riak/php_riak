--TEST--
Test mapreduce
--FILE--
<?php
include_once "connect.inc";
try {
    $client = new RiakClient($host, $port);
    $jsanon = RiakMrJavascriptFunction::anon("anon_source");
    $bucketinput = new RiakMrInputBucket("mapred_bucket");
    $mr = new RiakMapreduce($client);
    $mr ->addPhase(new RiakMapreducePhase(RiakMapreducePhase::reduce, $jsanon))
        ->setInput($bucketinput)
        ->run();
    echo "success!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
success!
