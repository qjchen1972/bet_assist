<?php 
$config = include(dirname(__FILE__)."/Cache/config.php");
header("Content-Type:text/html;charset=gb2312");
echo "访问情况：<br />"; 
$connect = mysqli_connect($config['DB_HOST'],$config['DB_USER'],$config['DB_PWD'],$config['DB_NAME']); 
if (mysqli_connect_errno($connect)) 
{ 
    echo "连接 MySQL 失败: " . mysqli_connect_error(); 
}
//mysqli_query("SET NAMES 'gb2312'");
//mysql_select_db($config['DB_NAME']);

//echo("type=".$_GET['type']);
//echo("page=".$_GET['page']);
if (!function_exists('mysql_result')) {
  function mysql_result($result, $number, $field=0) {
    mysqli_data_seek($result, $number);
    $row = mysqli_fetch_array($result);
    return $row[$field];
  }
}
include_once('qqwry.php');
$QQWry=new QQWry; 
if(!$QQWry) echo "不能查IP位置";
$pagesize=50;     //每页记录条数

$Start=empty ( $_GET['page'] ) ? 0 : (intval ( $_GET['page'] )-1)*$pagesize;


//echo($mpurl );

$currtype=empty ( $_GET['type'] ) ? "0" : $_GET['type'];
$typeurl="type=".$currtype;
$currtype=str_replace("","",$currtype);
//echo($currtype);
mysqli_query($connect,"SET NAMES gb2312");
$rss=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess"); 
$query = mysqli_query($connect,"select * from ".$config['DB_PREFIX']."homeaccess where 1 order by ipid desc LIMIT ".$Start.",".$pagesize);

$count=mysql_result($rss,0,0);
$rss_usercnt=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."user"); 
$count2=mysql_result($rss_usercnt,0,0);
$start = mktime(0,0,0,date('m'),date('d'),date('Y'));
$end = mktime(0,0,0,date('m'),date('d')+1,date('Y'))-1;
//var_dump($start.'+'.$end);
$day_count_all=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$start." AND iptime <=".$end); 
$count3=mysql_result($day_count_all,0,0);
$day_count=mysqli_query($connect,"select count(distinct(ipdata)) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$start." AND iptime <=".$end); 
$count4=mysql_result($day_count,0,0);
$beginYesterday=mktime(0,0,0,date('m'),date('d')-1,date('Y'));
$endYesterday=mktime(0,0,0,date('m'),date('d'),date('Y'))-1;

$beginLastMonth=mktime(0,0,0,date('m'),date('d')-30,date('Y'));
$endLastMonth=mktime(0,0,0,date('m'),date('d'),date('Y'));

$day_count_all=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$beginYesterday." AND iptime <=".$endYesterday); 
$count5=mysql_result($day_count_all,0,0);
$day_count=mysqli_query($connect,"select count(distinct(ipdata)) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$beginYesterday." AND iptime <=".$endYesterday); 
$count6=mysql_result($day_count,0,0);

$day_count_all=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count11=mysql_result($day_count_all,0,0);
$day_count=mysqli_query($connect,"select count(distinct(ipdata)) from ".$config['DB_PREFIX']."homeaccess where iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count12=mysql_result($day_count,0,0);

$day_count_all=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess where type=\"安卓\" AND iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count13=mysql_result($day_count_all,0,0);
$day_count=mysqli_query($connect,"select count(distinct(ipdata)) from ".$config['DB_PREFIX']."homeaccess where type=\"安卓\" AND iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count14=mysql_result($day_count,0,0);

$day_count_all=mysqli_query($connect,"select count(*) from ".$config['DB_PREFIX']."homeaccess where type=\"苹果IPhone\" AND iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count15=mysql_result($day_count_all,0,0);
$day_count=mysqli_query($connect,"select count(distinct(ipdata)) from ".$config['DB_PREFIX']."homeaccess where type=\"苹果IPhone\" AND iptime >=".$beginLastMonth." AND iptime <=".$endLastMonth); 
$count16=mysql_result($day_count,0,0);

echo "总访问数：".$count,"，";
echo "总注册数：".$count2,"；<br />";

echo "今天访问量：".$count3,"，";
echo "独立设备访问量：".$count4,"；<br />";

echo "昨天访问量：".$count5,"，";
echo "独立设备访问量：".$count6,"；<br />";

echo "<br />";

$last_30days_date = date('Y-m-d',strtotime('-30 day'));
$now_date = date('Y-m-d');
echo "过去30天（".$last_30days_date."至".$now_date."）<br />";

echo "访问总量：".$count11,"，";
echo "独立设备访问量：".$count12,"，";
echo "单个设备平均访问".round(($count11/$count12),2),"次；<br />";

echo "安卓访问总量：".$count13,"，";
echo "独立设备访问量：".$count14,"，";
echo "安卓（独立设备）访问量占比：".(round(($count14/$count12),4))*100,"%；<br />";

echo "苹果访问总量：".$count15,"，";
echo "独立设备访问量：".$count16,"，";
echo "苹果（独立设备）访问量占比：".(round(($count16/$count12),4))*100,"%。<br /><br />";

echo "注：以上数据均来自于海橋多元FM APP。";

mysqli_close($connect);

echo<<<EOT
<tr align="center" >
					<td align="center" colspan = 4> $pagecontrol </td>
</tr> 
			</table>

EOT

?> 