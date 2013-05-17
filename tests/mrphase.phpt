--TEST--
Test map reduce phase object
--FILE--
<?php
$jsfunction = new RiakMrJavascriptFunction(true, "source_name");
$jsanon = RiakMrJavascriptFunction::anon("anon_source");
$jsnamed = RiakMrJavascriptFunction::named("named_source");
$phase1 = new RiakMapreducePhase(RiakMapreducePhase::map, $jsfunction);
$phase2 = new RiakMapreducePhase(RiakMapreducePhase::reduce, $jsnamed, true, array("plappe"=>"pluppe"));
echo "success!";
?>
--EXPECT--
success!
