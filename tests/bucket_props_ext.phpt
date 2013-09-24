--TEST--
Test bucket properties
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket_props_ext");
$oldProps = $bucket->getPropertyList();

$newProps = new BucketPropertyList();
$newProps->setSearchEnabled(true)
         ->setR(1)
         ->setNValue(1)
         ->setW(1)
         ->setRW(1)
         ->setDW(1)
         ->setBigVClock(5000);
$bucket->setPropertyList($newProps);

$currentProps = $bucket->getPropertyList();
if ($currentProps->getNValue() === 1 &&
    $currentProps->getAllowMult() === false &&
    $currentProps->getSearchEnabled() === true &&
    $currentProps->getR() === 1 &&
    $currentProps->getW() === 1 &&
    $currentProps->getDW() === 1 &&
    $currentProps->getRW() === 1 &&
    $currentProps->getBigVClock() === 5000) {
        echo "success!";
} else {
        var_dump($currentProps);
}

$bucket->setPropertyList($oldProps);
?>
--EXPECT--
success!
