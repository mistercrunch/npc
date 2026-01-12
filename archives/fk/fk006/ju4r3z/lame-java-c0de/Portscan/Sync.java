public class Sync {

  private int contents;
  private boolean available = false;

  public synchronized int take() {
    while (!available) {
      try {
        wait();
      } catch (InterruptedException e) { }
  }

    available = false;
    notifyAll();
    return contents;
  }

  public synchronized void give(int value) {
    while (available) {
      try {
        wait();
      } catch (InterruptedException e) { }
    }
      contents = value;
      available = true;
      notifyAll();
  }
}
