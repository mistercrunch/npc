public class GenThread extends Thread {

  private Sync sync;
  private int lastPort;

  public GenThread(Sync s, int lastPort) {
    sync = s;
    this.lastPort = lastPort;
  }

  public void run() {
    for (int x = 1; x <= lastPort; x++) {
      sync.give(x);
      try {
        sleep(10);
      } catch (InterruptedException e) { }
    }
  System.exit(0);
  }
}
