#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

Servo myservo;

// LCD: RS->A0, E->A1, D4->A2, D5->A3, D6->A4, D7->A5
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Keypad
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1','2','3','A' },
  { '4','5','6','B' },
  { '7','8','9','C' },
  { '*','0','#','D' }
};

// Avoid pins 0/1 (Serial). Wire keypad pin order R1,R2,R3,R4,C1,C2,C3,C4
// to Arduino pins 2,3,4,5,6,7,8,9 respectively.
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const byte SERVO_PIN = 10;          // matches the green wire in your circuit
const char Master[] = "4573";       // password
const byte PASS_LEN = sizeof(Master) - 1;

char Data[PASS_LEN + 1];            // +1 for the terminating '\0'
byte data_count = 0;

void showPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void clearData() {
  memset(Data, 0, sizeof(Data));
  data_count = 0;
}

void ServoClose() {
  for (int pos = 90; pos >= 0; pos -= 10) {
    myservo.write(pos);
    delay(30);
  }
}

void ServoOpen() {
  for (int pos = 0; pos <= 90; pos += 10) {
    myservo.write(pos);
    delay(30);
  }
}

void setup() {
  myservo.attach(SERVO_PIN);
  ServoClose();

  lcd.begin(16, 2);
  lcd.print("Door is locked");
  delay(2000);
  lcd.clear();
  lcd.print("Loading...");
  delay(2000);

  clearData();
  showPrompt();
}

void loop() {
  char key = customKeypad.getKey();
  if (!key) return;

  // '*' clears the current entry
  if (key == '*') {
    clearData();
    showPrompt();
    return;
  }

  if (data_count < PASS_LEN) {
    Data[data_count] = key;
    lcd.setCursor(data_count, 1);
    lcd.print('*');                 // hide the digit (use 'key' to show it)
    data_count++;
    Data[data_count] = '\0';
  }

  if (data_count == PASS_LEN) {
    delay(300);                     // let the last '*' be seen
    lcd.clear();

    if (strcmp(Data, Master) == 0) {
      lcd.print("Opening Door...");
      ServoOpen();
      lcd.setCursor(0, 1);
      lcd.print("Opened!");
      delay(2000);

      lcd.clear();
      lcd.print("Waiting...");
      delay(3000);

      lcd.clear();
      lcd.print("Closing Door...");
      ServoClose();
      lcd.setCursor(0, 1);
      lcd.print("Closed!");
      delay(2000);
    } else {
      lcd.print("ERROR!");
      lcd.setCursor(0, 1);
      lcd.print("Wrong password");
      delay(2000);
    }

    clearData();
    showPrompt();
  }
}
