// =============================================
//  Stepper Motor Control - A4988 (Basic)
// =============================================

#define STEP_PIN    3
#define DIR_PIN     2

// --- Speed & movement settings ---
const unsigned int  STEP_DELAY_US = 800;  // Pulse width in microseconds (lower = faster)
const unsigned long STEPS_PER_REV = 200;  // Standard 1.8° stepper = 200 steps/rev

// --- Direction ---
// HIGH = forward | LOW = backward
const bool DIRECTION = HIGH;

// =============================================
void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);

  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, DIRECTION);

  Serial.begin(9600);
  Serial.println("Stepper Motor Ready.");
  Serial.print("Speed delay: ");
  Serial.print(STEP_DELAY_US);
  Serial.println(" us");
}

// =============================================
void step() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(STEP_DELAY_US);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(STEP_DELAY_US);
}

void loop() {
  // Spin one full revolution forward
  for (unsigned long i = 0; i < STEPS_PER_REV; i++) {
    step();
  }

  delay(500); // Pause between revolutions

  // Reverse direction
  digitalWrite(DIR_PIN, !digitalRead(DIR_PIN));
}
