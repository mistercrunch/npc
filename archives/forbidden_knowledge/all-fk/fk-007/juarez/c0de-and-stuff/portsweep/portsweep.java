// portsweep.java by wyze1 - pretend you care

import java.net.*;
import java.io.*;

public class portsweep {
	
  public static void main(String[] args) {
	
    if (args.length != 4) {
      System.out.println("Syntax: java portsweep [1st-octet] [2nd-octet] [3rd-octet] [port]");
      System.out.println("Example: portsweep 143 23 12 80 scans 143.23.12.* for webservers");
      System.exit(0);
    }
	  
    int port = Integer.parseInt(args[3]);
	
    for (int i = 0; i <= 255; i++) {
		
      boolean found = true;
      String host = (args[0] + "." + args[1] + "." + args[2] + "." + i);
		
      try {
        Socket testSok = new Socket(host, port);
        testSok.close();
      } catch (IOException e) {
        found = false;
      }
	  	
      if (found) { System.out.println(host + " has port " + port + " open"); } } } }
