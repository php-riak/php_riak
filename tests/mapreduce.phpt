--TEST--
Test mapreduce
--FILE--
<?php
include_once "connect.inc";
try {
    $client = new RiakClient($host, $port);
    $jsanon = RiakJavascriptFunction::anon("anon_source");
    $mr = new RiakMapreduce($client);
    $mr->addPhase(new RiakMapreducePhase(RiakMapreducePhase::reduce, $jsanon));
    echo "success!";
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
success!
