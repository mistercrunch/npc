#!/usr/bin/perl

package common;

use 5.005; # this is needed to use the map qr// trick 
use Exporter;
use LWP::UserAgent;
use Data::Dumper;
use HTTP::Cookies;
use URI;

@ISA 	= qw 	(Exporter);
@EXPORT = qw 	(
		exturl 
		getpage 
		walkpages
		walkpages_loop
		);

#----------------------------------------------------------------------------
# CONSTANTS

my $MAX_RETRIES = 5;

#----------------------------------------------------------------------------
# VARIABLES

my $ua;	# global user agent (for efficiency)
my $kj; # global cookie jar
my %h_vurl;

#----------------------------------------------------------------------------
#

BEGIN{

  @MyAgent::ISA = qw(LWP::UserAgent); # set inheritance

  $ua = MyAgent->new;
  $ua->agent("Two/0.01");
  $ua->env_proxy; # load proxy from environment variables *_proxy
  
  $kj = new HTTP::Cookies;
  $kj->load("cookies.txt");
  # activate cookie jar for the useragent - every page will support cookies now!
  $ua->cookie_jar($kj);
  push @{ $ua->requests_redirectable }, 'POST';
  $h_vurl{_}=0; 
}

#----------------------------------------------------------------------------
#

sub getpage{
	my $url = shift;
	
	my $retries = $MAX_RETRIES;
	# connect to the main url
	my $req = new HTTP::Request GET=> $url;
	my $res = $ua->request($req);

	while ((!$res->is_success)&&($retries)){
		my $retried = $MAX_RETRIES-$retries+1;
		warn "Error connecting to $url:\n";#.Dumper($res->headers);
		warn "Retrying ($retried)...\n";
		$res = $ua->request($req);
		$retries--;
	}

	unless ($res->is_success){
		warn "REALLY could not connect to $url (ignoring)\n";
		return;
	}

	# parse main page content for conferences
	$content = $res->content;
	return $content;
}

#----------------------------------------------------------------------------
#

sub exturl{
	my ($content,$regexp1,$regexp2,$pageurl) = @_;

	my (@links, @links2); 	# these are used to store links
	my %hash;		# this is used to store links occurrencies
	
	# if $regexp1 or $regexp2 are simple strings (scalars) and not array
	# of strings, convert them in an array of one element.
	my @a_regexp1 = (ref($regexp1) ? @$regexp1 : $regexp1);
	my @a_regexp2 = (ref($regexp2) ? @$regexp2 : $regexp2);

	# powerful regexp! :)
	while ($content =~ /<a.*?href\s*=[\s\"]*([^\s\">]+).*?>(.*?)(<\/a>|<\/td>)/gsi){
		my $url = $1;
		my $str = $2;
		
		# create an absolute version of the url, if it's relative
		my $uri1 = URI->new($pageurl);
		my $uri2 = URI->new($url);
		$url = $uri2->abs($uri1);
		
		my @comp_re1 = map qr/$_/i, @a_regexp1;
		my @comp_re2 = map qr/$_/i, @a_regexp2;
		for (@comp_re1) {
			push (@links,$url) if $url =~ /$_/;
		}
		for (@comp_re2) {
			push (@links,$url) if $str =~ /$_/;
		}
	}

	undef @ret_links;
	for (@links){
	      $hash{$_}++ || push @links2,$_;
        }

	return @links2;
}

#----------------------------------------------------------------------------
#

sub walkpages{
	my ($url,$regexp1,$regexp2,$regexp3,$regexp4,$eachpage) = @_;

	my %hash;
	my (@in_links, @out_links, @ret_links);
	
	my @a_url    = (ref($url)     ? @$url     : $url    );
	my @a_refol1 = (ref($regexp1) ? @$regexp1 : $regexp1);
	my @a_refol2 = (ref($regexp2) ? @$regexp2 : $regexp2);
	my @a_recol1 = (ref($regexp3) ? @$regexp3 : $regexp3);
	my @a_recol2 = (ref($regexp4) ? @$regexp4 : $regexp4);
	
	# extract links which match top level regexps
	# if LOOP then push on top the array I've just shifted
	my $a_re1 = shift (@a_refol1);
	my $a_re2 = shift (@a_refol2);
	my $empty = !($a_re1 || $a_re2); # are both the arrays empty?

	foreach $s_url (@a_url){
		$h_vurl{$s_url}++;
		print "Working on url $s_url:\n";
		# download url
		$content = getpage ($s_url);
		# NOTE: the values of @a_re1 and @a_re2 are array REFS!
		# first I check for the links to FOLLOW
		my @links = exturl ($content,$a_re1,$a_re2,$s_url);
		
		# then I should check for the links to COLLECT
		push @out_links, exturl ($content,\@a_recol1,\@a_recol2,$s_url) if ($eachpage||$empty);
		
		# if any link is contained in the parent array, delete it
		undef @in_links;
		for (@links){
		      $h_vurl{$_}++ || push @in_links,$_;
        	}

		# foreach link walkpage (link) with shifted regexp array
		if (!$empty){
			push @out_links, walkpages(\@in_links,\@a_refol1,\@a_refol2,\@a_recol1,\@a_recol2,$eachpage);
		}
	}
	for (@out_links){
		$hash{$_}++ || push @ret_links,$_;
	}
	return @ret_links;
}

#----------------------------------------------------------------------------
#

sub walkpages_loop{
	my ($url,$regexp1,$regexp2,$regexp3,$regexp4,$depth) = @_;
	
	my %hash;
	my (@in_links, @out_links, @ret_links);
	
	my @a_url    = (ref($url)     ? @$url     : $url    );
	my @a_refol1 = (ref($regexp1) ? @$regexp1 : $regexp1);
	my @a_refol2 = (ref($regexp2) ? @$regexp2 : $regexp2);
	my @a_recol1 = (ref($regexp3) ? @$regexp3 : $regexp3);
	my @a_recol2 = (ref($regexp4) ? @$regexp4 : $regexp4);
	
	# extract links which match top level regexps
	# if LOOP then push on top the array I've just shifted
	my $a_re1 = shift (@a_refol1);
	my $a_re2 = shift (@a_refol2);

	# push the values at the end of the arrays
	push @a_refol1 , $a_re1; 
	push @a_refol2 , $a_re2;
	
	foreach $s_url (@a_url){
		$h_vurl{$s_url}++;
		print "$depth: Working on url $s_url:\n";
		# download url
		$content = getpage ($s_url);
		# NOTE: the values of @a_re1 and @a_re2 are array REFS!
		# first I check for the links to FOLLOW
		my @links = exturl ($content,$a_re1,$a_re2,$s_url);
		
		# then I should check for the links to COLLECT
		push @out_links, exturl ($content,\@a_recol1,\@a_recol2,$s_url);
		
		# if any link is contained in the parent array, delete it
		undef @in_links;
		for (@links){
		      $h_vurl{$_}++ || push @in_links,$_;
        	}

		# foreach link walkpage (link) with shifted regexp array
		# if depth is !=1, of course
		# and passing (depth-1) or 0 if depth==0
		if ($depth!=1){
			$depth-- if $depth;
			push @out_links, walkpages_loop(\@in_links,\@a_refol1,\@a_refol2,\@a_recol1,\@a_recol2,$depth);
		}
	}
	undef @ret_links;
	for (@out_links){
		$hash{$_}++ || push @ret_links,$_;
	}
	return @ret_links;
}

1;

