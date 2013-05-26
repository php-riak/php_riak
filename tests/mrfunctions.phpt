--TEST--
Test map reduce function objects
--FILE--
<?php
$js1 = new RiakMrJavascriptFunction(true, "source_name");
$jsanon = RiakMrJavascriptFunction::anon("anon_source");
$jsnamed = RiakMrJavascriptFunction::named("named_source");

$jsanonarr = $jsanon->toArray();
if ($jsanonarr["language"] !== "javascript" || $jsanonarr["source"] !== "anon_source") {
    var_dump($jsanonarr);
}
$jsnamedarr = $jsnamed->toArray();
if ($jsnamedarr["language"] !== "javascript" || $jsnamedarr["name"] !== "named_source") {
    var_dump($jsanonarr);
}

$erl1 = new RiakMrErlangFunction("module", "function");

$erlnarr = $erl1->toArray();
if ($erlnarr["module"] !== "module" || $erlnarr["function"] !== "function") {
    var_dump($erlanonarr);
}
echo "done!";
?>
--EXPECT--
done!
