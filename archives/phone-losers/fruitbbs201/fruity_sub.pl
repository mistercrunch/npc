#!/usr/bin/perl
##############################################################################

# Set the following variables to match your system.
# Don't forget to set the variables in the .htm files
# that come with this script.

$rootpath = "/html/virtual/FruitWare/"; # include trailing slash
$rooturl = "http://www.yourhost.org/FruitWare/"; # include trailing slash
$cgiurl = "http://www.yourhost.org/cgi-bin/fruity_sub.pl"; # url to script
$noaccess = "noaccess.txt"; # name of file with banned domains in it
$sublog = "fflog.htm"; # name of bbs log file 
$date_command = "/bin/date";

# Options:

$allowgifs = 1;         # 1 = Yes; 0 = No
$uselog = 1;            # 1 = Yes; 0 = No
$separator = 1;         # 1 = <hr>; 0 = <p>
$entry_order = 1;       # 1 = Newest entries added first;
                        # 0 = Newest Entries added last.
$allow_html = 0;        # 1 = Yes; 0 = No
$line_breaks = 1; 	# 1 = Yes; 0 = No

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


# If the user didn't fill out his name or the body of the message,
# send him a nasty message and tell him where to go...

&no_comments unless $FORM{'comments'};
&no_name unless $FORM{'realname'};

# Convert their IP address into a host name so posters can identify
# themselves in messages and also so you can ban certain hosts from
# posting messages.

 $ip = $ENV{'REMOTE_HOST'};
 $n = `nslookup $ip | grep Name`; chop($n);
 $host=substr($n, rindex($n, " ")+1);
 $period1=rindex($host,'.');
 $period2=rindex($host,'.',$period1-1);
 $host = substr($host,$period2+1);

# Open the banned file (specified in $noaccess) and don't let them post.
# You can add to this file by using a text editor and inserting hosts.

open(FILE,"$rootpath$noaccess") || &error('open->random_file',$noaccess);
@FILE = <FILE>;
close(FILE);
$hostbanfile = join('',@FILE);
if ( $host eq "") {
	$host = "yousuckbadly";
	}
if ( $hostbanfile =~ $host ) {
	&no_soap;
}

$badbad = $FORM{'comments'};
open(FILE,"$rootpath$badwords") || &error('open->random_file',$badwords);
@FILE = <FILE>;
close(FILE);
$badwordlist = join('',@FILE);
if ( $badbad =~ $badwordlist ) {
	&bad_word;
}

# Begin the Editing of the web board file
open (FILE,"$rootpath$FORM{'subnumber'}") || die "Can't Open $rootpath$FORM{'subreal'}: $!\n";
@LINES=<FILE>;
close(FILE);
$SIZE=@LINES;

# Open Link File to Output
open (FWWB,">$rootpath$FORM{'subnumber'}") || die "Can't Open $rootpath$FORM{'subnumber'}: $!\n";

for ($i=0;$i<=$SIZE;$i++) {
   $_=$LINES[$i];
   if (/<!--begin-->/) { 

      if ($entry_order eq '1') {
         print FWWB "<!--begin-->\n";
      }
   
      if ($line_breaks == 1) {
         $FORM{'comments'} =~ s/\cM\n/<br>/g;
      }

      if ( $FORM{'headcolor'} ) {
      	print FWWB " <font color=$FORM{'headcolor'}> ";
    	      }

      print FWWB "<!--$shortdate$FORM{'topic'}-->";
	print FWWB "TOPIC - $FORM{'topic'} <br> DATE - $shortdate<br>";
	print FWWB "FROM - <a href=\"mailto:$FORM{'username'}\">$FORM{'realname'}</a>";
      print FWWB " $FORM{'imfrom'}";

      if ( $FORM{'headcolor'} ) {
      	print FWWB " </font> ";
    	      }

	if ( $FORM{'showme'} ) {
         print FWWB " - <font color=ff33ff size=-7>$host</font>";
	}

      if ( $FORM{'bodycolor'} ) {
      	print FWWB " <font color=$FORM{'bodycolor'}> ";
    	      }

	print FWWB "<p>$FORM{'comments'}<br>";

      if ( $FORM{'bodycolor'} ) {
      	print FWWB " </font> ";
    	      }

      if ( $FORM{'limage'} ){
         print FWWB "<center><img src=$FORM{'limage'}></center>";
      }

      if ( $FORM{'pageurl'} ){
         print FWWB "<center><a href=$FORM{'pageurl'}>$FORM{'ptitle'}</a></center>";
      }

      if ($separator eq '1') {
         print FWWB "<hr>\n";
      }
      else {
         print FWWB "<p>\n";
      }

      if ($entry_order eq '0') {
         print FWWB "<!--begin-->\n";
      }

   }
   else {
      print FWWB $_;
   }
}

close (FWWB);

# Log the post (if specified) then tell them their message posted okay.

if ($uselog eq '1') {
   &log('entry');
}
   &congrats;

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

#print out the form to post a message
sub post_message {
   print "Content-type: text/html\n\n";
   print "<html><head><title>Post Your Message Now</title></head>\n";
   print "<body bgcolor=000000><font face=arial color=ffff00><center><h3>Post A Message</h3><br>\n";
   print "</center><form method=POST action=\"$cgiurl\">\n";
   print "<input type=hidden name=subnumber value=\"$FORM{'subnumber'}\">\n";
   print "<h5><font face=arial color=ffffff size=-7>\n";
   print "<input type=text name=realname size=15 maxlength=90> - Name/Alias<br>\n";
   print "<input type=text name=username size=13 maxlength=90>\n";
   print "<input type=checkbox name=showme value=checked>\n";
   print " - E-Mail (check the box to prove it)<br>";
   print "<input type=text name=topic size=15 maxlength=90> -  Topic<br>\n";
   print "<input type=text name=imfrom size=15 maxlength=25> - Where are you from?<br>\n";
   print "<textarea name=comments COLS=55 ROWS=10 wrap=soft></textarea><br>\n";
   print "<input type=submit value=\"Post\"><input type=reset value=\"Quit\">Enter message above and press \"post\".<br>\n";
   print "<center>Optional Entries:</center><br>\n";
   print "URL of a web page to display:<br>\n";
   print "<input type=text name=pageurl size=15 maxlength=90><br>\n";
   print "Title of the web page:<br>\n";
   print "<input type=text name=ptitle size=15><br>\n";
   if ($allowgifs eq '1') {
   print "Image to include in your post:<br>\n";
   print "<input type=text name=limage size=15 maxlength=60><br>\n";
   }
   print "header color:<input type=text name=headcolor size=6><br>\n";
   print "msg body color:<input type=text name=bodycolor size=6>\n";
   print "</form></body></html>\n";
   exit;
}


# Log the Entry or Error
sub log {
   $log_type = $_[0];
   open (LOG, ">>$rootpath$sublog");
   if ($log_type eq 'entry') {
      print LOG "$shortdate-SUCCESS: $ENV{'REMOTE_HOST'} - $FORM{'subnumber'}<br>\n";
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
}