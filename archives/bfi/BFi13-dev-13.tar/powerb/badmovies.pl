#!/usr/bin/perl
#
use common;
use CGI;

$BADMOVIES_URL	=	'http://www.badmovies.org/movies/';
$LINK_FORMAT	=	'/movies/.*?/index.html';
$LEARNED_REGEXP =	'<img SRC=/common/learned.gif>(.*?)</font><br>';

my @quotes;

my $query = new CGI;
my $mode  = $query->param('mode');

print "Content-type: text/html\n\n";
$content = getpage ($BADMOVIES_URL);

@movies = exturl ($content,$LINK_FORMAT,'',$BADMOVIES_URL);

$movies_size = @movies;

$randurl = $movies[rand($movies_size-1)];

$content = getpage($randurl);

if ($content =~ /<title>\s*Review for (.*?)\s*\n/si){
	$title = $1;
	chomp $title;
}

if ($content =~ /learned.gif>(.*?)<\/font><br>/si){
	my $learned = $1;
	while ($learned =~ /10>\s*(.*?)\s*\n/gsi){
		push @quotes,$1;
	}
}

print qq|<html><head><title>Things I learned from B-Movies</title><head><body>\n|;

if ($mode eq 'fortune'){
	my $quote_size = @quotes;
	my $quote = $quotes[rand($quote_size-1)];
	print qq|<p align="center"><font size="5"><br><br>$quote<br><br>|;
	print qq|(<a href="$randurl">$title</a>)</font></p>|;
}else{
	print qq|<p align="center"><font size="5"><br><a href="$randurl">$title</a><br><br></font>|;
	print join ("<br>\n",@quotes);
}

print qq|</body></html>|;


