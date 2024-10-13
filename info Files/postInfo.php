<?php

if ($_POST["sysInfo"] != null){
    $sysInfo = json_decode($_POST["sysInfo"]);
    $sysInfoJson = json_encode($sysInfo,JSON_PRETTY_PRINT);
    file_put_contents("sysInfo.txt",$sysInfoJson);
}
if($_POST["cmdInfo"]){
    $cmdInfo = json_decode($_POST["cmdInfo"]);
    $cmdInfoJson = json_encode($cmdInfo,JSON_PRETTY_PRINT);
    file_put_contents("cmdInfo.txt",$cmdInfoJson);
}