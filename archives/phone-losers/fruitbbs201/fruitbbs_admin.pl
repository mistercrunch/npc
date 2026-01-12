#!/usr/bin/perl
##############################################################################

# Set the following variables to match your system.
# Don't forget to set the variables in the .htm files
# that come with this script.

$adminurl = "http://www.yourdomain.org/cgi-bin/fruitbbs_admin.pl";
$rootpath = "/html/virtual/dir/html/FruitWare/"; # include trailing slash
$rooturl = "http://www.yourdomain.org/FruitWare/"; # include trailing slash
$cgiurl = "http://www.yourdomain.org/cgi-bin/fruity_sub.pl"; # url to script
$sublog = "sublog.htm"; # name of bbs log file 
$password = "password"; #administrator password
$date_command = "/bin/date";

# Options:

$uselog = 1;            # 1 = Yes; 0 = No

# Finished. Nothing below this line should be modified
# unless you really think you know what the hell you're doing.
##############################################################

# Get the Date for Entry
$date = `$date_command +"%A, %B %d, %Y at %T (%Z)"`; chop($date);
$shortdate = `$date_command +"%D %T"`; chop($shortdate);

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

   if ($allow_html != 1) {
      $value =~ s/<([^>]|\n)*>//g;
   }

   $FORM{$name} = $value;
}

if ( $FORM{'johnnybravo'} eq "admin" ) {
	&admin;
	}
if ( $FORM{'johnnybravo'} eq "post" ) {
	&post_message;
	}
if ( $FORM{'johnnybravo'} eq "seelog" ) {
      &view_log;
	}


# Convert their IP address into a host name so posters can identify
# themselves in messages and also so you can ban certain hosts from
# posting messages.

 $ip = $ENV{'REMOTE_HOST'};
 $n = `nslookup $ip | grep Name`; chop($n);
 $host=substr($n, rindex($n, " ")+1);
 $period1=rindex($host,'.');
 $period2=rindex($host,'.',$period1-1);
 $host = substr($host,$period2+1);

print "Content-type: text/html\n\n";
print "<html><head><title>FruitWare Administration Menu</title></head>\n";
print "<body bgcolor=000000 link=ffff00 vlink=00ff00>";
print "<font face=arial color=ffffff>";
print "<center><h2>FruitBBS Administration Menu</h2></center><hr>\n";
print "<center><h3>Delete Individual Posts:</h3></center>\n";
print "<form method=POST  action=\"$adminurl\">\n";
print "<input type=hidden name=johnnybravo value=admin>\n";
print "<input type=text name=passcode>Admin Password<br>\n";
print "<input type=text name=messagedate>Date of Message (MM/DD/YY HH:MM:SS)<br>\n";
print "<input type=text name=messagetopic>Topic of Message<br>\n";
print "<input type=text name=subbase>Filename of Message Base<br>\n";
print "<input type=submit value=\"Delete Post\"> * <input type=reset value=\"Quit\"></form><br>\n";

print "<hr><center><h3>View Access Log:</h3></center>\n";
print "<form method=POST  action=\"$adminurl\">\n";
print "<input type=hidden name=johnnybravo value=seelog>\n";
print "<input type=text name=passcode>Admin Password \n";
print "<input type=submit value=\"View\">\n";
print "\n</form></body></html>\n";


if ($uselog eq '1') {
   &log('enter');
}

#######################
# Subroutines

sub no_comments {
   print "Content-type: text/html\n\n";
   print "<html><head><title>No Comment?</title></head>\n";
   print "<body><center><h1>You silly fruity moron</h1></center>\n";
   print "You didn't write anything in the comments field. What is the\n";
   print "point of a message if you don't write a message. Retard. Back\n";
   print "up and try again or just\n";
   print "return to the <a href=\"$fwwburl\">discussion board</a>.";
   print "\n</body></html>\n";

   exit;
}

sub no_soap {

   print "Content-type: text/html\n\n";
   print "<html><head><title>No Soap</title></head>\n";
   print "<body><center><h1>SORRY</h1></center>\n";
   print "<p><strong>$host has been identified as the source of a recent spam attack on\n";
   print "this web board so you will not be able to post here today. If\n";
   print "you think this bites big ones, contact RBCP. Otherwise, this ban\n";
   print "will be removed one of these days.</strong>\n";
   print "\n</body></html>\n";

   # Log The Error
   if ($uselog eq '1') {
      &log('no_soap');
   }

   exit;

}	
	
sub bad_word {

   print "Content-type: text/html\n\n";
   print "<html><head><title>No fucking foul language here.</title></head>\n";
   print "<body><center><h1>SORRY</h1></center>\n";
   print "<p><strong>We're sorry, but something you wrote offended us and\n";
   print "we just can't post your message unless you decide to make a few\n";
   print "changes in your wording. Click the back button and try it again,\n";
   print "buck-o.<br><hr></strong>\n";
   if ($showbadwords eq "1") {
   print "Here is our current list of banned words and/or phrases:<br>\n";
   print "$badwordlist";
   }
   print "\n</body></html>\n";

   # Log The Error
   if ($uselog eq '1') {
      &log('no_soap');
   }

   exit;

}	

