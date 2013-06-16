<?php
define('BASEPATH', true);
$config = array();
include("geshi_languages.php");

foreach($config['geshi_languages'] as $key => $val) {
	echo "{\"".$key."\", \"".$val."\"},\n";
}
