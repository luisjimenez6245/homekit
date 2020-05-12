
#define PIN_ESP_READ 7
#define PIN_ESP_WRITE 9
#define PIN_RELAY 4
#define PIN_SWITCH 5

boolean led_state = false;
int last_switch_state = 0;
unsigned long last_debounce_time = 0;  
unsigned long debounce_delay = 60;

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
    delay(100);
    if(actual_led_state == !digitalRead(PIN_ESP_READ)){
      digitalWrite(PIN_RELAY,actual_led_state);     
      if(actual_led_state != led_state){
        led_state = actual_led_state;
      }
    }
}


void loop() {
  int switch_state = digitalRead(PIN_SWITCH);
  if(switch_state != last_switch_state){
    if(last_debounce_time == 0){
      last_debounce_time = millis();
    }
    if ((millis() - last_debounce_time) > debounce_delay) {
      last_debounce_time = 0;
      Serial.println("cambie");
      last_switch_state = switch_state;
      digitalWrite(PIN_ESP_WRITE, LOW);
      delay(100);
      digitalWrite(PIN_ESP_WRITE, HIGH);
    }
  }
  manage_esp();
}
