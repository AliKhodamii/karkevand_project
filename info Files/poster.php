<?php
date_default_timezone_set('Asia/Tehran');

//data comes from esp8266
if ($_POST["info"] != null) {
    $data = json_decode($_POST["info"]);
    $text = json_encode($data, JSON_PRETTY_PRINT);
    file_put_contents("information.txt", $text);
}

// data comes from website
if ($_POST["info_from_web"] != null) {
    $data = json_decode($_POST["info_from_web"]);
    $text = json_encode($data, JSON_PRETTY_PRINT);
    file_put_contents("information.txt", $text);
}

// data comes from website to insert in db
if ($_POST["insert_new_record"] != null) {
    $data = json_decode($_POST["insert_new_record"]);
}

// insert data in db
if ($data->valveValue == 'on' && $_POST["insert_new_record"] != null) {
    // Database connection parameters
    $hostname = 'localhost:3306';
    $username = 'jjqioyps_watering_user';
    $password = 'ibG_tvK1bznh';
    $database = 'jjqioyps_smartgarden_db';

    // Create a database connection
    $conn = new mysqli($hostname, $username, $password, $database);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Data to be inserted
    $date_and_time = $currentDate = date('Y-m-d H:i:s');
    $sensor_value_at_start = $data->rotubat;
    $watering_time = $data->duration;

    // Prepare the SQL query
    $sql = "INSERT INTO watering_records (date_and_time, sensor_value_at_start , watering_time) VALUES ('$date_and_time', '$sensor_value_at_start' , '$watering_time')";

    // Execute the SQL query
    if ($conn->query($sql) === TRUE) {
        echo "Data inserted successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }

    // Close the database connection
    $conn->close();
}