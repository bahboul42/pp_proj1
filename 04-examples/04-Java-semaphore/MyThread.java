import java.util.concurrent.Semaphore;

public class MyThread extends Thread {
  static private final Semaphore semaphore =
      new Semaphore(2, true);

  private final int id;
  
  public MyThread(int i) {
    id = i;
  }

  public void run() {
    int i = 30;
    for (; i > 0; i--) {
      try {
	semaphore.acquire();
	Thread.sleep(1000);
	System.out.println("Thread "+id+
			   " entering critical section");
        Thread.sleep(1000);
	System.out.println("Thread "+id+
			   " exiting critical section");
	semaphore.release();
      }
      catch(InterruptedException ex){
	Thread.currentThread().interrupt();
      }
    }
  }
}
