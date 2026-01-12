#!/usr/bin/env perl

$script = "/home/drew/vodasms";

sub xtract {
  while (<>) {
    if ($_ =~ /^\n$/) {
      if (($_ = <>) =~ 'This is a multi-part message in MIME format.') {
        while (<> !~ /^\n$/) {} while (<> !~ /^\n$/) {} $_ = <>; }
      if (length $_ <= 140) {return $_;} else {die "Message too long";}}}}

if (($func = shift) == 1) {
  system ($script, shift, xtract) == 0 || die "system call failed: $?";
} elsif ($func == 2) {
  @_ = split /\s+/, xtract, 3;
  for ($_ = 0; $_ <= @_[1]; $_++) {
    system ($script, @_[0], @_[2]) == 0 || die "system call failed: $?"; }
} else { die "No valid Function specified\n"; }