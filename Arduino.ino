// Pin definitions for traffic lights
// Lane 1
const int red1 = 4;
const int yellow1 = 3;
const int green1 = 2;

// Lane 2
const int red2 = 7;
const int yellow2 = 6;
const int green2 = 5;

// Lane 3
const int red3 = 10;
const int yellow3 = 9;
const int green3 = 8;

// Lane 4
const int red4 = 13;
const int yellow4 = 12;
const int green4 = 11;

// Sensor input pins (A0, A1, A2, A3)
const int lane1Sensor = A0;
const int lane2Sensor = A1;
const int lane3Sensor = A2;
const int lane4Sensor = A3;

// Time durations (in milliseconds)
const int greenTime = 1000;   // Normal green light duration (10 seconds)
const int yellowTime = 2000;   // Normal yellow light duration (2 seconds)
const int redTime = 3000;      // Red light duration before emergency (3 seconds)

void setup() {
  // Set all traffic light pins as outputs
  pinMode(red1, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(green1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(red3, OUTPUT);
  pinMode(yellow3, OUTPUT);
  pinMode(green3, OUTPUT);
  pinMode(red4, OUTPUT);
  pinMode(yellow4, OUTPUT);
  pinMode(green4, OUTPUT);

  // Set sensor pins as inputs
  pinMode(lane1Sensor, INPUT);
  pinMode(lane2Sensor, INPUT);
  pinMode(lane3Sensor, INPUT);
  pinMode(lane4Sensor, INPUT);

  // Initialize all lights to red
  initializeLights();
}

void loop() {
  // Normal traffic cycle with checks for emergency signals after each yellow light
  normalCycle();
  checkForEmergency();
}

// Function to set all lanes to red initially
void initializeLights() {
  digitalWrite(red1, HIGH);
  digitalWrite(yellow1, LOW);
  digitalWrite(green1, LOW);

  digitalWrite(red2, HIGH);
  digitalWrite(yellow2, LOW);
  digitalWrite(green2, LOW);

  digitalWrite(red3, HIGH);
  digitalWrite(yellow3, LOW);
  digitalWrite(green3, LOW);

  digitalWrite(red4, HIGH);
  digitalWrite(yellow4, LOW);
  digitalWrite(green4, LOW);
}


// Function to handle emergency green light
void emergencyGreen(int lane) {
  // Turn all lanes to red
  initializeLights();

  // Wait for 3 seconds (turning all lights red for 3 seconds)
  delay(redTime);

  // Activate green for the emergency lane
  switch (lane) {
    case 1:
      digitalWrite(red1, LOW);
      digitalWrite(green1, HIGH);
      break;
    case 2:
      digitalWrite(red2, LOW);
      digitalWrite(green2, HIGH);
      break;
    case 3:
      digitalWrite(red3, LOW);
      digitalWrite(green3, HIGH);
      break;
    case 4:
      digitalWrite(red4, LOW);
      digitalWrite(green4, HIGH);
      break;
  }

  // Keep the emergency lane green for 10 seconds
  delay(greenTime);

  // Return to normal traffic cycle after emergency
  normalCycle();
}

// Function to handle the normal traffic light timings
void normalCycle() {
  // Lane 1 green, others red
  laneGreen(1);
  delay(greenTime);
  laneYellow(1);
  delay(yellowTime);
  if (checkForEmergency()) return;  // Check for emergency after yellow

  // Lane 2 green, others red
  laneGreen(2);
  delay(greenTime);
  laneYellow(2);
  delay(yellowTime);
  if (checkForEmergency()) return;  // Check for emergency after yellow

  // Lane 3 green, others red
  laneGreen(3);
  delay(greenTime);
  laneYellow(3);
  delay(yellowTime);
  if (checkForEmergency()) return;  // Check for emergency after yellow

  // Lane 4 green, others red
  laneGreen(4);
  delay(greenTime);
  laneYellow(4);
  delay(yellowTime);
  if (checkForEmergency()) return;  // Check for emergency after yellow
}

// Function to set one lane to green, others to red
void laneGreen(int lane) {
  // Set all lanes to red
  initializeLights();

  // Activate green for the specified lane
  switch (lane) {
    case 1:
      digitalWrite(red1, LOW);
      digitalWrite(green1, HIGH);
      break;
    case 2:
      digitalWrite(red2, LOW);
      digitalWrite(green2, HIGH);
      break;
    case 3:
      digitalWrite(red3, LOW);
      digitalWrite(green3, HIGH);
      break;
    case 4:
      digitalWrite(red4, LOW);
      digitalWrite(green4, HIGH);
      break;
  }
}

// Function to set one lane to yellow, others to red
void laneYellow(int lane) {
  // Set all lanes to red
  initializeLights();

  // Activate yellow for the specified lane
  switch (lane) {
    case 1:
      digitalWrite(red1, LOW);
      digitalWrite(yellow1, HIGH);
      break;
    case 2:
      digitalWrite(red2, LOW);
      digitalWrite(yellow2, HIGH);
      break;
    case 3:
      digitalWrite(red3, LOW);
      digitalWrite(yellow3, HIGH);
      break;
    case 4:
      digitalWrite(red4, LOW);
      digitalWrite(yellow4, HIGH);
      break;
  }
}

// Function to check for emergency vehicles (interrupt if any sensor is HIGH)
bool checkForEmergency() {
  if (digitalRead(lane1Sensor) == HIGH) {
    emergencyGreen(1);
    return true;
  } else if (digitalRead(lane2Sensor) == HIGH) {
    emergencyGreen(2);
    return true;
  } else if (digitalRead(lane3Sensor) == HIGH) {
    emergencyGreen(3);
    return true;
  } else if (digitalRead(lane4Sensor) == HIGH) {
    emergencyGreen(4);
    return true;
  }
  return false;
}
