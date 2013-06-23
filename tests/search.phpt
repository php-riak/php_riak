--TEST--
Riak search test
--FILE--
<?php
use \Riak\Search\Search;
use \Riak\Search\Input\Parameters;
include_once "connect.inc";

$riak = new RiakClient($host, $port);
$bucket = new RiakBucket($riak, 'testsearch');

$json[] = '{"name": "apple","price": 2.50, "tags": ["fruit"]}';
$json[] = '{"name": "potato","price": 1.50, "tags": ["veg", "something"]}';
$json[] = '{"name": "pineapple","price": 15, "tags": ["fruit"]}';
$json[] = '{"name": "cheese", "price": 45, "tags": ["cow", "dairy"]}';
foreach ($json as $j) {
    $obj = new RiakObject();
    $obj->contentType = "application/json";
    $obj->data = $j;
    $bucket->put($obj);
}
$search = new Search($riak);
$si = new Parameters();
$si->setDefaultField('name');
$search->search("testsearch", "apple");
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
