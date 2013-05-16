--TEST--
Test map reduce phase object
--FILE--
<?php
$jsfunction = new RiakJavascriptFunction(true, "source_name");
$jsanon = RiakJavascriptFunction::anon("anon_source");
$jsnamed = RiakJavascriptFunction::named("named_source");
$phase1 = new RiakMapreducePhase(RiakMapreducePhase::map, $jsfunction);
$phase2 = new RiakMapreducePhase(RiakMapreducePhase::reduce, $jsnamed, true, array("plappe"=>"pluppe"));
echo "success!";
?>
--EXPECT--
success!
