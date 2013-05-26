--TEST--
Test map reduce phase object
--FILE--
<?php
$jsfunction = new RiakMrJavascriptFunction(true, "source_name");
$jsanon = RiakMrJavascriptFunction::anon("anon_source");
$jsnamed = RiakMrJavascriptFunction::named("named_source");
$phase1 = new RiakMapreducePhase(RiakMapreducePhase::map, $jsfunction);
$arr1 = $phase1->toArray();
$subarr1 = $arr1['map'];
if ($subarr1["language"] !== "javascript" || $subarr1["name"] !== "source_name") {
    var_dump($arr1);
}
$phase2 = new RiakMapreducePhase(RiakMapreducePhase::reduce, $jsnamed, true, array("plappe"=>"pluppe"));
$arr2 = $phase2->toArray();
$subarr2 = $arr2['reduce'];
$subarr2arg = $arr2['arg'];
if ($subarr2["keep"] !== true || $subarr2arg["plappe"] !== "pluppe" || $subarr1["language"] !== "javascript" || $subarr1["name"] !== "source_name") {
    var_dump($arr2);
}
echo "done!";
?>
--EXPECT--
done!
