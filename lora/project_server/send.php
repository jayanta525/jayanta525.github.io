<html>
<body>
<?php
    $client = $_GET["client"];
    $access = $_GET["access"];
    $file = fopen('new/index.html', 'w+');
    ftruncate($file, 0);
    $content = $client.$access. PHP_EOL;
    fwrite($file , $content);
    fclose($file );
    die(header("Location: ".$_SERVER["HTTP_REFERER"]));
?>
</body>
</html>
