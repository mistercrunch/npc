# phyleshare!get, by wzd

$savedir = "C:\\Leechd";

(@keywords = split /,/, shift)
  || die "syntax: perl psget.pl [keyword1,keyword2,..]";

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

  $s = $inbox->Items($i)->Subject();
  foreach(@keywords) {
    if ($s =~ /$_/) {
      if($inbox->Items($i)->Attachments->Count() = 0) {
        $inbox->Items($i)->SaveAs($savedir."\\".$_.".".(time).".eml");
        $inbox->Items($i)->Delete();
      } elsif ($inbox->Items($i)->Attachments->Count() > 1) {
        for($x = 1; $x < $inbox->Items($i)->Attachments->Count(); $x++) {
          $inbox->Items($i)->Attachments($x)->SaveAsFile($savedir."\\");
        }
        $inbox->Items($i)->Delete();
      }
    }
  }
}
$dirtbox = $ns->GetDefaultFolder(3);
for ($i = 1; $i <= $dirtbox->Items->Count(); $i++) {
  $s = $dirtbox->Items($i)->Subject();
  foreach(@keywords) {
    if ($s =~ /$_/) {
      $dirtbox->Items($i)->Delete();
    }
  }
}