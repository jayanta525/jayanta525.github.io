<html>
<body>
<?php
    $client = $_GET["client"];
    $access = $_GET["access"];
    $sensor = $_GET["sensor"];
    $var = $client.'/index.html';
    $file = fopen($var, 'w+');
    ftruncate($file, 0);
    $content = $access.$sensor. PHP_EOL;
    fwrite($file , $content);
    fclose($file );
    die(header("Location: ".$_SERVER["HTTP_REFERER"]));
?>
</body>
</html>
