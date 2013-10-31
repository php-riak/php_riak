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
$resultList = $result->getResultList();
if ($resultList[0]->getKey() !== "obj1") {
    echo "exact failed".PHP_EOL;
    var_dump($result);
}

$q->setRangeValue(1, 8);
$input = new \Riak\Input\IndexInput();
$input->setMaxResults(4);
$result = $bucket->indexQuery($q, $input);
$resultList = $result->getResultList();
if ($resultList[0]->getKey() !== "obj1") {
    echo "ranged failed".PHP_EOL;
    var_dump($result);
}
$cont = $result->getContinuation();
$input->setContinuation($cont);
$input->setMaxResults(10);
$result = $bucket->indexQuery($q, $input);
$resultList = $result->getResultList();
if ($resultList[0]->getKey() !== "obj5") {
    echo "continuation failed ".$cont.PHP_EOL;
    var_dump($result);
}

echo "done!".PHP_EOL;
?>
--EXPECT--
done!
