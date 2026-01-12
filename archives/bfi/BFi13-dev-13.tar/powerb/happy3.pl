#!/usr/bin/perl
#
use LWP::Simple;

$URL =		'http://www.happytreefriends.com/watch_episodes';

# flash base url: who knows, it might change sooner or later and 
# become different from $URL
$BASE_URL =	'http://www.happytreefriends.com/watch_episodes'; 

print "Content-type: text/html\n\n";
print qq|<html><head><title>Happy3 FLASH URLS</title></head><body>|;
$content = get ($URL);

while ($content =~ /'flash\/play\.asp\?episode=(.*?)'/gsi){
	$FLASH_URL = "$BASE_URL/flash/htf_$1.swf";
		print qq|<a href="$FLASH_URL">$FLASH_URL</a><br>|;
} 

print qq|</html>|;

