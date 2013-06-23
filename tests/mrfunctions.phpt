--TEST--
Test map reduce function objects
--FILE--
<?php
use \Riak\MapReduce\Functions\JavascriptFunction;
use \Riak\MapReduce\Functions\ErlangFunction;

$js1 = new JavascriptFunction(true, "source_name");
$jsanon = JavascriptFunction::anon("anon_source");
$jsnamed = JavascriptFunction::named("named_source");

$jsanonarr = $jsanon->toArray();
if ($jsanonarr["language"] !== "javascript" || $jsanonarr["source"] !== "anon_source") {
    var_dump($jsanonarr);
}
$jsnamedarr = $jsnamed->toArray();
if ($jsnamedarr["language"] !== "javascript" || $jsnamedarr["name"] !== "named_source") {
    var_dump($jsanonarr);
}

$erl1 = new ErlangFunction("module", "function");

$erlnarr = $erl1->toArray();
if ($erlnarr["module"] !== "module" || $erlnarr["function"] !== "function") {
    var_dump($erlanonarr);
}
echo "done!";
?>
--EXPECT--
done!
