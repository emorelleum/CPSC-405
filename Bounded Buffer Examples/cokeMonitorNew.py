from threading import *
#from threading_cleanup import *
import random, time

MAX = 10

class Shared:
    def __init__(self, start=0, capacity = 10):
        self.lock = Lock()
        self.MAX = capacity
        self.itemAdded = Condition(self.lock)
        self.itemRemoved = Condition(self.lock)
        self.nFull = 0

    def isFull(self):
        return True if self.nFull == MAX else False 

    def isEmpty(self):
        return True if self.nFull == 0 else False 
        
    def remove(self):
        self.lock.acquire()
        while(self.isEmpty()):
            self.itemAdded.wait()
        self.nFull -= 1
        print self.nFull
        self.itemRemoved.notify()
        self.lock.release()

    
    def insert(self):
        self.lock.acquire()
        while(self.isFull()):
            self.itemRemoved.wait()
        self.nFull += 1
        print self.nFull
        self.itemAdded.notify()
        self.lock.release()


def consume(shared):
    #print "consume"
    shared.remove()

def produce(shared):
    #print "Produce"
    shared.insert()


def loop(shared, f, mu=1):
    while True:
        t = random.expovariate(1.0/mu)
        time.sleep(t)
        f(shared)

shared = Shared()
fs = [consume]*5 + [produce]*5 
threads = [Thread(target=loop, args=(shared, f)) for f in fs]
for thread in threads: thread.start()
for thread in threads: thread.join()

