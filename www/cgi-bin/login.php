<?php
// Manually output CGI headers
// Start the session (This resumes or creates a new session)
session_start();

echo "Content-Type: text/html\r\n";
echo "\r\n"; // Blank line to separate headers from body

?>

<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Login Page</title>
</head>
<body>

<?php
// If the user is already logged in, show a welcome message
if (isset($_SESSION['username'])) {
	echo "<h1>Welcome, " . htmlspecialchars($_SESSION['username']) . "!</h1>";
	echo "<p>You are already logged in.</p>";
	echo '<a href="logout.php">Logout</a>';
	exit;
}

// Check if the login form was submitted
if ($_SERVER["REQUEST_METHOD"] == "POST") {


    // Retrieve username and password from the form submission
    $username = $_POST['username'];
    $password = $_POST['password'];
    
    // Hardcoded credentials for testing
    if ($username == "admin" && $password == "secret") {
		// Store username in the session
        $_SESSION['username'] = $username;
		
        // Redirect using echo (CGI cannot use PHP's header() for Location)
        echo "Status: 302 Found\r\n";
        echo "Location: login.php\r\n";
        echo "\r\n"; // Blank line before ending headers
        exit;
    } else {
		// Show error if login fails
        echo "<p style='color: red;'>Invalid credentials. Please try again.</p>";
    }
}
?>

<h2>Login</h2>
<form method="post" action="login.php">
    <label for="username">Username:</label>
    <input type="text" name="username" id="username" required /><br /><br />
    
    <label for="password">Password:</label>
    <input type="password" name="password" id="password" required /><br /><br />
    
    <input type="submit" value="Login" />
</form>

</body>
</html>
