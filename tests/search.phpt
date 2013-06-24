--TEST--
Riak search test
--FILE--
<?php
use \Riak\Search\Search;
use \Riak\Search\Input\ParameterBag;
include_once "connect.inc";

$riak = new RiakClient($host, $port);
$bucket = new RiakBucket($riak, 'testsearch');

$json[] = '{"name": "apple","price": 2.50, "tags": ["fruit"]}';
$json[] = '{"name": "potato","price": 1.50, "tags": ["veg", "something"]}';
$json[] = '{"name": "pineapple","price": 15, "tags": ["fruit"]}';
$json[] = '{"name": "cheese", "price": 45, "tags": ["cow", "dairy"]}';

$i = 0;
foreach ($json as $j) {
    $i++;
    $obj = new RiakObject("id$i");
    $obj->contentType = "application/json";
    $obj->data = $j;
    $bucket->put($obj);
}

$search = new Search($riak);
$si = new ParameterBag();
$si->setDefaultField('name');
$res = $search->search("testsearch", "apple", $si);
foreach ($res->getDocuments() as $doc) {
    $fields = $doc->getFields();
    if ($fields["name"] != "apple") {
        var_dump($doc);
    }
}
for ($i=0; $i<count($json); $i++) {
    $bucket->delete("id$i");
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
