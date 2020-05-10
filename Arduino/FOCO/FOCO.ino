
#define PIN_ESP_READ 7
#define PIN_ESP_WRITE 9
#define PIN_RELAY 4
#define PIN_SWITCH 5

boolean led_state = false;
int last_switch_state = 0;

void manage_led(){
  /*if(led_state){
      Serial.println("Prendido\n");
  }
  else{    
    Serial.println("Apagado\n");
  }*/
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_ESP_READ, INPUT);
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_SWITCH, INPUT);
    digitalWrite(PIN_RELAY, LOW);
    delay(1500);
    pinMode(PIN_ESP_WRITE, OUTPUT);
    digitalWrite(PIN_ESP_WRITE, HIGH);
    last_switch_state = digitalRead(PIN_SWITCH);
}


void manage_esp(){
    boolean actual_led_state = !digitalRead(PIN_ESP_READ);
    //Serial.print("esp_state:");
    //Serial.println(actual_led_state);  
    digitalWrite(PIN_RELAY,actual_led_state);     
    if(actual_led_state != led_state){
      led_state = actual_led_state;
      manage_led();
    }
    //delay(2000);
}


void loop() {
  int switch_state = digitalRead(PIN_SWITCH);
  //Serial.print("switch_state:");
  //Serial.println(switch_state);
  if(switch_state != last_switch_state){
    Serial.println("cambie");
    last_switch_state = switch_state;
    digitalWrite(PIN_ESP_WRITE, LOW);
    delay(100);
    digitalWrite(PIN_ESP_WRITE, HIGH);
  }
  manage_esp();
 // Serial.print("led_state:");
 // Serial.println(led_state);
}
