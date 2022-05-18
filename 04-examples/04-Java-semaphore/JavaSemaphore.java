public class JavaSemaphore {

    public static void main(String[] args) {
	MyThread[] thread = new MyThread[4];
	for (int i = 0; i < thread.length; i++) {
	    thread[i] = new MyThread(i);    
	}
	for (int i = 0; i < thread.length; i++) {
	    thread[i].start();
	}
	for (int i = 0; i < thread.length; i++) {
	    try {
		thread[i].join();
	    }
	    catch (InterruptedException e) {
		System.out.println("Interrupted!");
	    }
	}
    }
}
