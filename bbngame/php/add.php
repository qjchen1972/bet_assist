<?php

require "config.php";

function addUser($name, $pwd, $day){
    
    $connect = mysqli_connect(HOST,USER,PWD,DBNAME);
    if(mysqli_connect_errno($connect)) return 1;
    print("connec OK\n");
    mysqli_set_charset($connect, 'utf8');
    
    $now = time();
    $end = $now + $day * 24 * 3600;
    
    $conStr = "INSERT INTO userinfo(name,passwd, regTime, endTime) VALUES('".$name.
              "', '".$pwd."',". $now.",".$end.")";  
    print($conStr."\n");             
    $result = mysqli_query($connect, $conStr);
    print($result);
    if(!$result) {
        mysqli_close($connect);
        return 1;
    }
    mysqli_close($connect);
    return 0;
}


addUser($_POST['name'], $_POST['pwd'], $_POST['validTime']);

/*//print("start2\n");

//$link = mysql_connect(HOST,USER,PWD);
//or die("提示：数据库连接失败！");
$connect = mysqli_connect(HOST,USER,PWD,DBNAME);
//print("start connect\n");
if (mysqli_connect_errno($connect)) { 
    echo "连接 MySQL 失败: " . mysqli_connect_error(); 
}

print("connect OK\r\n");

//mysql_select_db(DBNAME,$link);
//print("connect1\r\n");
//编码设置
mysqli_set_charset('utf8',$connect);
print("connect2\n");


$name = $_POST['name'];
$pwd = $_POST['pwd'];
$validTime = $_POST['validTime'];
$validTime = 24 * 3600 * $validTime + time();

$conStr = "INSERT INTO userinfo(name,pwd, endTime) VALUES('".$name."', '".$pwd."',". $validTime.")";
print($conStr."\n");
mysqli_query($connect, $conStr);
mysqli_close($con);*/

?>
