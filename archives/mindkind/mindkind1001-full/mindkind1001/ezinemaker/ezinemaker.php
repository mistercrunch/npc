<?PHP
$zinefile = "template_mkd1000.txt";
$EOL = "\n";
$ezinefinal = "Mindkind1000.txt";

$zine = read_file($zinefile);


$line = explode($EOL,$zine);
$ezine = "";
for ($x=0;$x<count($line);$x++)
	{	
	$zarticle = read_file(str_replace("%","",$line[$x]));
echo $zarticle;
        $ezine .= $zarticle;
/*	for($x=0;$x<=4;$x++)
		{
		$ezine .= $EOL;
		}*/
	}

$fp = fopen ($ezinefinal, "w+");
fputs ($fp,$ezine);
fclose ($fp);

function read_file($file)
	{
	@$fid=fopen($file,"r");
	$page="";
	if ($fid)
		{
		while (!feof($fid))
			{
			$page=$page.fread($fid,1024);
                        }
		fclose($fid);
		}
                
	else
		{
		$page=0;
		}
	return($page);
}




?>
