import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class World {
    // State
    private static Bank bank = new Bank();

    // State for testing infrastructure
    private static final CyclicBarrier barrier = new CyclicBarrier(4);
    private static int i;

    public static void main(String[] args) throws InterruptedException, BrokenBarrierException {
	final Thread Thread1 = new Thread(() -> {
		while (true) 
		try {
		    barrier.await();
		    Thread.sleep(2);
		    bank.deposit();
		    Thread.sleep(2);
		    bank.deposit();
		    barrier.await();
		} catch (InterruptedException | BrokenBarrierException e) {
		    throw new RuntimeException(e);
		}
	});
	final Thread Thread2 = new Thread(() -> {
		while (true) 
		try {
		    barrier.await();
		    Thread.sleep(1);
		    bank.withdraw();
		    barrier.await();
		} catch (InterruptedException | BrokenBarrierException e) {
		    throw new RuntimeException(e);
		}
	});
	final Thread Thread3 = new Thread(() -> {
		while (true) 
		try {
		    barrier.await();
		    Thread.sleep(2);
		    bank.withdraw();
		    barrier.await();
		} catch (InterruptedException | BrokenBarrierException e) {
		    throw new RuntimeException(e);
		}
	});


	Thread1.start();
	Thread2.start();
	Thread3.start();

	while (true)
	    {
		barrier.await(); // Let both threads run...
		barrier.await(); // ...and wait until they are done
		System.out.println("OK");
	    }
	
    }
}
