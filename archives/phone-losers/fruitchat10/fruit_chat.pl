#!/usr/bin/perl
################################################################
# 
# FruitWare Live Chat version 1.01
# created by RedBoxChiliPepper, January 1998
#
# http://www.phonelosers.org/fruitchat.htm
# Check out our website for updates of FruitWare
# products and lots of Phone Losers of America crap.
#
################################################################
# Set the following variables to match your system.

$chatpage = "/path/to/chatpage.htm";
$chatpageurl = "http://www.url_to.com/chatpage.htm";
$chatcgiurl = "http://www.cgi_path_to.com/cgi-bin/fruit_chat.pl";
$chatinfo = "http://www.phonelosers.org/chat_info.htm";
$chatinput = "http://www.phonelosers.org/chat_input.htm";
$chatlog = "/html/pla/chatlog.htm";
$date_command = "/bin/date";

# Finished. Nothing below this line should be modified
# unless you really think you know what the hell you're doing.
##############################################################

# Get the Date for Entry
$date = `$date_command +"%A, %B %d, %Y at %T (%Z)"`; chop($date);
$shortdate = `$date_command +"%T"`; chop($shortdate);
$postdate = `$date_command +"%A at %T"`; chop($postdate);

# Get the input
read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

# Split the name-value pairs
@pairs = split(/&/, $buffer);

foreach $pair (@pairs) {
   ($name, $value) = split(/=/, $pair);

   # Un-Webify plus signs and %-encoding
   $value =~ tr/+/ /;
   $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
   $value =~ s/<!--(.|\n)*-->//g;

   $FORM{$name} = $value;
}

#Turn the IP address into a readable domain name.
 $realip = $ENV{'REMOTE_ADDR'};
 $ip = $ENV{'REMOTE_HOST'};
 $n = `nslookup $ip | grep Name`; chop($n);
 $host=substr($n, rindex($n, " ")+1);    
 $period1=rindex($host,'.');
 $period2=rindex($host,'.',$period1-1);
 $host = substr($host,$period2+1);


if ($FORM{'yough'} eq "login") {
    &welcome;
}
elsif ($ENV{'HTTP_REFERER'} eq "$chatpageurl") {
    &show_users;
}
elsif ($FORM{'message'} eq "") {
	&reload_chat;
}
elsif ($FORM{'message'} eq " ") {
	&reload_chat;
}


# begin editing chat file

$newmessage = "<font color=ffffff>[$shortdate]</font><font color=$FORM{'colorname'}>[$FORM{'myname'}]\n";

open (HTMLOLD, "$chatpage");
@lines=<HTMLOLD>;
close(HTMLOLD);

open (FRUITC, ">>$chatlog");
print FRUITC "<a href=$ENV{'REMOTE_HOST'}>$FORM{'myname'}</a>$FORM{'message'}<br>\n";

open (NEW, ">$chatpage");
print NEW '<HTML><HEAD><META HTTP-EQUIV="Refresh" CONTENT="13; URL=';
print NEW "$chatpageurl#end\">";
print NEW '</HEAD><BODY BGCOLOR="#000000" link=ffff00 vlink=ffff00><font color=ff0000>';
print NEW "\n";
for ($i = 2; $i < 25; $i++)
	{
	print NEW "$lines[$i]";
	print "$lines[$i]";
	}
if ($FORM{'sizzle'} eq "action") {
print NEW "<br><a href=\"$FORM{'homeurl'}\" onMouseOver=\"window.status='Posted on $postdate by ";
if ($FORM{'getem'} eq "seeIP") {
	print NEW "$realip";
	}
	elsif ($FORM{'getem'} eq "seehost") {
	print NEW "$ip";
	}
	elsif ($FORM{'getem'} eq "seedomain") {
	print NEW "$host";
	}
print NEW "'; return true\">.</a></font>";
print NEW "<font color=$FORM{'colortext'}> $FORM{'myname'} $FORM{'message'}</font>\n";
}
else {
print NEW "<br><a href=\"$FORM{'homeurl'}\" onMouseOver=\"window.status='Posted on $postdate by ";
if ($FORM{'getem'} eq "seeIP") {
	print NEW "$realip";
	}
	elsif ($FORM{'getem'} eq "seehost") {
	print NEW "$ip";
	}
	elsif ($FORM{'getem'} eq "seedomain") {
	print NEW "$host";
	}
print NEW "'; return true\">\&lt;$FORM{'myname'}\&gt;</a></font>";
print NEW "<font color=$FORM{'colortext'}> $FORM{'message'}</font>\n";
}
print NEW "<a name=\"end\">\n</body></html>\n";
close(NEW);

&reload_chat;

#######################
# Subroutines

sub reload_chat {

   print "Content-Type: text/html\n\n";
   print "<html><head><META HTTP-EQUIV=Refresh CONTENT=0;URL=$chatpageurl#end><title>POSTED</title></head>\n";
   print "<body bgcolor=000000></body></html>\n";
   exit;
}

sub welcome {

   print "Content-Type: text/html\n\n";
   print "<html><head><title>LOGGED IN</title></head>\n";
   print "<body bgcolor=000000>\n<center>\n";
   print "<FORM ACTION=\"$chatcgiurl\" METHOD=\"POST\" target=\"chatbox\">\n";
   print "<input type=\"hidden\" name=\"myname\" value=\"$FORM{'username'}\">\n";
   print "<input type=\"hidden\" name=\"colortext\" value=\"$FORM{'textcolor'}\">\n";
   print "<input type=\"hidden\" name=\"homeurl\" value=\"$FORM{'homepage'}\">\n";
   print "<input type=\"hidden\" name=\"getem\" value=\"$FORM{'seeme'}\">\n";
   print "<SELECT name=sizzle>\n";
   print "<OPTION value=post selected>Post\n";
   print "<OPTION value=action>Action\n</select>\n";
   print "<input type=text name=message size=35>\n";
   print "<input type=submit value=\"Talk (Refresh)\">";
   print "<input type=reset value=\"Clear Talk\"></form>\n";
   print "<table><tr><td><form method=GET action=$chatinfo target=chatbox>\n";
   print "<input type=submit value=Help></form></td>\n";
   print "<td><form method=GET action=$chatinput>\n";
   print "<input type=submit value=Re-Login></form></td></tr></table>\n";
   print "</body></html>\n";
   exit;

}

sub log_chat {

   open (CHATLOG, ">>$chatlog");
   print CHATLOG "$ENV{'REMOTE_HOST'}<br>\n";
   exit;
}
