#include <Servo.h>

// Servo motor
Servo motor;
const int initAngle = 90;
const int slippAngle = 0;

//Photonressitor
const int sensor = A0;
int whenDarkValue = 100;
int lavesteLys = 0;

// Startknapp
const int startKnapp = 13;

//Resetknapp
const int resetKnapp = 12;
int teller = 0;

// Gul diode (registrert paa kuler)
const int gulDiode = 10;

// Gronn diode (lyser naar man starter)
const int gronnDiode = 11;

// Kuler
int antallKuler = 0;
int slupneKuler = 0;

// Start og reset boolean
boolean start = false;
boolean reset = false;

// Tiden
int tid = 0;

// Intervallen (5 min)
int intervall = 200;

void setup() {
  Serial.begin(9600);
  motor.attach(9);
  motor.write(initAngle);
  pinMode(startKnapp, INPUT);
  pinMode(resetKnapp, INPUT);
  pinMode(sensor, INPUT);
  pinMode(gulDiode, OUTPUT);
  pinMode(gronnDiode, OUTPUT);

  // Kaller på kalibrer lys
  kalibrerLys();

}

void loop() {
  // For developer
  Serial.print("Millis: ");
  Serial.println(millis());
  Serial.println(digitalRead(startKnapp));
  Serial.println(analogRead(sensor));
  Serial.print("Tid: ");
  Serial.println(tid);
  Serial.print("When dark value: ");
  Serial.println(whenDarkValue);

  // Sjekker lyssensoren for hver gjennomgang av loopen.
  // Hvis det registreres en kule plusser den paa en kule i antallKuler.
  if (seSensor()) {
    Serial.println("Det er moerkt");
    blinkGulDiode();
    antallKuler++;
  }

  // Sjekker om startknappen trykkes for hver gjennomgang av loopen.
  // Registrerer et trykk naar knappen slippes.
  if (seKnapp(startKnapp)) {
    Serial.println("Startknapp trykket");
    startGronnDiode();
    start = true;
  }

  // Sjekker om resetknappen trykkes for hver gjennomgang av loopen.
  // Registrerer et trykk naar knappen slippes.
  if (seKnapp(resetKnapp)) {
    Serial.println("Resetknapp trykket");
    sluttGronnDiode();
    reset = true;
    tid = 0;
  }

  // Hvis startknappen er trykket legger den til én til tid.
  // Hver gang tid % intervall ikke returnerer en rest slipper den en kule.
  if (start == true) {
    tid++;
    if ((tid % intervall) == 0) {
     slippKule();
   }
  }

  // Hvis enten resetknappen er trykket eller det ikke er flere kuler igjen
  // slukkes den groenne dioden for aa indikere at den er ferdig.
  // Hvis det er flere kuler igjen (etter at feks resetknappen er trykket)
  // skyver den ut de resterende saa man kan bruke den senere.
  // Den setter saa alle variabler til null igjen.
  if (antallKuler == 0 || reset == true) {
      if (antallKuler > 0) {
        while (antallKuler > 0) {
          slippKule();
          delay(100);
        }
      }

     sluttGronnDiode();
     start = false;
     reset = false;
     tid = 0;
  }


  delay(10);


}

// seKnapp registrerer at en knapp er trykket naar den slippes.
// Tar inn en digital knapp
// Returnerer boolean
boolean seKnapp (int knapp) {
  if (digitalRead(knapp) == HIGH) {
    while(digitalRead(knapp) == HIGH) {
    }
   return true;
  }
  else {
    return false;
  }
}


//seSensor returnerer true hvis det blir for morkt (under whenDarkValue).
boolean seSensor () {
  if (analogRead(sensor) <= whenDarkValue) {
    while(analogRead(sensor) <= whenDarkValue) {
    }
    return true;
  }
  else {
    return false;
  }
}

// Blinker den gule dioden
void blinkGulDiode() {
  digitalWrite(gulDiode, HIGH);
  delay(500);
  digitalWrite(gulDiode, LOW);
}

// Starter den gronne dioden
void startGronnDiode() {
  digitalWrite(gronnDiode, HIGH);
}

// Slukker den gronne dioden
void sluttGronnDiode() {
  digitalWrite(gronnDiode, LOW);
}

// Slipper kule ved aa snu paa servoen.
void slippKule() {
  delay(500);
  motor.write(slippAngle);
  delay(500);
  motor.write(initAngle);
  antallKuler--;
}

// Klibrerer lysnivaaet
void kalibrerLys() {

  // Setter forst variablen laveste lys til aa vaere det forste lyssensoren finner.
  lavesteLys = analogRead(sensor);

  // En loop som gaar 10 ganger og sjekker og endrer variablen lavestelys hvis den er lavere enn foer
  // Den blinker ogsaa hver gang for aa indikere at den kalibrerer.
  for (int i = 0; i < 10; i++) {
    digitalWrite(gulDiode, HIGH);
    delay(250);
    digitalWrite(gulDiode, LOW);
    delay(250);
    int nyLys = analogRead(sensor);
    if (nyLys <= lavesteLys) {
      lavesteLys = nyLys;
    }
  }

  // Blinker gul diode raskt 3 ganger for aa indikere at den er ferdig med aa kalibrere.
   digitalWrite(gulDiode, HIGH);
   delay(150);
   digitalWrite(gulDiode, LOW);
   delay(150);
   digitalWrite(gulDiode, HIGH);
   delay(150);
   digitalWrite(gulDiode, LOW);
   delay(150);
   digitalWrite(gulDiode, HIGH);
   delay(150);
   digitalWrite(gulDiode, LOW);
   delay(150);

   // Setter whenDarkValue til aa vaere den laveste registrerte lyset, minus 50 for aa lage en liten margin.
   whenDarkValue = lavesteLys - 100;
  }
