/* rip of that one thingie on the web. */

#include <stdio.h>
#include <unistd.h>

char *africa[] = {
	"Shenika",
	"Mohammed",
	"Al Jabbar",
	"Shaquita",
	"Mohadupah",
	"Jamal",
	"Shaquille",
	"Devinder",
	"Mola Ram",
	"Laquita",	 // 10
	"Shaft",
	"Anil",
	"Tyrone",
	"Rashid",
	"Raheem",
	"Shahid",
	"Danita",
	"Latoya",
	"Abdul",
	"Jabar",	// 20
	"Kareem",
	"Rashanda",
	"Lamar",
	"Lakeesha",
	"Shaniqua",
	"Kamal",
	"Yaniq",
	"Kamilah",
	"Tireese",	// 30
	"Terrel",
	"Antwon",
	"Rodney",
	"Anfernee",
	"Zambizu",
	"Lomb",
	"Shaka",
	"Zulu",
	"Sharonda",	
	"Boniqua",
	NULL };

char *whitey[] = {
	"John",
	"john",
	"jon",
	"Bob",
	"Sam",
	"Josh",
	"Robert",
	"The White Oppressor",
	"The Man",
	"the man",
	"man",
	"Sean Harney",
	"sean harney",
	"sam",
	"sean",
	"biff",
	"horizon",
	"antilove",
	"jsbach",
	"chrak",
	"tip",
	"duke",
	"jennicide",
	"JP",
	"Rloxley",
	"Kent",
	"Ken",
	NULL };
	
int chk(char *b) {

int n;
char *str;

  for (n = 0 ; n < sizeof(whitey) ; n++) 
  {
         str = whitey[n];
              	if (str == (char *)NULL) 
				return 0;
		if (strcmp(b, str) == 0)
			return 1; 
  }

  return 0;
}

int main(int argc, char *argv[]) {

int i;
char *f, *m, *l, b[20];

  srand (time(NULL)); 
 
  f = africa[rand() % 40];
  m = africa[rand() % 40];
  l = africa[rand() % 40];

  printf ("Hello Whitey. you must cleanse yourself from the evils of the"
	  "blue eyed devilz.\n");
  printf ("Type your name: ");

  scanf ("%20s", b);

  if (chk(b) == 0) 
	{
  	  printf ("%s, you have been cleansed of whitey impuritys. Your new name iz: ", b);
  	  printf ("%s %s %s\n", f, m, l); 
	  exit(0);
	}

  printf ("you, %s, are the white oppressor. damn the man.\n", b); 
  return 0; 

}
