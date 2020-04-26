import threading
from time import sleep, strftime
from datetime import datetime
import threading 
import ctypes

class display_manager(threading.Thread): 
    
    display = None
    message = ""
    top = 0

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
                self.manage()
            sleep(.8)


    def manage(self):
        self.top = len(self.message)
        count = 0
        helper = self.message[0:15]
        self.display.clear()
        self.display.message(helper + "\n" + datetime.now().strftime('%b %d  %H:%M:%S\n'))
        for x in range(0, self.top):
            self.display.move_right()
            sleep(.1)
    
    def pause(self):
        self.paused = True
        self.pause_cond.acquire()

    def resume(self):
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