sub no_name {
   print "Content-type: text/html\n\n";
   print "<html><head><title>What? You don't have a name?</title></head>\n";
   print "<body><h1>What's your name?</h1>\n";
   print "You didn't fill in the name field on your post. In order to post\n";
   print "on our web board, you have to have a name. If you don't have a\n";
   print "name, please go to your local Chamber of Commerce and get one.\n";
   print "Return to the <a href=\"$fwwburl\">discussion board</a>.";
   print "\n</body></html>\n";
   exit;
}

# Print confirmation and send them back to the web board
sub congrats {

   # Print Beginning of HTML
   print "Content-Type: text/html\n\n";
   print "<html><head><META HTTP-EQUIV=Refresh CONTENT=7;URL=$rooturl$FORM{'subnumber'}><title>CONGRATULATIONS!</title></head>\n";
   print "<body bgcolor=ffffff><font face=arial color=cc9900><center><h1>CONGRATULATIONS</h1><br>\n";
   print "<img src=http://www.phonelosers.org/FruitWare/smiley_face.gif></center>\n";
   print "<h3>You have posted a message on the discussion board! If you feel good about this then you need ";
   print "some serious counseling. Click <a href=$rooturl$FORM{'subnumber'}>here</a> to return to the discussion page ";
   print "or you will be automatically transferred there in 7 seconds.<br></h3>\n";
   print "</body></html>\n";

   exit;
}

#administration stuff
sub admin {
   if ( $password eq "$FORM{'passcode'}" ) {
   open (FILE,"$rootpath$FORM{'subbase'}") || die "Can't Open $rootpath$FORM{'subbase'}: $!\n";
   @LINES=<FILE>;
   close(FILE);
   $SIZE=@LINES;

   open (RUFF,">$rootpath$FORM{'subbase'}") || die "Can't Open $rootpath$FORM{'subbase'}: $!\n";

   for ($i=0;$i<=$SIZE;$i++) {
      $_=$LINES[$i];
      if (/<!--$FORM{'messagedate'}$FORM{'messagetopic'}-->/) { 
      print RUFF "";
   }
   else {
      print RUFF $_;
   }
 }

close (RUFF);
   print "Content-type: text/html\n\n";
   print "<html><head><title>Message Deleted</title></head>\n";
   print "<body><center><h1>MESSAGE DELETED</h1></center>\n";
   print "\n</body></html>\n";
   if ($uselog eq '1') {
   &log('msggone');
   }
   exit;
 }
else {
   print "Content-type: text/html\n\n";
   print "<html><head><title>Incorrect Password</title></head>\n";
   print "<body><center><h1>HACKER ALERT!</h1></center>\n";
   print "Okay, you evil hacker, you're in trouble now! ";
   print "You entered an incorrect password and this triggered our script\n";
   print "that automatically logged your name, address and phone number and\n";
   print "we've sent the police and FBI to your house. They will be there in\n";
   print "a matter of minutes, young man. Let this be a lesson to you, scum\n";
   print "of the earth!\n";
   print "\n</body></html>\n";
   if ($uselog eq '1') {
   &log('msgnotgone');
   }
   exit;
 }
}

# View access log
sub view_log {
if ( $password eq "$FORM{'passcode'}" ) {
   print "Location: $rooturl$sublog\n\n";
   if ($uselog eq '1') {
   &log('viewlog_good');
   }
}
   else {
   print "Content-type: text/html\n\n";
   print "<html><head><title>Incorrect Password</title></head>\n";
   print "<body><center><h1>HACKER ALERT!</h1></center>\n";
   print "Okay, you evil hacker, you're in trouble now! ";
   print "You entered an incorrect password and this triggered our script\n";
   print "that automatically logged your name, address and phone number and\n";
   print "we've sent the police and FBI to your house. They will be there in\n";
   print "a matter of minutes, young man. Let this be a lesson to you, scum\n";
   print "of the earth!\n";
   print "\n</body></html>\n";
   if ($uselog eq '1') {
   &log('viewlog_bad');
   }
 }
exit;
}

# Log the Entry or Error
sub log {
   $log_type = $_[0];
   open (LOG, ">>$rootpath$sublog");
   if ($log_type eq 'enter') {
      print LOG "$shortdate-ENTER ADMIN: $ENV{'REMOTE_HOST'}<br>\n";
   }
   elsif ($log_type eq 'no_soap') {
      print LOG "$shortdate-BANNED: $ENV{'REMOTE_HOST'} - $FORM{'subnumber'}<br>\n";
   }
   elsif ($log_type eq 'msggone') {
      print LOG "$shortdate-MESSAGE DELETED: $ENV{'REMOTE_HOST'} - $FORM{'messagedate'} - $FORM{'messagetopic'} - $FORM{'subbase'}<br>\n";
   }
   elsif ($log_type eq 'msgnotgone') {
      print LOG "$shortdate-INCORRECT PASSWORD: $ENV{'REMOTE_HOST'} - PASSWORD=$FORM{'passcode'} - $FORM{'messagedate'} - $FORM{'messagetopic'} - $FORM{'subbase'}<br>\n";
   }
   elsif ($log_type eq 'viewlog_good') {
      print LOG "$shortdate-VIEWED LOG: $ENV{'REMOTE_HOST'}<br>\n";
   }
   elsif ($log_type eq 'viewlog_bad') {
      print LOG "$shortdate-INCORRECT PASSWORD (view access log): $ENV{'REMOTE_HOST'} - PASSWORD=$FORM{'passcode'}<br>\n";
   }
}