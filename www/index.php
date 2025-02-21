<?php
$servername = "db";
$username = "root";
$password = "rootpassword";
$dbname = "ctf_challenge";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$flag = "AKASEC{TESTFLAG}"; 
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $user = $_POST['username'];
    $pass = $_POST['password'];

    $query = "SELECT * FROM users WHERE username = '$user' AND password = '$pass'"; // anser login = [staff' OR 1=1 -- ]

    $result = $conn->query($query);

    if ($result === false) {
        echo 'EMMMMM';
    } else {
        if ($result->num_rows > 0) {
            echo "<b>$flag</b>";
        } else {
            echo "Invalid credentials.";
        }
    }
}


$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SQL Injection CTF</title>
</head>
<body>
    <h2>STAFF Login</h2>
    <form method="post">
        Username: <input type="text" name="username"><br>
        Password: <input type="password" name="password"><br>
        <input type="submit" value="Login">
    </form>
</body>
</html>
