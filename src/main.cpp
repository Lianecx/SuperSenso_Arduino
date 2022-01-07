#include <Arduino.h>

const int buzz = 12;

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
const int SPEED = 300;

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
    Serial.println("Press <GREEN AND YELLOW BUTTON> to start normal game!");
    Serial.println("Press <RED AND BLUE BUTTON> to start game with random sequences!");
    Serial.println("Press <YELLOW AND RED BUTTON> to start game without lights (Very hard!)!");

    do {
        butGreenStatus = digitalRead(butGreen);
        butYelStatus = digitalRead(butYel);
        butRedStatus = digitalRead(butRed);
        butBlueStatus = digitalRead(butBlue);

        if (butGreenStatus == 0 && butYelStatus == 0) randLights[0] = random(8, 12);
        else if (butRedStatus == 0 && butBlueStatus == 0) randSequence = true;
        else if(butYelStatus == 0 && butRedStatus == 00) {
            onlySound = true;
            randLights[0] = random(8, 12);
        }
    } while((butGreenStatus == 1 || butYelStatus == 1) && (butRedStatus == 1 || butBlueStatus == 1) && (butYelStatus == 1 || butRedStatus == 1));

    delay(300);
    for(int i=12; i>7; i--) {
        digitalWrite(i, LOW);
        tone(i);
        delay(50);
    }
    noTone(buzz);
    delay(1000);

    if(onlySound) {
        digitalWrite(ledYel, HIGH);
        digitalWrite(ledRed, HIGH);
        for(int i=8; i<13; i++) {
            tone(i);
            delay(1000);
        }
        noTone(buzz);
        delay(1000);
    }
}

void end(int correctPin) {
    onlySound = false;
    randSequence = false;

    Serial.println("YOU LOST IN ROUND " + String(count-1));
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
    if(count >= 31) Serial.println("YOU CHEATER!!! THERE'S NO WAY YOU ACTUALLY MADE IT TO ROUND 30?!?!\nI think we'll end it here, cuz youre prob gonna break the game.\nYoure just too good ¯\\_(ツ)_/¯");
    Serial.println("YOU WON ROUND " + String(count-1));
    count++;
    delay(1000);
}

void setup() {
    Serial.begin(9600);
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