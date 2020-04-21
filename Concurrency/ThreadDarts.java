import java.lang.Math;

// Estimate pi using "darts"
public class ThreadDarts {
  int N;
  int NT;
  // Keep track of how many total darts hit in the unit circle
  int overall_yes = 0;
  // Keep track of how many total darts are thrown
  // (hitting either inside or outside of the unit circle)
  int total = 0;
  // N is the number of darts that each thread will throw
  // NT is the number of threads
  public ThreadDarts(int N, int NT) {
	  this.N = N;
	  this.NT = NT;
  }

  public void run() throws InterruptedException {
	// The lock
    final Object mutex = new Object();
    // The code that each thread runs
    Runnable threadCode = () -> {
      // Keep track of how many darts of yours have landed in the unit circle
      int local_yes = 0;
      // Throw N darts
      for (int i = 0; i < N; i++) {
    	// Get a random (x, y) in [0, 1)
    	double x = Math.random();
    	double y = Math.random();
    	// Check if that (x, y) are in the unit circle
    	// If so, add to local_yes
    	if (x*x + y*y <= 1) {
    		local_yes += 1;
    	}
      }
      // With the lock, update the overall_yes and the total
      synchronized (mutex) {
          overall_yes += local_yes;
          total += N;
      }
    };

    // Create and start the threads!
    Thread[] threads = new Thread[NT];
    for (int i = 0; i < NT; i++) {
    	Thread t = new Thread(threadCode);
    	threads[i] = t;
    	t.start();
    }
    // Wait for the threads to finish
    for (Thread t : threads) {
    	t.join();
    }
    // Area of the unit circle is equal to PI*r*r = PI*1*1 = PI
    // We are looking at the area of 1 quadrant out of 4
    System.out.printf("%1.9f\n", overall_yes/(total/4.0));
  }

  // Actually run the application, tracking how long it takes
  public static void main(String[] args) throws InterruptedException {
    ThreadDarts app = new ThreadDarts(2_000_000, 4);
    long start = System.nanoTime();
    app.run();
    long end = System.nanoTime();
    System.out.printf("time: %1.3f seconds", ((end-start) / Math.pow(10, 9)));
    
    /*
     * Sample print:
     * 3.142023500
	 * time: 2.537 seconds
	 */
  }
}