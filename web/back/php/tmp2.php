<?php
require_once('./lib/jdatetime.class.php');


$hostname = 'localhost:3306';
$username = 'jjqioyps_watering_user';
$password = 'ibG_tvK1bznh';
$database = 'jjqioyps_smartgarden_db';

// Create connection
$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT * FROM watering_records  ORDER BY date_and_time DESC LIMIT 5";
$result = $conn->query($sql);

$data = array();

//put result in a array
if ($result->num_rows > 0) {


    while ($row = $result->fetch_assoc()) {
        $row["date"] = (new jDateTime(false, true))->convertFormatToFormat('d / F ', 'Y-m-d H:i:s', $row["date_and_time"]);
        $row["farsiDay"] = (new jDateTime(false, true))->convertFormatToFormat('l', 'Y-m-d H:i:s', $row["date_and_time"]);
        $row["time"] = (new jDateTime(false, true))->convertFormatToFormat('H:i', 'Y-m-d H:i:s', $row["date_and_time"]);

        $data[] = $row;
    }
}

$conn->close();
header('Content-Type: application/json');
echo json_encode($data);
?>