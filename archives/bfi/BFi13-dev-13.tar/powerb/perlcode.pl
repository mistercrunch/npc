#!/usr/bin/perl

use CGI;
use Syntax::Highlight::Perl;
use warnings;
use strict;

# put your sources directory here
my $SOURCES_DIR = '../src';

my $color_table = {
    'Variable_Scalar'   => 'color:#008800;',
    'Variable_Array'    => 'color:#ff7700;',
    'Variable_Hash'     => 'color:#8800ff;',
    'Variable_Typeglob' => 'color:#ff0033;',
    'Subroutine'        => 'color:#998800;',
    'Quote'             => 'color:#0000aa;',
    'String'            => 'color:#0000aa;',
    'Comment_Normal'    => 'color:#006699;font-style:italic;',
    'Comment_POD'       => 'color:#001144;font-family:garamond,serif;font-size:11pt;',
    'Bareword'          => 'color:#33AA33;',
    'Package'           => 'color:#990000;',
    'Number'            => 'color:#ff00ff;',
    'Operator'          => 'color:#000000;',
    'Symbol'            => 'color:#000000;',
    'Keyword'           => 'color:#000000;',
    'Builtin_Operator'  => 'color:#330000;',
    'Builtin_Function'  => 'color:#000011;',
    'Character'         => 'color:#880000;',
    'Directive'         => 'color:#339999;font-style:italic;',
    'Label'             => 'color:#993399;font-style:italic;',
    'Line'              => 'color:#000000;',
};

my $content = "";

sub pdie{
	my $msg = shift;
	print $msg;
	die;
}

print "Content-type: text/html\n\n";
print <<EOF;
<HTML><HEAD><TITLE>Perl sources viewer</TITLE></HEAD>
<BODY>
EOF

# load source file here 
my $query = new CGI;
my $file  = $query->param('file');

# if filename is not specified, then show file list

if (!$file){
	opendir(DIR, $SOURCES_DIR) || pdie "Can't open sources directory: $!";
	my @files = readdir(DIR);
	closedir DIR;
	print qq|<h2>Perl sources list</h2><br>|;
	for (@files){
		# show only .pl and .pm files
		print qq|<a href="/cgi-bin/perlcode.pl?file=$_">$_</a><br>| if /\.(pm|pl)$/;
	}
	exit;
}

# clean file name (am I too restrictive? ;))
$file =~ s/(\.\.|[^a-zA-z0-9.])//g;

my $f1 = open (IN,"<$SOURCES_DIR/$file") || pdie ("Could not open file\n\n");
while (<IN>){
	$content .= $_;
}
close IN;

# create a new formatter
my $formatter = Syntax::Highlight::Perl->new();

# define basic HTML escapes substitutions for the formatter
$formatter->define_substitution('<' => '&lt;', 
                                '>' => '&gt;', 
                                '&' => '&amp;');

# install the formats set up above
while ( my ( $type, $style ) = each %{$color_table}) {
    $formatter->set_format($type, [ qq|<span style=\"$style\">|, '</span>' ] );
}

print '<PRE style="font-size:10pt;color:#333366;">';
print $formatter->format_string($content);
print "</PRE>";

exit 0;

