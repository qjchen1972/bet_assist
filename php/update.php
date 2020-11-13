<?php

require "config.php";

function updateUser($name, $day){
    
    $connect = mysqli_connect(HOST,USER,PWD,DBNAME);
    if (mysqli_connect_errno($connect)) return 1;
    mysqli_set_charset($connect, 'utf8');
    
    $now = time();
    $end = $now + $day * 24 * 3600;
    //print($name);
    //print($day);
    $conStr = "UPDATE userinfo SET endTime=".$end." where name='". $name."'";
    //print($conStr);
    $result = mysqli_query($connect, $conStr);
    if(!$result) {
        mysqli_close($connect);
        return 1;
    }
    mysqli_close($connect);
    return 0;
}
    
updateUser($_POST['name'], $_POST['validTime']);
?>