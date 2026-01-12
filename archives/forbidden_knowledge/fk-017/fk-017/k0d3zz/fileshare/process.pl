# phyleshare!, by wzd

$keyword = "w4r3z";
$sharedir = "C:\\myw4r3z";

use Win32::OLE;

eval {$olook = Win32::OLE->GetActiveObject('Outlook.Application')};
die "Outlook not installed\n" if $@;
unless (defined $olook) {
  $olook = Win32::OLE->new('Outlook.Application', 'Quit')
    || die "Cannot start Outlook\n";
}

$ns = $olook->GetNameSpace("MAPI");
$inbox = $ns->GetDefaultFolder(6);

for ($i = 1; $i <= $inbox->Items->Count(); $i++) {

  $_ = $inbox->Items($i)->Subject();

  if ($_ =~ /^$keyword$/) {

    opendir(DIR, $sharedir);
    @_ = readdir(DIR);
    closedir(DIR);
    for ($x = 2; $x < (@_); $x++) {

      if(-d @_[$x]) {
        $list .= (@_[$x] . "\\\n");
      } else {
        $list .= (@_[$x] . "\n");
      }
    }
    $reply = $inbox->Items($i)->Reply();
    $reply->{'Body'} = $list;
    $reply->Send();
  }

  if ($_ =~ /^$keyword\s(.+?)$/) {

    if ($1 !~ /(\.\.)|(\%)/) { # no '..', no unic0de nons3nz.. - is this fine?

      if (-f ($sharedir."\\".$1)) {

        $reply = $inbox->Items($i)->Reply();
        $reply->Attachments->Add($sharedir."\\".$1);
	$reply->{'Body'} = "\n\n-phyleshare!, by wzd";
        $reply->Send();
      }

      elsif (-d ($sharedir."\\".$1)) {

        opendir(DIR, $sharedir."\\".$1);
        @_ = readdir(DIR);
        closedir(DIR);
        for ($x = 2; $x < (@_); $x++) {

          if(-d @_[$x]) {
            $list .= (@_[$x] . "\\\n");
          } else {
            $list .= (@_[$x] . "\n");
          }
        }
        $reply = $inbox->Items($i)->Reply();
        $reply->{'Body'} = $list;
        $reply->Send();
      }

      else {

        $reply = $inbox->Items($i)->Reply();
        $reply->{'Body'} = 'File Not Found';
        $reply->Send();
      }
    }
  }
}