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
include_once "connect.inc";
ini_set('session.save_path',"proto://$host:$port/sessions?n=3");

session_start();
$_SESSION['favcolor'] = 'green';
session_commit();
session_start();
$readVal1 = $_SESSION['favcolor'];
session_destroy();
session_start();
if (!isset($_SESSION['favcolor']) && $readVal1 == 'green') {
	echo "success!";
}
session_destroy();
?>
--EXPECT--
success!
