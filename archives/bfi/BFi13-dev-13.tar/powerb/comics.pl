#!/usr/bin/perl

# comics.pl
#
# A little, nice, almost working perl script to download and view on-the-fly
# comic strips from comics.com
# Still messy, unmaintainable, but well the idea is nice :)
#
# +mala, august 2003

use LWP::Simple;
use CGI;
use Time::Local;	# used to reverse given date into seconds since epoch

$MAIN_URL	=	"http://www.comics.com";
$URL_TEMPLATE	=	"http://www.comics.com/comics/#COMIC#/archive/#COMIC#-#DATE#.html";

@DEFAULT_COMICS =	qw(getfuzzy dilbert);
$DEFAULT_CGI_URL=	"/cgi-bin/comics.pl?";

# ---------------------------------------------------------------------------
sub makeurl{
	my ($url,$date) = @_;
   	$url =~ s/#DATE#/$date/g;
	return $url;
}

sub get_img_url{
	my ($url,$date) = @_;
	my $page = get (makeurl($url,$date));
	if ($page =~ /img src="([^"]+)"[^>]+?ALT="Today's/i){
		return $MAIN_URL.$1;
	}
}

sub to_date{
	my ($time) = @_;
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($time);
	$year+=1900; $mon=sprintf("%02d",$mon+1); $mday=sprintf("%02d",$mday); 
	return "$year$mon$mday";
}
# ---------------------------------------------------------------------------

print "Content-type: text/html\n\n";

# get CGI params
my $query = new CGI;
my $date 	= 	$query->param('date');
my $comics 	= 	$query->param('comics');
my $debug 	= 	$query->param('debug');

# now
my $time = time();
# today 
$TODAY = to_date ($time);

if ($date){
	$date =~ /(\d{4})(\d{2})(\d{2})/;
	my $year = $1; my $mon = $2; my $mday = $3;
	$time  = timelocal (0,0,0,$mday,$mon-1,$year);
}else{
	# if date's not specified use today's date
	$date = $TODAY;
}

# at this point:
# - $TODAY contains today's date - in YYYYMMDD format
# - $date  contains strip's date - in YYYYMMDD format
# - $time  contains strip's date - in epoch format

# yesterday
$YESTERDAY = to_date ($time - 86400);
# tomorrow
$TOMORROW  = to_date ($time + 86400);
$TOMORROW  = 0 if $TOMORROW > $TODAY;

# if comics are not specified use DEFAULT_COMICS
my $cgi_url;
if ($comics){
	@COMICS = split (",",$comics);
	$cgi_url = $DEFAULT_CGI_URL."comics=$comics&";
}else{
	@COMICS = @DEFAULT_COMICS;
	$cgi_url = $DEFAULT_CGI_URL;
}

# Write page header
print <<EOF;
<html>
<head><title>Malacomix</title></head>
<body>
<p align=center>
<font size="7">malacomix</font><font size="4">v1.0 (20040421)</font><br>
<font size="2">comix downloaded on the fly! --- 
<b>Usage:</b> <i>comics.pl?date=YYYYMMDD&comics=comic01,comic02,...</i><br>
see http://comics.com for strips names (ie. dilbert,getfuzzy,franknernest,...) --- 
default is <i>getfuzzy+dilbert</i></font>
<br><br>
EOF

# added for debugging purposes
# print ("$TODAY - $YESTERDAY - $date - $TOMORROW");

# Generate menu
print qq|[<a href="/index.html">back to index</a>] [|;
print qq|<a href="|.$cgi_url.qq|date=$YESTERDAY">prev</a>|;
print qq|-<a href="|.$cgi_url.qq|date=$TOMORROW">next</a>-<a href="/cgi-bin/comics.pl">Last</a>| if $TOMORROW;
print qq|]</p>\n|;

# now, for each comic, include strip image in a centered paragraph
for (@COMICS){
	# get page url for this comic
	my $page_url = $URL_TEMPLATE;
	   $page_url =~ s/#COMIC#/$_/g;

	my $full_url = get_img_url($page_url,$date);
	if ($full_url){
		print qq|<p align="center">|;
		print qq|<img src="$full_url"><br>|;
		print "</p>\n";
	}else{
		print "No images found for $_";
	}
}

