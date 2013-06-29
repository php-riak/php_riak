--TEST--
Test key filters
--FILE--
<?php
use \Riak\Object;
use \Riak\MapReduce\MapReduce;
use \Riak\MapReduce\Phase\MapPhase;
use \Riak\MapReduce\Functions\ErlangFunction;
use \Riak\MapReduce\Input\BucketInput;
use \Riak\BucketPropertyList;
include_once "connect.inc";

$client = new \Riak\Connection($host, $port);
$bucket = new RiakBucket($client, "test_keyfilters");
$newProps = new BucketPropertyList(3, false);
$bucket->applyProperties($newProps);

// Make 20 keys for testing
for ($i=0; $i<20; $i++) {
    if ($i<10) {
        $obj = new Object("key_0$i");
    } else {
        $obj = new Object("key_$i");
    }
    $obj->setContent("dummy");
    $bucket->put($obj);
}
$function1 = new ErlangFunction("riak_kv_mapreduce","map_object_value");
$input = new BucketInput("test_keyfilters");
$input->keyFilters = array( array("tokenize", "_", 2), array("between", "05", "15") );

$mr = new MapReduce($client);
$mr ->addPhase(new MapPhase($function1))
    ->setInput($input);
$mrres = $mr->run();

// Should return 11 objects since we fetch 05-15 both inclusive
$rescnt = 0;
foreach ($mrres as $resp) {
    $rescnt += count($resp->value);
}
if ($rescnt !== 11) {
    var_dump($result);
} else {
    echo "success!";
}
?>
--EXPECT--
success!
