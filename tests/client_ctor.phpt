--TEST--
Basic riak connection
--FILE--
<?php
use \Riak\Exception\ConnectionException;
include_once "connect.inc";
try {
    $riak1 = new \Riak\Connection($host, $port);
    $riak2 = new \Riak\Connection("255.255.255.255");

    $riak1->ping();

    echo "done!"  .PHP_EOL;

    $riak2->ping();

    echo "fail!"  .PHP_EOL;

} catch (ConnectionException $e) {
    echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
done!
Connection error
