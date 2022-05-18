import time
import threading

thread_cs = 0
semaphore = threading.Semaphore(2)

def mythread(myid):
    global semaphore
    i = 30;
    while i > 0:
        time.sleep(1)
        semaphore.acquire()
        print("Thread %d entering critical section" % myid)
        time.sleep(1)
        print("Thread %d exiting critical section" % myid)
        semaphore.release()

if __name__ == "__main__":
    threads = []

    for i in range(4):
        p = threading.Thread(target=mythread, args=(i,))
        threads.append(p)
        p.start()

    for p in threads:
        p.join()
