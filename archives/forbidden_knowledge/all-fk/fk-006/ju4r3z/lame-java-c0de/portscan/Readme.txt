Portscan.java version 0.6 by Wyzewun
--==--==--==--==--==--==--==--==--==

-/- So what the hell is this anyway?

    An extremely simple, threaded, cross-platform Portscanner written in Java
    (of all things =P)

-/- Why is it super-kewl?

    Heh, well, with L0pht releasing a Wardialer for the PalmPilot etc, I
    figured we may as well get some nice Hacking/Security tools for the Java
    Platform - after all, there's a Java VM for every single major PDA. Uuh,
    Except Windows CE, but those aren't *Real* PDA's. Oh yeh, and I suppose
    all those luzers with arb OS's like OS/2 might like this too. =P

-/- Why is it super-stupid?

    It uses System.out instead of having a funky GUI effort, and because not
    all OS's have the concept of a standard output - its not truly platform
    independant yet. Yucky. I will have a nice JFC/Swing GUI by 0.7 hopefully.
    I still haven't gotten round to learning the JFC yet. ;P

-/- Your code sucks. Can I fix it?

    Yes, please - If you can find somewhere where the code can be optimized,
    or otherwise improved, please mail a diff of your proposed changes to me
    at w1@antioffline.com - All help is greatly appreciated.

-/- Oh, and one more thing...

    The Bytecode included was compiled with JDK 1.2.1, but the results of a
    javac -release 1.1 Portscan.java produced an identical result, so I
    assume that the bytecode is friendly to 1.1.x VM's as well.