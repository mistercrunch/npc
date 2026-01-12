import java.net.*;
import java.io.*;

public class ScanThread extends Thread {

  private Sync sync;
  private String hostname;
  private boolean open;
  Socket scanSock = null;

  public ScanThread(Sync s, String hostname) {
    sync = s;
    this.hostname = hostname;
  }

  public void run() {

    int port;

    for (;;) {
      port=sync.take();
      open = true;

      try {
        scanSock = new Socket(hostname, port);
        scanSock.close();
      } catch (UnknownHostException e) {
        System.out.println("Host: " + hostname + " is non-existant");
        System.exit(1);
      } catch (IOException e) {
        open = false;
      }

      if (open) { System.out.println("Port " + port + " is Open"); }
    } 
  }
}
