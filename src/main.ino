#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AnimationBitmaps.cpp"

//CONSTANTS
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

const int SPEED = 300;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

//VARIABLES
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

//CONSTRUCTOR
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);


//ANIMATIONS
const unsigned char* start_allArray[28] = {
        start_frame_00_delay_0,
        start_frame_01_delay_0,
        start_frame_02_delay_0,
        start_frame_03_delay_0,
        start_frame_04_delay_0,
        start_frame_05_delay_0,
        start_frame_06_delay_0,
        start_frame_07_delay_0,
        start_frame_08_delay_0,
        start_frame_09_delay_0,
        start_frame_10_delay_0,
        start_frame_11_delay_0,
        start_frame_12_delay_0,
        start_frame_13_delay_0,
        start_frame_14_delay_0,
        start_frame_15_delay_0,
        start_frame_16_delay_0,
        start_frame_17_delay_0,
        start_frame_18_delay_0,
        start_frame_19_delay_0,
        start_frame_20_delay_0,
        start_frame_21_delay_0,
        start_frame_22_delay_0,
        start_frame_23_delay_0,
        start_frame_24_delay_0,
        start_frame_25_delay_0,
        start_frame_26_delay_0,
        start_frame_27_delay_0
};


//HELPER FUNCTIONS
void printCentered(const String &string) {
    int16_t x1, y1;
    uint16_t w, h;
    oled.getTextBounds(string, oled.getCursorX(), oled.getCursorY(), &x1, &y1, &w, &h); //calc width of new string
    oled.setCursor(SCREEN_WIDTH/2 - w/2, oled.getCursorY()); //Subtract half the string's width from half the screen width

    oled.print(string);
}
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


//MAIN GAME
void startGame() {
    for(int i=8; i<13; i++) {
        digitalWrite(i, HIGH);
        tone(i);
        delay(50);
    }
    noTone(buzz);

    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    printCentered("SuperSenso\n\n");

    oled.setTextSize(1);
    printCentered("Choose difficulty:\n\n");
    printCentered("Easy  Medium  Hard");
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


    for (int i = 0; i < 3; ++i) {
        for(auto &frame : start_allArray) {
            oled.clearDisplay();
            oled.drawBitmap(SCREEN_WIDTH/3, SCREEN_HEIGHT/5, frame, 48, 48, SSD1306_WHITE);
            oled.display();
            delay(0);
        }
    }


    oled.clearDisplay();
    oled.setTextSize(3);
    oled.setCursor(0, SCREEN_HEIGHT/4);
    printCentered(mode + "\n");
    printCentered("MODE");
    oled.display();

    delay(300);
    for(int i=12; i>7; i--) {
        digitalWrite(i, LOW);
        tone(i);
        delay(50);
    }
    noTone(buzz);

    //Animation for only sound
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
    oled.setTextSize(2);
    oled.setCursor(0, SCREEN_HEIGHT/3);
    printCentered("YOU LOST\n");
    printCentered("IN ROUND " + String(count-1));
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
        oled.setTextSize(3);
        oled.setCursor(0, SCREEN_HEIGHT/4);
        printCentered("YOU BEAT\n");
        printCentered(mode + "\n");
        printCentered("MODE!");
        oled.display();

        delay(5000);
        startGame();
    } else {
        oled.setTextSize(2);
        oled.setCursor(0, SCREEN_HEIGHT/3);
        printCentered("YOU WON\n");
        printCentered("ROUND " + String(count - 1));
        oled.display();

        count++;
        delay(1000);
    }

}

void setup() {
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oled.setTextColor(WHITE);

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
