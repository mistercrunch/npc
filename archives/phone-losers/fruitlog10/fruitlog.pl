#!/usr/bin/perl
##############################################################
# FruitWare FruitLog v1.01 written by RedBoxChiliPepper      #
# For more information and updates, visit                    #
# http://www.phonelosers.org/FruitWare/scripts.htm           #
#                                                            #
# To make this script work, all you have to do is enter the  #
# filepaths below, create those files (a blank text file will#
# do) and chmod this script and the blank files to 777. Then #
# on your web page(s), enter the following line somewhere:   #
#                                                            #
#    <!--#exec cgi="/cgi-bin/fruitlog.pl"-->                 #
#                                                            #
##############################################################

# User Defined Variables:

$logfile = "/html/path/to/access_log.htm";
$return = "/html/path/to/return_links.htm";

# Turn their IP into a domain name:
 
 $ip = $ENV{'REMOTE_HOST'};
 $n = `nslookup $ip | grep Name`; chop($n);
 $host=substr($n, rindex($n, " ")+1);    
 $period1=rindex($host,'.');
 $period2=rindex($host,'.',$period1-1);
 $host = substr($host,$period2+1);

# Log the time, host and page the person was last on:

if (-e $logfile) {
 open(foo,">>$logfile") || die "Can't open $logfile";
 print foo "<font color=ff0000>$time</font>\t$host\t$ENV{'HTTP_USER_AGENT'}\t<a href=$ENV{'HTTP_REFERER'}>$ENV{'HTTP_REFERER'}</a><br>\n";
 close(foo);
} #if

$cactus = $ENV{'HTTP_REFERER'};

# Check for repeat URLs so your page doesn't fill up so fast:

open(FILE,"$return");
@lines = <FILE>;
close(FILE);

$i=1;
foreach $line (@lines) {    
    if ($line =~ $ENV{'HTTP_REFERER'}) {
            &repeat_url;
        }
        $i++;
}

# Eliminate URL hits from most of the search engines and a few choice
# domains (such as your own) to reduce clutter. Add more if needed...

if ($cactus) {
if ($cactus =~ 'yahoo.com') {
&repeat_url;
}
elsif ($cactus =~ 'excite.com') {
&repeat_url;
}
elsif ($cactus =~ 'webcrawler.com') {
&repeat_url;
}
elsif ($cactus =~ 'infoseek.com') {
&repeat_url;
}
elsif ($cactus =~ 'file:') {
&repeat_url;
}
elsif ($cactus =~ 'search.com') {
&repeat_url;
}
elsif ($cactus =~ 'lycos.com') {
&repeat_url;
}
elsif ($cactus =~ 'www.webring.org') {
&repeat_url;
}
elsif ($cactus =~ 'metacrawler.com') {
&repeat_url;
}
elsif ($cactus =~ 'netfind.aol.com') {
&repeat_url;
}
elsif ($cactus =~ 'altavista.digital.com') {
&repeat_url;
}
elsif (-e $return) {
 open(foo,">>$return") || die "Can't open $return";
 print foo "<a href=$ENV{'HTTP_REFERER'}>$ENV{'HTTP_REFERER'}</a><br>\n";
 close(foo);
} #if
}

# If it's a repeat URL, quit the program
sub repeat_url {
   exit;
}