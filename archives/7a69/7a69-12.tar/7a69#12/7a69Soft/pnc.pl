
#!/usr/bin/perl -w
#
# Anarion
# 02-09-2001
#
# FileTransfer encryptet with Blowfish :)
# You need the Blowfish module from www.cpan.org
# 
#
use IO::Socket;
use Crypt::Blowfish;
use Getopt::Std;
use strict;

my %opcions;
getopts('hd:lp:o:kK:',\%opcions) or ajuda();
ajuda() if $opcions{h} || (! $opcions{d} and ! $opcions{l}) || ! $opcions{p};

my $key = exists $opcions{k} && $opcions{k}==1 && llegeix_clau() || exists $opcions{K} && length($opcions{K}) == 8 && $opcions{K} || '';
print $key ?  "Using your key\n" : "No valid key introduced, skip encription\n";

server() if $opcions{l};
client() if $opcions{d};

######
#
sub ajuda
{
die <<AJUDA;
Version [1.1] by Anarion
Connect to somewhere:	  $0 -d host -p port [-options]
Inbound connects:	  $0 -l      -p port [-options]
Options:
		-o output	Redirect the output 
 		-k 		Read the key from STDIN 
		-K key 		use this key [8 chars]

AJUDA
}

######
#
sub llegeix_clau
{
	local $| = 1;
	local *TTY;
	my ($clau1,$clau2);
	open(TTY,"/dev/tty");
	system "stty -echo </dev/tty";
	print STDERR "the key must be 8 chars long\nType the key: ";
	chomp($clau1 = <TTY>);
	print STDERR "\nRe-type key: ";
	chomp($clau2 = <TTY>);
	print "\n\r";
	print "Keys dont match\n" and return unless $clau1 eq $clau2;
	print "The password must be 8 chars long\n" and return if length($clau1) != 8;
	return $clau1;
}

######
#
sub server
{
	my $socket = IO::Socket::INET->new(LocalPort => $opcions{p},
					   Listen    => 2,
					   Reuse     => 1,
					   Type	     => SOCK_STREAM,
					   Proto     => 'tcp',
					   ) or die "Cant bind socket $opcions{p}: $!";
	my ($client,$resposta);

	while ($client = $socket->accept())
	{
		print "Conection from: ",$client->peerhost(),"\nAccept it? [y/n]";
		chop($resposta = <STDIN>);
		last if $resposta eq 'y';
		close($client);
	}
	print "\n";
	open(FITXER,">$opcions{o}") || die "Sucks cant create $opcions{o}\n" if $opcions{o};
	my $sortida = *FITXER;
	$sortida = *STDOUT unless $opcions{o};
	my $blow = new Crypt::Blowfish $key if $key;
	while(<$client>)
	{
		chomp($_);
		$_ = pack("H*",$_);
		if ($key)
		{
			my @blocks = unpack("a8" x (int(length($_) / 8)+1),$_);
			my $text;
			foreach my $block (@blocks)
			{
				last unless $block;
				$text .= $blow->decrypt($block);
			}
			$text=~s/ *$//;
			print $sortida "$text\n";
			next;
		}
		print $sortida "$_\n";
	}
	exit;
}

##
#
sub client
{
	my $socket = IO::Socket::INET->new(PeerPort => $opcions{p},
					   PeerAddr => $opcions{d},
					   Type	     => SOCK_STREAM,
					   Proto     => 'tcp',
					   ) or die "Cant connect to $opcions{d} $opcions{p}: $!";


	my $blow = new Crypt::Blowfish $key if $key;

	while(1)
	{
		last unless $socket;
		chomp($_ = <STDIN>);
		last unless $_;
		if ($key)
		{
			my @blocks = unpack("a8" x (int(length($_) / 8)+1),$_);
			my $text;
			foreach my $block (@blocks)
			{
				$block .= " " x (8 - length($block)) if length($block) != 8;
				$text .= $blow->encrypt($block);
			}
			$text= unpack("H*",$text);
			print $socket "$text\n";
			next;
		}
		$_ = unpack("H*",$_);
		print $socket "$_\n";
	}
	close($socket);
	exit;							   
}



