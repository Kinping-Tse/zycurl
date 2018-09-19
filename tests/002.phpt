--TEST--
zycurl_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('zycurl')) {
	echo 'skip';
}
?>
--FILE--
<?php 
$ret = zycurl_test1();

var_dump($ret);
?>
--EXPECT--
The extension zycurl is loaded and working!
NULL
