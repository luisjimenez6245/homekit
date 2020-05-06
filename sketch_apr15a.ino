const int buttonPin = 4;     
const int relayPin =  7;     
const int kitPin = 3;
const int notificationPin = 6;
const int timeButtonReset = 15000;
const int timeWaiting = 6;

int buttonState = LOW;
int doorState = 0;
int kitState=0;


int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long timePressed = 0;

int sendNotification(){
  digitalWrite(notificationPin, HIGH);
  delay(100);
  digitalWrite(notificationPin, LOW);
  delay(100);
  kitState = digitalRead(kitPin);
  return(kitState != LOW && doorState == 0);
}

void manageRelay(int state){
   if(state != doorState){
      doorState = state;
   }
   if(doorState == 1){
        digitalWrite(relayPin, HIGH);
        delay(1000);
   }
   else{
      digitalWrite(relayPin, LOW);
   }
}


void manageButton(){
  if((millis() - timePressed) >= timeButtonReset){
    digitalWrite(notificationPin, HIGH);
    delay(6000);
    digitalWrite(notificationPin, LOW);
  }
  else{
    int noti = 1;
    while(buttonState == HIGH){
      while(noti){
        noti = sendNotification();
      }
      Serial.println(kitState);
      manageRelay(kitState);
      kitState = digitalRead(kitPin);
      buttonState = digitalRead(buttonPin);
    }
    manageRelay(0);
    digitalWrite(notificationPin, HIGH);
  }
    /*
 
  */
}

void manageKit(){
  delay(timeWaiting * 100);
  if(digitalRead(kitPin) == LOW){
     delay(timeWaiting * 100);
     if(digitalRead(kitPin) == LOW){
        manageRelay(1);
     }
  } 
  manageRelay(0);
}


void setup() {
  Serial.begin(115200); 
  pinMode(relayPin, OUTPUT);      
  pinMode(kitPin, INPUT_PULLUP);
  pinMode(notificationPin, OUTPUT);
  pinMode(buttonPin, INPUT);
      digitalWrite(notificationPin, HIGH);
  delay(1000); 
}

void loop(){
  int reading  = digitalRead(buttonPin);
  kitState = digitalRead(kitPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    if(timePressed > 0){
      manageButton();
      timePressed = 0;
    }
  }
  lastButtonState = reading;

   if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        timePressed = millis();
      }
      else{
        timePressed = 0;
      }
    }
  }
  if(kitState == HIGH){
    digitalWrite(relayPin, LOW);
  }
  else{
      if(kitState == LOW){
         Serial.println("home");
         manageKit();
      }
  }
}
