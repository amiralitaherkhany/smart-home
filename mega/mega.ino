#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN 2
#define BUTTON_PIN 22
#define sw180 25
#define led_gas 23
#define sw_led 26
#define LDR_pin 6
#define led_pin 5
#define buzzer_pin 24
#define pir_Pin 3
#define fan_pin 7


int hum;
int temp;
int soil;
int gas_value;
int sw_value = 0;
int sw_counter = 0;
int switchState;
int lastSwitchState = HIGH;
int LDR_state;
int pir_val = 0;
int fan_state = HIGH;
bool pir_work = false;
bool sw_led_state = false;
bool fan_auto = false;
String pir_text;
String pir_State = "LOW";
String fan_text = "Manual";
String fan_state_text = "OFF";
unsigned long lastTime = 0;
unsigned long timerDelay = 500;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial3.begin(115200);
  Serial.begin(115200);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LDR_pin, INPUT);
  pinMode(pir_Pin, INPUT);
  pinMode(sw180, INPUT);
  pinMode(fan_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(led_gas, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(sw_led, OUTPUT);
  digitalWrite(fan_pin, fan_state);
  digitalWrite(led_gas, LOW);
}
void loop() {
  if ((millis() - lastTime) > timerDelay) {

    pir_val = digitalRead(pir_Pin);
    if (pir_work && (pir_val == HIGH)) {
      digitalWrite(buzzer_pin, HIGH);
      pir_State = "HIGH";
    } else {
      pir_State = "LOW";
      digitalWrite(buzzer_pin, LOW);
    }


    if (pir_work == true) {
      pir_text = "ON";
    } else {
      pir_text = "OFF";
    }

    soil = analogRead(A0);
    soil = map(soil, 1023, 0, 0, 100);
    hum = dht.readHumidity();
    temp = dht.readTemperature();






    if (fan_state == LOW) {
      fan_state_text = "ON";
    } else {
      fan_state_text = "OFF";
    }





    String text = "سلام";
    if (soil < 750) {
      text = " نمی خواد به گل ها آب بدی";
    } else {
      text = " به گل ها آب بده";
    }







    LDR_state = digitalRead(LDR_pin);
    if (LDR_state == LOW) {
      digitalWrite(led_pin, LOW);
    } else {
      digitalWrite(led_pin, HIGH);
    }




    gas_value = analogRead(A1);
    if (gas_value > 100) {
      digitalWrite(led_gas, HIGH);
    } else {
      digitalWrite(led_gas, LOW);
    }






    String datastring = '?' + String(temp) + "," + String(hum) + "," + String(soil) + "," + text + "," + pir_State + "," + pir_text + "," + fan_text + "," + fan_state_text;
    Serial3.println(datastring);
    lastTime = millis();
  }


  if (fan_auto) {
    fan_text = "Auto";
    if (temp > 27) {
      fan_state = LOW;
      digitalWrite(fan_pin, fan_state);
    } else {
      fan_state = HIGH;
      digitalWrite(fan_pin, fan_state);
    }
  } else {
    fan_text = "Manual";
    switchState = digitalRead(BUTTON_PIN);

    if (switchState != lastSwitchState) {
      if (switchState == HIGH) {
        fan_state = !fan_state;
        digitalWrite(fan_pin, fan_state);
      }
      delay(40);
    }

    lastSwitchState = switchState;
  }


  sw_value = digitalRead(sw180);
  Serial.println(sw_value);
  if (sw_value == LOW) {
    delay(500);
    sw_counter++;
    if (sw_counter == 2) {
      sw_led_state = !sw_led_state;
      sw_counter = 0;
    }
    if (sw_led_state) {
      digitalWrite(sw_led, HIGH);
    } else {
      digitalWrite(sw_led, LOW);
    }
  }

  if (Serial3.available() > 0) {

    char firstChar = Serial3.read();
    if (firstChar == '!') {
      Serial.println("PIR_toggle");
      pir_work = !pir_work;
    }
    if (firstChar == '~') {
      Serial.println("fan_toggle");
      fan_auto = !fan_auto;
    }
  }
}
