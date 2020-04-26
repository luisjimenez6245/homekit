import threading
from time import sleep, strftime
from datetime import datetime
import threading 
import ctypes

class display_manager(threading.Thread): 
    
    display = None
    message = ""
    top = 0
    count = 0

    def __init__(self, display, message = ""): 
        threading.Thread.__init__(self) 
        self.paused = False
        self.pause_cond = threading.Condition(threading.Lock())
        self.display = display
        self.message = message

              
    def run(self):

        while True:
            with self.pause_cond:
                while self.paused:
                    self.pause_cond.wait()
                if(self.top != 0 and self.count != self.top):
                    self.manage()
                else:
                    self.display.set_cursor(0,1)
                    self.display.message(datetime.now().strftime('%b %d  %H:%M:%S\n'))
            sleep(.5)


    def manage(self):
        print("count" + str(self.count) + " top:" + str(self.top))
        self.top = len(self.message)
        self.display.clear()
        helper = self.message[self.count:(self.count + 15)]
        self.display.message(helper + "\n" + datetime.now().strftime('%b %d  %H:%M:%S\n'))
        if(self.count != self.top):
            for x in range(0, self.top):
                if(self.paused):
                    break
                self.display.set_cursor(0,0)
                self.display.message(helper)
                sleep(.5)
                if(x % 2 == 0): 
                    self.display.set_cursor(0,1)
                    self.display.message(datetime.now().strftime('%b %d  %H:%M:%S\n'))
                helper = self.message[x:(x + 15)]
            self.count = self.top
        self.display.set_cursor(0,0)
        self.display.message(self.message[0:15])

    
    def pause(self):
        self.message = ""
        self.top = 0
        self.count = 0
        self.display.set_cursor(0,0)
        self.display.message(self.message)
        self.display.set_cursor(0,1)
        self.display.message(datetime.now().strftime('%b %d  %H:%M:%S\n'))
        self.paused = True
        self.pause_cond.acquire()

    def resume(self):
        self.count = 0
        self.top = len(self.message)
        self.paused = False
        self.pause_cond.notify()
        self.pause_cond.release()
           
    def get_id(self):   
        if hasattr(self, '_thread_id'): 
            return self._thread_id 
        for id, thread in threading._active.items(): 
            if thread is self: 
                return id
   
    def raise_exception(self): 
        thread_id = self.get_id() 
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id, 
              ctypes.py_object(SystemExit)) 
        if res > 1: 
            ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id, 0) 
            print('Exception raise failure') 
