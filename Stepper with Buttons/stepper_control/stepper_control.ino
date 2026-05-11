// =============================================
//  Stepper Motor Control - A4988 + 4 Buttons
//  Buttons: INPUT_PULLUP (connected to GND, LOW = pressed)
// =============================================

#define STEP_PIN  3
#define DIR_PIN   2

#define BTN_FORWARD  13   // Blue   - move forward
#define BTN_BACKWARD 12   // Red    - move backward
#define BTN_STOP     11   // Yellow - stop motor
#define BTN_START    10   // Green  - restart motor

// --- Speed setting (microseconds) ---
// Lower = faster | Higher = slower
const unsigned int STEP_DELAY = 800;   // ~600 RPM
const unsigned long DEBOUNCE_MS = 50;

// --- State variables ---
bool motorRunning = false;
bool dirForward   = true;

// Last press timestamps for debounce
unsigned long lastPressBtn1 = 0;
unsigned long lastPressBtn2 = 0;
unsigned long lastPressBtn3 = 0;
unsigned long lastPressBtn4 = 0;

// Non-blocking step timing
unsigned long lastStepTime = 0;
bool stepState = false;

// =============================================
void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);

  // INPUT_PULLUP: HIGH when not pressed, LOW when pressed
  pinMode(BTN_FORWARD,  INPUT_PULLUP);
  pinMode(BTN_BACKWARD, INPUT_PULLUP);
  pinMode(BTN_STOP,     INPUT_PULLUP);
  pinMode(BTN_START,    INPUT_PULLUP);

  // Initial direction
  digitalWrite(DIR_PIN, HIGH);

  Serial.begin(9600);
  Serial.println("Stepper Motor Ready.");
}

// =============================================
//  Debounced button read
//  Returns: true = button just pressed (trigger moment)
// =============================================
bool buttonPressed(int pin, unsigned long &lastTime) {
  if (digitalRead(pin) == LOW) {
    unsigned long now = millis();
    if (now - lastTime > DEBOUNCE_MS) {
      lastTime = now;
      return true;
    }
  }
  return false;
}

// =============================================
void loop() {

  // --- Button: Forward ---
  if (buttonPressed(BTN_FORWARD, lastPressBtn1)) {
    dirForward = true;
    digitalWrite(DIR_PIN, HIGH);
    motorRunning = true;
    Serial.println(">> Forward");
  }

  // --- Button: Backward ---
  if (buttonPressed(BTN_BACKWARD, lastPressBtn2)) {
    dirForward = false;
    digitalWrite(DIR_PIN, LOW);
    motorRunning = true;
    Serial.println("<< Backward");
  }

  // --- Button: Stop ---
  if (buttonPressed(BTN_STOP, lastPressBtn3)) {
    motorRunning = false;
    Serial.println("== Stopped");
  }

  // --- Button: Restart (resumes last direction) ---
  if (buttonPressed(BTN_START, lastPressBtn4)) {
    motorRunning = true;
    Serial.print(">> Restarted - Direction: ");
    Serial.println(dirForward ? "Forward" : "Backward");
  }

  // --- Non-blocking step generation ---
  if (motorRunning) {
    unsigned long now = micros();
    if (now - lastStepTime >= STEP_DELAY) {
      lastStepTime = now;
      stepState = !stepState;
      digitalWrite(STEP_PIN, stepState ? HIGH : LOW);
    }
  } else {
    // Keep STEP pin LOW when motor is stopped
    digitalWrite(STEP_PIN, LOW);
    stepState = false;
  }
}
