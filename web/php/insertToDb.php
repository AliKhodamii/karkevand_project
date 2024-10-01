<?php
date_default_timezone_set('Asia/Tehran');

file_put_contents("debug.txt" , $_POST["insertIntoDB"]);
if($_POST["insertIntoDB"] != null){

    //get duration from ESP data
    $infoJson = $_POST["insertIntoDB"];
    //$infoJson = "{\"duration\" : 62}";
    $info = json_decode($infoJson);

    //create date of irr
    $dateTime = date('Y-m-d H:i:s');
    $duration = $info->duration;

    $hour = floor($duration/60);
    $min = $duration%60;

    $durTime = $hour . ":" . $min;

    //connect to db
    $hostname = 'localhost:3306';
    $username = 'jjqioyps_karSSG_db_user';
    $password = '123456';
    $database = 'jjqioyps_karSSG_db';

    // Create a database connection
    $conn = new mysqli($hostname, $username, $password, $database);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }



    // Prepare the SQL query
    $sql = "INSERT INTO IrrRec (dateTime, duration) VALUES ('$dateTime', '$durTime')";

    // Execute the SQL query
    if ($conn->query($sql) === TRUE) {
        echo "Data inserted successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }

    // Close the database connection
    $conn->close();
}