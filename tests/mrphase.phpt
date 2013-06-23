--TEST--
Test map reduce phase object
--FILE--
<?php
use \Riak\MapReduce\Functions\JavascriptFunction;
use \Riak\MapReduce\Phase\MapPhase;
use \Riak\MapReduce\Phase\ReducePhase;

$jsfunction = new JavascriptFunction(true, "source_name");
$jsanon = JavascriptFunction::anon("anon_source");
$jsnamed = JavascriptFunction::named("named_source");
$phase1 = new MapPhase($jsfunction);
$arr1 = $phase1->toArray();
$subarr1 = $arr1['map'];
if ($subarr1["language"] !== "javascript" || $subarr1["name"] !== "source_name") {
    echo "map".PHP_EOL;
    var_dump($arr1);
}
$phase2 = new ReducePhase($jsnamed, true, array("plappe"=>"pluppe"));
$arr2 = $phase2->toArray();
$subarr2 = $arr2['reduce'];
$subarr2arg = $arr2['arg'];
if ($subarr2["keep"] !== true || $subarr2arg["plappe"] !== "pluppe" || $subarr1["language"] !== "javascript" || $subarr1["name"] !== "source_name") {
    echo "reduce".PHP_EOL;
    var_dump($arr2);
}
echo "done!";
?>
--EXPECT--
done!
