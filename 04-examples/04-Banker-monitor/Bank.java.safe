public class Bank {
    private int cash;
    
    public synchronized void deposit(int n) { cash += n; }

    public synchronized void withdraw(int n) {
	if (cash >= n)
	    cash -= n;
    }

    public synchronized int value() { return cash; }

    public synchronized void reset() { cash = 0; }
}
