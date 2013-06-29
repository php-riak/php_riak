--TEST--
Riak session handler test
--INI--
riak.persistent.timeout=10000
inisession.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=riak
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Exception\UnexpectedResponseException;

include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new RiakBucket($client, "sessions_violation");

// Set properties so we are sure writing a session later will fail because w > n
$props = new BucketPropertyList(2, false);
$bucket->applyProperties($props);

ini_set('session.save_path',"proto://$host:$port/sessions_violation?w=4&dw=4&pw=4&r=4&rw=4&pr=4");
try {
    session_start();
    $_SESSION['favcolor'] = 'green';
    session_commit();
} catch (UnexpectedResponseException $exc) {
    echo "done!";
}
?>
--EXPECTF--
Warning: session_commit(): Failed to write session data (riak). Please verify that the current setting of session.save_path %s
done!
