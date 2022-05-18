import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class World {
    // State
    private static Bank bank = new Bank();

    // State for testing infrastructure
    private static final CyclicBarrier barrier = new CyclicBarrier(3);
    private static int i;

    public static void main(String[] args) throws InterruptedException, BrokenBarrierException {
	final Thread Thread1 = new Thread(() -> {
		for (int j = 10000; j> 0; j--) {
		    try {
			barrier.await();
			int i;
			for (i = 0; i < 5; i++)
			    bank.deposit(2);
			for (i = 0; i < 5; i++)
			    bank.withdraw(2);
			barrier.await();
		    } catch (InterruptedException | BrokenBarrierException e) {
			throw new RuntimeException(e);
		    }
		}
	});
	final Thread Thread2 = new Thread(() -> {
		for (int j = 10000; j> 0; j--) {
		    try {
			barrier.await();
			int i;
			for (i = 0; i < 5; i++)
			    bank.deposit(2);
			for (i = 0; i < 5; i++)
			    bank.withdraw(2);
			barrier.await();
		    } catch (InterruptedException | BrokenBarrierException e) {
			throw new RuntimeException(e);
		    }
		}
	});

	Thread1.start();
	Thread2.start();

	for (i = 0; i < 10000; ++i) {
	    bank.reset();
	    barrier.await(); // Let both threads run...
	    barrier.await(); // ...and wait until they are done
	    if (bank.value() != 0)
	        System.out.println("Wrong value at loop " + i);
	}
	Thread1.join();
	Thread2.join();
    }
}
