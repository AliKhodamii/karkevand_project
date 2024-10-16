<?php
header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

$homepage = "";

if($_GET['file'] == 'sys'){
$homepage = file_get_contents('sysInfo.txt');
}
else if ($_GET['file'] == 'cmd'){
$homepage = file_get_contents('cmdInfo.txt');
}
echo $homepage;

// $homepage = "";

// header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
// header("Cache-Control: post-check=0, pre-check=0", false);
// header("Pragma: no-cache");

// if (isset($_GET['file'])) {
//     $file = $_GET['file'];
//     if ($file == 'sys') {
//         $homepage = file_get_contents('sysInfo.txt');
//     } else if($file == 'cmd') {
//         $homepage = file_get_contents('cmdInfo.txt');
//     }
// }

// echo $homepage;
?> 