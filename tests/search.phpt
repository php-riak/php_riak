--TEST--
Simple ping test
--FILE--
<?php
use \Riak\Search\Search;
include_once "connect.inc";
$bucket = 'testsearch';
$riak = new RiakClient($host, $port);
$search = new Search($riak);
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
