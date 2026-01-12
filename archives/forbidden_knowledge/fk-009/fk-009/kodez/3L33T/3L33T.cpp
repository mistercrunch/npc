/* dIS iZ 3L33T.cPp bY wYz3wUn -- pHEAR!@#%
   iT w1LL mAkE j0r pHyLez FuQn k-r4d!@#$%^&

   tHe #dArKnEt rEsPonZe...

   <nuwezyw> g++ 3l33t.cpp -o 3l33t
   <Garefunke> but wath is the program supposedd to do?
   <Garefunke> what is the program for?
   <nuwezyw> run ./3l33t sometextfile.txt results.txt
   <Garefunke> ok
   <Garefunke> aha
   <Garefunke> it made the file krypted?
   <nuwezyw> errr
   <nuwezyw> not quite
   <Garefunke> what then?
   <Garefunke> hello?
   <Garefunke> what is the mening with this program?       */

#include <fstream.h>
#include <ctype.h>

int main(int argc, char **argv) {

if (argc != 3) {
  cout << "sYnT4x: 3L33T [1npH1Le] [0uTpHyL3]\n";
  return 0; }

char govboi_iz_jor_mastah;

ifstream we_fuqn_rool(argv[1], ios::in);
if (!we_fuqn_rool) { // oF c0Rz wE fUqn ro0L!@#$%^
  cout << "tHat pHyLe d0ezNt ex1zt j0o lAmh0r\n";
  return 1; }

ofstream eye_luv_hacking_and_zeroday_warez(argv[2]);
while (we_fuqn_rool.get(govboi_iz_jor_mastah)) {
  govboi_iz_jor_mastah = toupper(govboi_iz_jor_mastah); 
  switch (govboi_iz_jor_mastah) {
    case 'A': govboi_iz_jor_mastah = '4'; break;
    case 'E': govboi_iz_jor_mastah = '3'; break;
    case 'O': govboi_iz_jor_mastah = '0'; break;
    case 'S': govboi_iz_jor_mastah = '5'; break;
    case 'I': govboi_iz_jor_mastah = '1'; break;
    case 'G': govboi_iz_jor_mastah = '9'; break;
    case 'T': govboi_iz_jor_mastah = '7'; break; }
    eye_luv_hacking_and_zeroday_warez << govboi_iz_jor_mastah; }
return 0; }