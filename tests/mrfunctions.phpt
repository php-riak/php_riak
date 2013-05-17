--TEST--
Test map reduce phase object
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

$erl1 = new RiakMrErlangFunction(true, "source_name");
$erlanon = RiakMrErlangFunction::anon("anon_source");
$erlnamed = RiakMrErlangFunction::named("named_source");

$erlanonarr = $erlanon->toArray();
if ($erlanonarr["language"] !== "erlang" || $erlanonarr["source"] !== "anon_source") {
    var_dump($erlanonarr);
}
$erlnamedarr = $erlnamed->toArray();
if ($erlnamedarr["language"] !== "erlang" || $erlnamedarr["name"] !== "named_source") {
    var_dump($erlanonarr);
}
echo "done!";
?>
--EXPECT--
done!
