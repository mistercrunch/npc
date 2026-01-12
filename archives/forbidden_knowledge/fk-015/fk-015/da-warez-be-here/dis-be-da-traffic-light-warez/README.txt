well here it is du0ds! the reason for the extended release of fk15 :p

here is the binary and disassembled assembly 0-day leet w4rez c0de from an
eeprom that was removed from a traffic light control box one late (drunk)
night :)

The reason i did it was to see if our traffic lights are connected in some
way. i have seen how the traffic lights change whenever ambulances need to
cross. and i heard rumours about a whole control station where they can
control traffic lights remotly. even today i saw a big intersection go red
for all when a ambulance passed through, then after 30 seconds it went
back to normal. even if this is not true or anything, you can still remove
the eeprom from a robot that bothers you and reprogram the eeprom sequence
and timing to fit your needs ;p the locks are also g0d easy to pick, if u
can pick a fridge lock then your set :) i haven't gone thru the c0de yet
because of wizdump hogging muh ass about deadline *waq* but at least i got
it disassembled. btw use DIS8051.com for it. the chip is a intel 87C51
(for history purpose -> same chips used for viking mars explorer) so any
8051 disassembler would work. it wuz read using a normal universal eeprom
reader. i included the instruction routines so that you can sort of
understand wut's going on.  if there is any type of serial or even network
comm's happening, then it can maybe be accessed remotly ;)  <remember
hackers the 0-day movie.>

the files are (unless wizdump screwed it up)
   
   87C51.BIN   <--- da binary stuph
   87C51.SRC   <--- eish, e'asm
   disasm.txt <--- dis8051 instruction set

   87C51.D51 <-- Another, prettier disassembly plunkett submitted later - Ed.

well... that is about it. if i found anything juicy in the src c0de worth noting i'll notify someone
and maybe tell the world about it! *waq*

cheers - plunkett
