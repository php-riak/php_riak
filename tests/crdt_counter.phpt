--TEST--
Test Riak\Crdt\Counter
--FILE--
<?php
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "cnt_test_bucket");
$newProps = new \Riak\BucketPropertyList(1, true);
$bucket->setPropertyList($newProps);

$counter1 = $bucket->counter("cnt1");
$counter2 = new \Riak\Crdt\Counter($bucket, "cnt2");


$getInput = new \Riak\Crdt\Input\GetInput();
$getInput->setR(1);
$getInput->setPR(1);
$getInput->setBasicQuorum(true);
$getInput->setNotFoundOk(false);

$updateInput = new \Riak\Crdt\Input\UpdateInput();
$updateInput->setW(1);
$updateInput->setDW(1);
$updateInput->setPW(1);

$start1 = $counter1->get();
$start2 = $counter2->get($getInput);

$read1 = $counter1->incrementAndGet(1);
if ($start1+1 != $read1) {
    echo "Inc1 a failed".PHP_EOL;
}
$read1 = $counter1->incrementAndGet(1, $updateInput);
if ($start1+2 != $read1) {
    echo "Inc1 b failed".PHP_EOL;
}

$counter2->increment(-1);
$read2 = $counter2->get($getInput);
if ($start1 - 1 != $read2) {
    echo "expected: ".($start1 - 1)." but got ".$read2.PHP_EOL;
    echo "Inc2 a failed".PHP_EOL;
}

$counter2->increment(-1, $updateInput);
$read2 = $counter2->get($getInput);
if ($start1 - 2 != $read2) {
    echo "expected: ".($start1 - 2)." but got ".$read2.PHP_EOL;
    echo "Inc2 b failed".PHP_EOL;
}
echo "done!".PHP_EOL;
--EXPECT--
done!
