#!/usr/bin/perl

#use strict;
#use warnings;
use common;
#use Data::Dumper;
#use URI;

$CINEMA_LR  = qw|colspan="2">(.*?)</td>|;
$PHONE_LR   = qw|Telefono.*?height="1">(.*?)</td>|;
$MOVIE_HEAD = qw|<td width=1 height=15 rowspan=2 bgcolor="#009999"><img src="/monzacinema/immagini/spaziatore.gif" width=1 height=1></td>|;
$MOVIE_TAIL = qw|<td width=502 height=15 colspan=12><img src="/monzacinema/immagini/spaziatore.gif" width=502 height=1 border="0"></td>|;
$TITLE_LR   = qw|<td width=500 height=14 colspan=10.*?height="1">(<a.*?>)?(.*?)(</a>)?</td>|;
$ORARI_HEAD = qw|<td\sbgcolor="#D0D7D9"|;
$ORARI_TAIL = qw|<td colspan="13">|;
$GIORNO_LR  = qw|class="TestomenuNero"><img.*?>(.*?)</td>|;
$ORA_LR     = qw|align="center"><img.*?>(.*?)</td>|;

print "Content-type: text/html\n\n";
print "<html><head><title>Cinemaz</title></head>\n";
print "<body><pre>\n";

@a = 'http://www.monzacinema.it/monzacinema/index.jsp';
@d = ('scheda_cinema');
@array = walkpages (\@a,undef,undef,\@d,undef,1);

foreach $url (@array){
	print "================================================================\n";
	$content = getpage ($url);
	if ($content =~ /$CINEMA_LR/i){
		print "$1";
	}
	if ($content =~ /$PHONE_LR/si){
		print " ($1)\n";
	}
	while ($content =~ /$MOVIE_HEAD(.*?)$MOVIE_TAIL/gsi){
		$movie_body = $1;
		if ($movie_body =~ /$TITLE_LR/si){
			my $title = $2;
			$title =~ s/(\r\n)//s;
			$title =~ s/(\s\s+)/ /s;
			print "\n$title\n";
		}
		while ($movie_body =~ /$ORARI_HEAD(.*?)$ORARI_TAIL/gsi){
			$orari_body = $1;
			if ($orari_body =~ /$GIORNO_LR/i){
				print "$1:";
			}
			while ($orari_body =~ /$ORA_LR/gi){
				print " $1";
			}
			print "\n";
		}
	}
	print "\n";
}
print "================================================================\n";

print "</pre></body></html>\n";
exit;

