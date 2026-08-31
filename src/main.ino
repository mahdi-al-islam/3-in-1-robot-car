#include <Servo.h>
#include <AFMotor.h>

#define Trig A4
#define Echo A5
#define ServoPin 10

// Servo
const int center = 103;
const int scanAngle = 60;

// Obstacle distances
const int safeFront = 40;
const int safeSide = 80;

// Speeds
const int highSpeed = 200;
const int midSpeed = 150;
const int lowSpeed = 100;

// Timing
const int turnTime = 600;
const int backTime = 500;

Servo servo;

AF_DCMotor M2(2);
AF_DCMotor M3(3);

int distance;
int leftDistance;
int rightDistance;

char value;

// 1 = Obstacle avoidance
// 2 = Bluetooth
// 3 = Voice
int mode = 1;


void setup() {
  Serial.begin(9600);

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);

  servo.attach(ServoPin);
  servo.write(center);

  M2.setSpeed(highSpeed);
  M3.setSpeed(highSpeed);

  stopMotors();

  delay(500);

  Serial.println("Robot Ready");
  Serial.println("1 = Auto");
  Serial.println("2 = Bluetooth");
  Serial.println("3 = Voice");
}


void loop() {

  // Change mode
  if (Serial.available()) {
    value = Serial.read();

    if (value == '1') {
      mode = 1;
      stopMotors();
      servo.write(center);
      Serial.println("Obstacle Avoidance Mode");
    }

    else if (value == '2') {
      mode = 2;
      stopMotors();
      servo.write(center);
      Serial.println("Bluetooth Mode");
    }

    else if (value == '3') {
      mode = 3;
      stopMotors();
      servo.write(center);
      Serial.println("Voice Mode");
    }

    else {
      // Send normal movement command
      if (mode == 2)
        bluetoothCommand(value);

      else if (mode == 3)
        voiceCommand(value);
    }
  }

  // Run selected mode
  if (mode == 1) {
    obstacleAvoidance();
  }
}


// ================= OBSTACLE AVOIDANCE =================

void obstacleAvoidance() {

  distance = getDistance(center);

  Serial.print("Front: ");
  Serial.println(distance);

  if (distance > safeFront) {

    adaptiveSpeed(distance);
    forward();

  } else {

    stopMotors();
    delay(150);

    leftDistance = getDistance(center + scanAngle);
    rightDistance = getDistance(center - scanAngle);

    Serial.print("Left: ");
    Serial.println(leftDistance);

    Serial.print("Right: ");
    Serial.println(rightDistance);

    servo.write(center);
    delay(150);

    if (leftDistance >= safeSide && leftDistance > rightDistance) {

      left();
      delay(turnTime);

    }

    else if (rightDistance >= safeSide) {

      right();
      delay(turnTime);

    }

    else {

      // Both sides are too close
      backward();
      delay(backTime);

      // Turn after reversing
      if (leftDistance > rightDistance)
        left();
      else
        right();

      delay(turnTime);
    }

    stopMotors();
  }
}


// ================= BLUETOOTH =================

void bluetoothCommand(char command) {

  if (command == 'F') {

    distance = getDistance(center);

    if (distance > safeFront)
      forward();
    else
      stopMotors();
  }

  else if (command == 'B') {
    backward();
  }

  else if (command == 'L') {
    left();
  }

  else if (command == 'R') {
    right();
  }

  else if (command == 'S') {
    stopMotors();
  }
}


// ================= VOICE =================

void voiceCommand(char command) {

  if (command == 'w') {

    distance = getDistance(center);

    if (distance > safeFront)
      forward();
    else
      stopMotors();
  }

  else if (command == 's') {
    backward();
  }

  else if (command == 'a') {
    left();
  }

  else if (command == 'd') {
    right();
  }

  else if (command == 'x') {
    stopMotors();
  }
}


// ================= MOTOR FUNCTIONS =================

void forward() {
  M2.run(FORWARD);
  M3.run(FORWARD);
}

void backward() {
  M2.run(BACKWARD);
  M3.run(BACKWARD);
}

void left() {
  M2.run(BACKWARD);
  M3.run(FORWARD);
}

void right() {
  M2.run(FORWARD);
  M3.run(BACKWARD);
}

void stopMotors() {
  M2.run(RELEASE);
  M3.run(RELEASE);
}


// ================= ULTRASONIC =================

int getDistance(int angle) {

  servo.write(angle);
  delay(250);

  digitalWrite(Trig, LOW);
  delayMicroseconds(2);

  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(Trig, LOW);

  long duration = pulseIn(Echo, HIGH, 30000);

  if (duration == 0)
    return 400;

  int distance = duration * 0.0343 / 2;

  if (distance > 400)
    distance = 400;

  return distance;
}


// ================= ADAPTIVE SPEED =================

void adaptiveSpeed(int distance) {

  if (distance > 100) {
    M2.setSpeed(highSpeed);
    M3.setSpeed(highSpeed);
  }

  else if (distance > 60) {
    M2.setSpeed(midSpeed);
    M3.setSpeed(midSpeed);
  }

  else {
    M2.setSpeed(lowSpeed);
    M3.setSpeed(lowSpeed);
  }
}
