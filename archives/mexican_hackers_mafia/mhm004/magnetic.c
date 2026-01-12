/****************************************************************************
*            Program to use the experimental MSR, ver. beta 3.10            *
*                                                                           *
*                                    by                                     *
*                                                                           *
*                          Luis Padilla Visdomine.                          *
*                                                                           *
*                           Madrid. January 1997.                           *
****************************************************************************/


#include     <dos.h>
#include    <time.h>
#include    <math.h>
#include   <stdio.h>
#include  <stdlib.h>


#define TRUE   1
#define FALSE  0

#define HIGH  TRUE
#define LOW   FALSE

#define BYTBCD    5   /*  Bits in a  BCD   byte  */
#define BYTALPHA  7   /*  Bits in an ALPHA byte  */

#define SSBCD    0x000B  /*  Start sentinel in ANSI BCD    */
#define SSALPHA  0x0005  /*  Start sentinel in ANSI ALPHA  */
#define ESBCD    0x000F  /*  End   sentinel in ANSI BCD    */
#define ESALPHA  0x001F  /*  End   sentinel in ANSI ALPHA  */

#define CLKLOW   20  /*  Max. clocking bits to use in low  density format  */
#define CLKHIGH  60  /*  Max. clocking bits to use in high density format  */

#define OFFBCD    48  /*  Start of BCD   characters in ASCII table  */
#define OFFALPHA  32  /*  Start of ALPHA characters in ASCII table  */

#define JPORT  0x0201  /*  Status joystick port                 */
#define JBIT   0x0010  /*  Pin 2  joystick port (Pin 4 is gnd)  */
#define JPOS   4       /*  Bit position                         */

#define PPORT  0x0379  /*  Status parallel port                  */
#define PBIT   0x0008  /*  Pin 15 parallel port (Pin 18 is gnd)  */
#define PPOS   3       /*  Bit position                          */

#define PORT  PPORT  /*  Port          */
#define BIT   PBIT   /*  Pin port      */
#define POS   PPOS   /*  Bit position  */

#define NUM   2000    /*  Maximum number of switches to record  */
#define THD   5L      /*  Minimum ticks to be a real switch     */


#define SQR(x)    ((x) * (x))
#define ROUND(x)  ((int) ((float) (x) + 0.5))


