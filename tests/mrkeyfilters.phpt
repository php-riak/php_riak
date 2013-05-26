--TEST--
Test mapreduce
--FILE--
<?php
include_once "connect.inc";

$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_keyfilters");
$newProps = new RiakBucketProperties(3, false);
$bucket->applyProperties($newProps);

// Make 20 keys for testing
for ($i=0; $i<20; $i++) {
    if ($i<10) {
        $obj = new RiakObject("key_0$i");
    } else {
        $obj = new RiakObject("key_$i");
    }
    $obj->data = "dummy";
    $bucket->putObject($obj);
}

$function1 = new RiakMrErlangFunction("riak_kv_mapreduce","map_object_value");
$input = new RiakMrInputBucket("test_keyfilters");
$input->keyFilters = array( array("tokenize", "_", 2), array("between", "05", "15") );

$mr = new RiakMapreduce($client);
$mr ->addPhase(new RiakMapreducePhase(RiakMapreducePhase::map, $function1))
    ->setInput($input);
$result = $mr->run();
// Should return 11 objects since we fetch 05-15 both inclusive
if (count($result) !== 11) {
    var_dump($result);
} else {
    echo "success!";
}
?>
--EXPECT--
success!
