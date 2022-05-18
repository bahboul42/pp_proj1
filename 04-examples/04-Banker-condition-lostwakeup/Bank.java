public class Bank {
    private int cash;
    
    public synchronized void deposit(int n) { cash += n; notify(); }

    public synchronized void withdraw(int n) {
	while (cash < n)
	    try {
		wait();
	    } catch (InterruptedException e) {
		throw new RuntimeException(e); }
	cash -= n;
    }

    public synchronized int value() { return cash; }

    public synchronized void reset() { cash = 0; }
}
