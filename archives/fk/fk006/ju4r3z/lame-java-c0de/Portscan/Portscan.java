public class Portscan {
  public static void main (String[] args) {

    if (args.length != 2) {
      System.out.println("Syntax: java Portscan [host] [stop-port]");
      System.exit(0);
    }

    // Shameless Self-Glorification Banner
    System.out.print("Portscan.java v0.6 by wyze1\n\n");

    int stopPort = Integer.parseInt(args[1]);
    String Hostname = args[0];

    /* Got bandwidth? Got system resources? Feel Macho? Then why not add in 
       some more Scanning threads */

    Sync s = new Sync();
    GenThread gen = new GenThread(s, stopPort);
    ScanThread scan1 = new ScanThread(s, Hostname);
    ScanThread scan2 = new ScanThread(s, Hostname);
    ScanThread scan3 = new ScanThread(s, Hostname);
    ScanThread scan4 = new ScanThread(s, Hostname);

    gen.start();
    scan1.start();
    scan2.start();
    scan3.start();
    scan4.start();

  }
}
