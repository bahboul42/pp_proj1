public class Bank {
    private int cash;
    
    public synchronized void deposit() { cash += 1; notify(); }

    public synchronized void withdraw() {
	if (cash == 0)
	    try {
		wait();
	    } catch (InterruptedException e) {
		throw new RuntimeException(e); }
	cash -= 1;
	assert(cash >= 0);
    }

    public synchronized int value() { return cash; }

    public synchronized void reset() { cash = 0; }
}
