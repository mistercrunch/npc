# phyleshare! cleanup.pl script, by wzd

$keyword = "w4r3z";

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
  if ($_ =~ /^$keyword/) {
    $inbox->Items($i)->Delete();
  }
}

$dirtbox = $ns->GetDefaultFolder(3);
for ($i = 1; $i <= $dirtbox->Items->Count(); $i++) {
  $_ = $inbox->Items($i)->Subject();
  if ($_ =~ /^$keyword/) {
    $inbox->Items($i)->Delete();
    $x++;
  }
}

print "Deleted $x messages\n";