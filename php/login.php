<?php

require "config.php";

/*
  0 : ok
  1:  connect db error
  2:   user no exsit
  3 : pwd error
  4:  out of date
*/

function checkUser($name, $pwd, &$id, &$validDate){
    
    $connect = mysqli_connect(HOST,USER,PWD,DBNAME);
    if (mysqli_connect_errno($connect)) return 1;
    mysqli_set_charset($connect, 'utf8');
    $conStr = "select * from userinfo  where name='". $name."'";
    $result = mysqli_query($connect, $conStr);
    if(!$result) {
        mysqli_close($connect);
        return 1;
    }
    
    $row = mysqli_fetch_row($result);
    if(!$row) {
        mysqli_free_result($result);
        mysqli_close($connect);
        return 2;   
    }
    
    if(strcmp($pwd, $row[2])) {
        mysqli_free_result($result);
        mysqli_close($connect);
        return 3;
    }
    
    $now = time();
    /*print($row[0]);
    print($row[1]);
    print($row[2]);
    print($row[3]);
    print($row[4]);
    print($row[5]);
    print($row[6]);
    print($row[7]);
    */
    
    $validDate = $row[4];
    if($now > $row[4]) {
        mysqli_free_result($result);
        mysqli_close($connect);
        return 4;
    }
    
    mysqli_free_result($result);       
    $conStr = "UPDATE userinfo SET loginTime=".$now.",loginId=".$now." where name='". $name."'";
    //print($conStr);
    $result = mysqli_query($connect, $conStr);
    if(!$result) {
        mysqli_close($connect);
        return 1;
    }
    $id = $now; 
    return 0;
}

$id = 0;
$validDate = 0;

$ret = checkUser($_GET['name'], $_GET['pwd'], $id, $validDate);
printf("%d,%d,%d", $ret, $id, $validDate);

/*$connect = mysqli_connect(HOST,USER,PWD,DBNAME);
$ret = 0;
if (mysqli_connect_errno($connect)) { 
    echo "连接 MySQL 失败: " . mysqli_connect_error(); 
}
//mysqli_set_charset('utf8',$connect);


$conStr = "select * from userinfo  where name='". $name."'";
//print($conStr."\n");
$result = mysqli_query($connect, $conStr);

if(!$result){
    print(" select error\n");
}
    
//printf("Select returned %d rows.\n", $result->num_rows);
$row = mysqli_fetch_row($result);
if(!$row){
        print(" no user\n");
}

$now = time() + 8 * 24 * 3600;
//print("now is ". $now."\n");
//print(date("Y-m-d h:i:s", $row[5]));
//print(date("Y-m-d h:i:s", $now));


if(!strcmp($_GET['name'], $row[1]) && !strcmp($_GET['pwd'], $row[2]) && $now < $row[5]) 
  print("ok\n");
else
	print("error\n");

print("test");
mysqli_free_result($result);
mysqli_close($connect)*/

?>