main (int argc, char * argv[])
{
    register int i, levelnow, lastlevel = HIGH;
    int j, num, lasttriglevel = HIGH, level[NUM], data, bit[NUM], parity;
    int byte[NUM/BYTBCD], count0[2], count1[2], CheckLRC, LRCBit[BYTALPHA];
    int LRC, byt, off, ss, es, clk;

    register long int counter, ticks;
    long int lasttrigticks = 0L, duration[NUM];

    float sumdur0[2], sumdur1[2], sumsqrdur0[2], sumsqrdur1[2], meandur0[2];
    float meandur1[2], sigmadur0[2], sigmadur1[2], threshold1[2];
    float threshold2[2], threshold3[2];

    clock_t time1, time2;


    printf ("\n");


/*  Processing command line argument  */

    if (argc != 3)
    {
        printf ("Usage: MAGNETIC <track> <clocking_bits>\n\n");
        exit (2);
    }

    if (atoi (argv[1]) == 1)
    {
        byt = BYTALPHA;
        off = OFFALPHA;
        ss  = SSALPHA;
        es  = ESALPHA;
        clk = CLKHIGH;
    }
    else if (atoi (argv[1]) == 2)
    {
        byt = BYTBCD;
        off = OFFBCD;
        ss  = SSBCD;
        es  = ESBCD;
        clk = CLKLOW;
    }
    else if (atoi (argv[1]) == 3)
    {
        byt = BYTBCD;
        off = OFFBCD;
        ss  = SSBCD;
        es  = ESBCD;
        clk = CLKHIGH;
    }
    else if (atoi (argv[1]) == 4)  /*  For non standard tracks  */
    {
        byt = BYTBCD;
        off = OFFBCD;
        ss  = ESBCD + 1;  /*  Non existent  */
        es  = ESBCD + 1;  /*  Non existent  */
        clk = CLKLOW;
    }
    else
    {
        printf ("Bad track number.\n\n");
        exit (3);
    }

    if (atoi (argv[2]) > 3)
        clk = atoi (argv[2]);
    else
        printf ("Bad clocking bits parameter. Using defaults.\n");


    printf ("Reading track %i.\n", atoi (argv[1]));

    i = 0;
    counter = 0L;
    ticks = 0L;

    time1 = clock ();

    do  /*  Loop of track reading  */
    {
        counter++;  /*  Increment of our aprox. time counter  */

        levelnow = (inp (PORT) & BIT) >> POS;  /*  Reading of input  */

/*        levelnow = ((int) counter & BIT) >> POS;  /*  Fake data  */

        if (levelnow == lastlevel)  /*  Constant level  */
            ticks++;

        else if (ticks >= THD && levelnow == lasttriglevel)
                            /*  Change of level after enough time: switch  */
        {
            level[i] = lasttriglevel;
            duration[i] = lasttrigticks;
            i++;
            lasttriglevel = lastlevel;
            lasttrigticks = ticks;
            ticks = 1L;
        }
        else  /*  Either the signal was too short or there
                 is no switch of the trigger level         */
        {
            lasttrigticks += ticks;
            ticks = 1L;
        }

        lastlevel = levelnow;
    }
    while (! ((ticks > 10000L && ticks < counter) || i == NUM));

    time2 = clock ();

    num = i;  /*  Number of switches  */

    if (num == NUM)
         printf ("WARNING: Buffer overflow.\n");

    printf ("Swipe time: %.2f s.\n", (float) (time2 - time1)/CLK_TCK/(float)
                                  counter * (float) (counter - duration[0]));
    printf ("Time unit: %.1e s.\n", (float) (time2 - time1)/CLK_TCK/(float)
                                                                    counter);


/*  Output data to be used with data utilities  */

/*    counter = 0L;

    for (i = 2; i < num - 1; i++)
    {
        printf ("%li %li %i\n", counter, duration[i], level[i]);
        counter += duration[i];
    }  /*  */


/*  Output data for human reader  */

/*    for (i = 0; i < num; i++)

        if (level[i] == HIGH)
            printf ("%s%li%s", "--", duration[i], "--");
        else
            printf ("%s%li%s", "__", duration[i], "__");

    printf ("\n");  /*  */


/*  Recognition of bits  */

/*  The calculation of the bit duration is made first with the clocking
   bits and then is updated with every new bit recognized. Note that the
   duration is different depending whether the bit was HIGH or LOW, this
   is due to an asymmetry of the square wave of the reader               */

    sumdur0[LOW] = 0.0;
    sumsqrdur0[LOW] = 0.0;
    count0[LOW] = 0;
    sumdur0[HIGH] = 0.0;
    sumsqrdur0[HIGH] = 0.0;
    count0[HIGH] = 0;

/*  Calculates the mean with clocking bits  */

    for (i = 2; i < clk && i < num - 1; i++)  /*  Leading clocking bits  */
    {
        sumdur0[level[i]] += (float) duration[i];
        sumsqrdur0[level[i]] += (float) SQR(duration[i]);
        count0[level[i]]++;
    }

    for (i = num - 1 - clk; i < num - 1; i++)  /*  Trailing clocking bits  */
    {
        sumdur0[level[i]] += (float) duration[i];
        sumsqrdur0[level[i]] += (float) SQR(duration[i]);
        count0[level[i]]++;
    }

/*  Estimates initial durations  */

    meandur0[LOW] = sumdur0[LOW]/(float) count0[LOW];
    sigmadur0[LOW] = (float) sqrt (sumsqrdur0[LOW]/(float) count0[LOW]
                                                       - SQR(meandur0[LOW]));
    meandur0[HIGH] = sumdur0[HIGH]/(float) count0[HIGH];
    sigmadur0[HIGH] = (float) sqrt (sumsqrdur0[HIGH]/(float) count0[HIGH]
                                                      - SQR(meandur0[HIGH]));

    sumdur1[LOW] = sumdur0[LOW]/2.0;
    sumsqrdur1[LOW] = sumsqrdur0[LOW]/4.0;
    count1[LOW] = count0[LOW];
    meandur1[LOW] = meandur0[LOW]/2.0;
    sigmadur1[LOW] = sigmadur0[LOW]/2.0;

    sumdur1[HIGH] = sumdur0[HIGH]/2.0;
    sumsqrdur1[HIGH] = sumsqrdur0[HIGH]/4.0;
    count1[HIGH] = count0[HIGH];
    meandur1[HIGH] = meandur0[HIGH]/2.0;
    sigmadur1[HIGH] = sigmadur0[HIGH]/2.0;

    printf ("Initial duration (sigma) of bits in time units:\n");
    printf ("0LO: %5.1f (%4.1f), 1LO: %5.1f (%4.1f)\n", meandur0[LOW],
                              sigmadur0[LOW], meandur1[LOW], sigmadur1[LOW]);
    printf ("0HI: %5.1f (%4.1f), 1HI: %5.1f (%4.1f)\n", meandur0[HIGH],
                           sigmadur0[HIGH], meandur1[HIGH], sigmadur1[HIGH]);

    threshold1[LOW] = meandur0[LOW] - 2.0 * sigmadur0[LOW];
    threshold2[LOW] = meandur1[LOW] + 2.0 * sigmadur1[LOW];
    threshold3[LOW] = meandur0[LOW] - sigmadur0[LOW] * (meandur0[LOW]
                          - meandur1[LOW])/(sigmadur0[LOW] + sigmadur1[LOW]);

    threshold1[HIGH] = meandur0[HIGH] - 2.0 * sigmadur0[HIGH];
    threshold2[HIGH] = meandur1[HIGH] + 2.0 * sigmadur1[HIGH];
    threshold3[HIGH] = meandur0[HIGH] - sigmadur0[HIGH] * (meandur0[HIGH]
                       - meandur1[HIGH])/(sigmadur0[HIGH] + sigmadur1[HIGH]);

    printf ("Initial thresholds LO: %5.1f, %5.1f, %5.1f\n", threshold1[LOW],
                                           threshold2[LOW], threshold3[LOW]);
    printf ("Initial thresholds HI: %5.1f, %5.1f, %5.1f\n", threshold1[HIGH],
                                         threshold2[HIGH], threshold3[HIGH]);

    bit[0] = FALSE;
    for (i = 2, j = 1; i < num - 1; i++, j++)
    {
        if (duration[i] <= THD + 5L)
            printf (" >");

        if ((float) duration[i] > threshold3[level[i]])
        {
            bit[j] = FALSE;
            sumdur0[level[i]] += (float) duration[i];  /*  Updates mean  */
            sumsqrdur0[level[i]] += (float) SQR(duration[i]);
            count0[level[i]]++;
            printf ("0");
        }
        else
        {
            bit[j] = TRUE;
            sumdur1[level[i]] += (float) duration[i];  /*  Updates mean  */
            sumsqrdur1[level[i]] += (float) SQR(duration[i]);
            count1[level[i]]++;
            if (atoi (argv[1]) != 4)  /*  For non standard track  */
                i++;
            printf ("1");
        }

        if (fmod ((double) j, 78.0) < 0.0001)
            printf ("\n");
 
        meandur0[LOW] = sumdur0[LOW]/(float) count0[LOW];
        sigmadur0[LOW] = (float) sqrt (sumsqrdur0[LOW]/(float) count0[LOW]
                                                       - SQR(meandur0[LOW]));
        meandur0[HIGH] = sumdur0[HIGH]/(float) count0[HIGH];
        sigmadur0[HIGH] = (float) sqrt (sumsqrdur0[HIGH]/(float) count0[HIGH]
                                                      - SQR(meandur0[HIGH]));

        meandur1[LOW] = sumdur1[LOW]/(float) count1[LOW];
        sigmadur1[LOW] = (float) sqrt (sumsqrdur1[LOW]/(float) count1[LOW]
                                                       - SQR(meandur1[LOW]));
        meandur1[HIGH] = sumdur1[HIGH]/(float) count1[HIGH];
        sigmadur1[HIGH] = (float) sqrt (sumsqrdur1[HIGH]/(float) count1[HIGH]
                                                      - SQR(meandur1[HIGH]));

        threshold1[LOW] = meandur0[LOW] - 2.0 * sigmadur0[LOW];
        threshold2[LOW] = meandur1[LOW] + 2.0 * sigmadur1[LOW];
        threshold3[LOW] = meandur0[LOW] - sigmadur0[LOW] * (meandur0[LOW]
                          - meandur1[LOW])/(sigmadur0[LOW] + sigmadur1[LOW]);

        threshold1[HIGH] = meandur0[HIGH] - 2.0 * sigmadur0[HIGH];
        threshold2[HIGH] = meandur1[HIGH] + 2.0 * sigmadur1[HIGH];
        threshold3[HIGH] = meandur0[HIGH] - sigmadur0[HIGH] * (meandur0[HIGH]
                       - meandur1[HIGH])/(sigmadur0[HIGH] + sigmadur1[HIGH]);
    }

    num = j;  /*  Number of bits  */
    printf ("\n");

    printf ("Mean duration (sigma) of bits in time units:\n");
    printf ("0LO: %5.1f (%4.1f), 1LO: %5.1f (%4.1f)\n", meandur0[LOW],
                              sigmadur0[LOW], meandur1[LOW], sigmadur1[LOW]);
    printf ("0HI: %5.1f (%4.1f), 1HI: %5.1f (%4.1f)\n", meandur0[HIGH],
                           sigmadur0[HIGH], meandur1[HIGH], sigmadur1[HIGH]);

    printf ("Final thresholds LO: %5.1f, %5.1f, %5.1f\n", threshold1[LOW],
                                           threshold2[LOW], threshold3[LOW]);
    printf ("Final thresholds HI: %5.1f, %5.1f, %5.1f\n", threshold1[HIGH],
                                         threshold2[HIGH], threshold3[HIGH]);


/*  Grouping bits into bytes and checking parity and LRC  */

    data = 0;

    CheckLRC = FALSE;
    for (j = 0; j < byt; j++)
        LRCBit[j] = FALSE;

    for (i = 1; i < num; i++)
        if (bit[i] || data > 0)
        {
            parity = FALSE;
            byte[data] = 0;
            for (j = 0; j < (byt - 1) && i < num; j++, i++)
            {
                byte[data] += bit[i] * ROUND(pow (2.0, (float) j));
                if (! CheckLRC)
                    LRCBit[j] ^= bit[i];
                parity ^= bit[i];
            }

            if (bit[i] == parity && atoi (argv[1]) != 4)
            /*  ^parity bit is odd, ^for non standard tracks  */
            {
                printf ("\nParity error in byte %i. Exiting.\n\n", data + 1);
                exit (-1);
            }

            if (CheckLRC)
            {
                LRC = 0;
                for (j = 0; j < byt - 1; j++)
                    LRC += LRCBit[j] * ROUND(pow (2.0, (float) j));

                if (LRC == byte[data])  /*  LRC bits are even  */
                {
                    printf ("LRC OK.\n\n");
                    exit (0);
                }
                else
                {
                    printf ("LRC error.\n\n");
                    exit (-1);
                }
            }

            if (byte[data] == ss)
            {
                printf ("Start sentinel found.\n");
                printf ("%c", (char) (byte[data] + off));
            }
            else if (byte[data] == es)
            {
                CheckLRC = TRUE;
                printf ("%c", (char) (byte[data] + off));
                printf ("\nEnd sentinel found.\n");
            }
            else
                printf ("%c", (char) (byte[data] + off));

            data++;

            if (fmod ((double) data, 78.0) < 0.0001 && ! CheckLRC)
                printf ("\n");

        }

    printf ("\n");

    printf ("\n");


    return 1;
}
