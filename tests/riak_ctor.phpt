--TEST--
Test Riak class constuctors
--FILE--
<?php
$riak = new Riak('1.2.3.4');
echo $riak->host.PHP_EOL;
echo $riak->port.PHP_EOL;
$riak = new Riak('4.3.2.1', 1234);
echo $riak->host.PHP_EOL;
echo $riak->port.PHP_EOL;
?>
--EXPECT--
1.2.3.4
8087
4.3.2.1
1234