from threading import *
import random, time

class LockerRoom:
    def __init__(self):
        #don't use occupants in your solution
        self.occupants = []
        self.lock = Lock() 
        self.isEmpty = Condition(self.lock)
        self.genderInRoom = ""
        self.nFull = 0

    def enter(self, gender):
        self.lock.acquire()
        if (self.nFull == 0):
            self.occupants.append(gender)
            self.nFull += 1
            self.genderInRoom = gender
        else:
            if self.genderInRoom == gender:
                self.occupants.append(gender)
                self.nFull += 1
            else:
                while(self.nFull > 0):
                    self.isEmpty.wait()
                self.occupants.append(gender)
                self.nFull += 1
                self.genderInRoom = gender

        print self.occupants 
        self.lock.release()

    def leave(self, gender):
        self.lock.acquire()
        self.occupants.remove(gender)
        self.nFull -= 1
        print self.occupants
        if(self.nFull == 0):
            self.gender = ""
            self.isEmpty.notify()
        self.lock.release()

def male(lockerRoom):
    lockerRoom.enter('male')
    time.sleep(random.random() / 2)
    lockerRoom.leave('male')

def female(lockerRoom):
    lockerRoom.enter('female')
    time.sleep(random.random() / 2)
    lockerRoom.leave('female')

def loop(lockerRoom, f, mu=1):
    while True:
        t = random.expovariate(1.0/mu)
        time.sleep(t)
        f(lockerRoom)

lockerRoom = LockerRoom()
fs = [male]*5 + [female]*5
threads = [Thread(target=loop, args=(lockerRoom, f)) for f in fs]
for thread in threads: thread.start()
for thread in threads: thread.join()

