--TEST--
Test extended index query´ying
--FILE--
<?php
use \Riak\Query\IndexQuery;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "index_test_bucket");

for ($i=0; $i<10; $i++) {
    $obj = new \Riak\Object("obj$i");
    $obj->setContent("test-idx");
    $obj->addIndex("tal_int", $i)->addIndex("tekst_bin", "text$i");
    $bucket->put($obj);
}

$q = new IndexQuery("tal_int");
$q->setExactValue(1);
$result = $bucket->indexQuery($q);
if ($result[0]->getKey() !== "obj1") {
    var_dump($result[0]);
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
