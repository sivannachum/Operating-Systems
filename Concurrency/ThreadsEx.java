// Professor's example
public class ThreadsEx {
  int count = 0;
  int N = 1000;

  private static void sleep(int time) {
    try {
      Thread.sleep(time);
    } catch (InterruptedException ie) {
      throw new RuntimeException("Interrupted (SIGINT)!", ie);
    }
  }

  public void run() throws InterruptedException {
    final Object mutex = new Object();
    Runnable upTask = () -> {
      for (int i = 0; i < N; i++) {
        synchronized (mutex) {
          count += 1;
        }
        // Just calling this because these threads are short
        // and may not run long enough to yield on their own
        sleep(0);

      }
    };
    Runnable downTask = () -> {
      for (int i = 0; i < N; i++) {
        synchronized (mutex) {
          count -= 1;
        }
        sleep(0);
      }
    };

    Thread t1 = new Thread(upTask);
    Thread t2 = new Thread(downTask);
    t1.start();
    t2.start();

    t1.join();
    t2.join();
    System.out.println(count);
  }

  public static void main(String[] args) throws InterruptedException {
    ThreadsEx app = new ThreadsEx();
    app.run();
  }
}