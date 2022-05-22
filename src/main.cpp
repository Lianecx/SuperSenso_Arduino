#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int buzz = 12;
const int backlight = 2;

const float freqGreen = 261.626;
const float freqYel = 329.628;
const float freqRed = 391.995;
const float freqBlue = 523.251;

const int ledGreen = 8;
const int ledYel = 9;
const int ledRed = 10;
const int ledBlue = 11;

const int butGreen = 4;
const int butYel = 5;
const int butRed = 6;
const int butBlue = 7;

int butGreenStatus;
int butYelStatus;
int butRedStatus;
int butBlueStatus;

int userLights[32];
int randLights[32];
int count = 2;

boolean randSequence = false;
boolean onlySound = false;
boolean win = true;

String mode;
const int SPEED = 300;

Adafruit_SSD1306 oled(128, 64, &Wire, 4);

void tone(int ledPin) {
    switch(ledPin) {
        case 8:
            tone(buzz, freqGreen);
            break;
        case 9:
            tone(buzz, freqYel);
            break;
        case 10:
            tone(buzz, freqRed);
            break;
        case 11:
            tone(buzz, freqBlue);
            break;
    }
}

void startGame() {
    for(int i=8; i<13; i++) {
        digitalWrite(i, HIGH);
        tone(i);
        delay(50);
    }
    noTone(buzz);

    oled.clearDisplay();
    oled.setTextSize(2);
    oled.println("SuperSenso\n");

    oled.setTextSize(1);
    oled.println("Choose difficulty:\n");
    oled.print("Easy Medium Hard");
    oled.display();

    do {
        butGreenStatus = digitalRead(butGreen);
        butYelStatus = digitalRead(butYel);
        butRedStatus = digitalRead(butRed);
        butBlueStatus = digitalRead(butBlue);

        if (butGreenStatus == 0 && butYelStatus == 0) {
            mode = "EASY";
            randLights[0] = random(8, 12);
        } else if (butRedStatus == 0 && butBlueStatus == 0) {
            onlySound = true;
            mode = "HARD";
            randLights[0] = random(8, 12);
        } else if(butYelStatus == 0 && butRedStatus == 00) {
            randSequence = true;
            mode = "MEDIUM";
        }
    } while((butGreenStatus == 1 || butYelStatus == 1) && (butRedStatus == 1 || butBlueStatus == 1) && (butYelStatus == 1 || butRedStatus == 1));

    oled.clearDisplay();
    oled.print(mode + " MODE");
    oled.display();

    delay(300);
    for(int i=12; i>7; i--) {
        digitalWrite(i, LOW);
        tone(i);
        delay(50);
    }
    noTone(buzz);
    delay(1000);

    if(onlySound) {
        for(int i=8; i<13; i++) {
            digitalWrite(i, HIGH);
            tone(i);
            delay(1000);
            digitalWrite(i, LOW);
        }
        noTone(buzz);
        delay(1000);
    }
}

void end(int correctPin) {
    onlySound = false;
    randSequence = false;

    oled.clearDisplay();
    oled.println("YOU LOST IN ROUND " + String(count - 1));
    oled.display();

    count = 2;

    for(int i=0; i<10; i++) {
        tone(correctPin);
        if(!onlySound) digitalWrite(correctPin, HIGH);
        delay(100);
        if(!onlySound) digitalWrite(correctPin, LOW);
        noTone(buzz);
        delay(100);
    }

    delay(1000);
    startGame();
}
void next() {
    oled.clearDisplay();

    if(count >= 31) {
        oled.println("You beat " + mode + " MODE!");
        oled.display();

        delay(5000);
        startGame();
    } else {
        oled.println("YOU WON ROUND " + String(count - 1));
        oled.display();

        count++;
        delay(1000);
    }

}

void setup() {
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oled.setTextColor(WHITE);
    oled.setTextSize(1);

    randomSeed(analogRead(A0));

    pinMode(buzz, OUTPUT);

    pinMode(ledGreen, OUTPUT);
    pinMode(ledYel, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledBlue, OUTPUT);

    pinMode(butGreen, INPUT_PULLUP);
    pinMode(butYel, INPUT_PULLUP);
    pinMode(butRed, INPUT_PULLUP);
    pinMode(butBlue, INPUT_PULLUP);

    for(int i=8; i<13; i++) {
        tone(i);
        digitalWrite(i, HIGH);
        delay(500);
    }
    noTone(buzz);
    for(int i=8; i<13; i++) digitalWrite(i, LOW);

    delay(1000);
    startGame();
}

void loop() {
    if(!randSequence) randLights[count-1] = random(8, 12);
    for(int i=0; i<count; i++) {
        if(randSequence) randLights[i] = random(8, 12);

        tone(randLights[i]);
        if(!onlySound) digitalWrite(randLights[i], HIGH);
        delay(SPEED);
        noTone(buzz);
        if(!onlySound) digitalWrite(randLights[i], LOW);
        delay(200);
    }

    for(int i=0; i<count; i++) {
        do {
            butGreenStatus = digitalRead(butGreen);
            butYelStatus = digitalRead(butYel);
            butRedStatus = digitalRead(butRed);
            butBlueStatus = digitalRead(butBlue);

            if (butGreenStatus == 0) userLights[i] = ledGreen;
            else if (butYelStatus == 0) userLights[i] = ledYel;
            else if (butRedStatus == 0) userLights[i] = ledRed;
            else if (butBlueStatus == 0) userLights[i] = ledBlue;

        } while (butGreenStatus == 1 && butYelStatus == 1 && butRedStatus == 1 && butBlueStatus == 1);

        tone(userLights[i]);
        if(!onlySound) digitalWrite(userLights[i], HIGH);
        delay(SPEED);
        noTone(buzz);
        if(!onlySound) digitalWrite(userLights[i], LOW);

        if (userLights[i] != randLights[i]) {
            end(randLights[i]);
            win = false;
            if(onlySound) {
                digitalWrite(ledYel, HIGH);
                digitalWrite(ledRed, HIGH);
            }
            return;
        } else win = true;
    }

    if(win) next();
}