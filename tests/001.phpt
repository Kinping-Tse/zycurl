--TEST--
Check if zycurl is loaded
--SKIPIF--
<?php
if (!extension_loaded('zycurl')) {
	echo 'skip';
}
?>
--FILE--
<?php 
echo 'The extension "zycurl" is available';
?>
--EXPECT--
The extension "zycurl" is available
