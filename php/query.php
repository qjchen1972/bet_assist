
<?php

require "config.php";

/*
  0 : ok
  1:  connect db error
  2:   user no exsit
  3:  out of date
*/

function queryUser($name, $id){
    
    $connect = mysqli_connect(HOST,USER,PWD,DBNAME);
    if (mysqli_connect_errno($connect)) return 1;
    mysqli_set_charset($connect, 'utf8');
    $conStr = "select * from userinfo  where name='". $name."' AND loginId=".$id;
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
    
    $now = time();
    
    if($now > $row[4]) {
        mysqli_free_result($result);
        mysqli_close($connect);
        return 4;
    }    
    
    mysqli_free_result($result);       
    mysqli_close($connect);
    return 0;
}

$ret = queryUser($_GET['name'], $_GET['id']);
printf("%d", $ret);

?>
