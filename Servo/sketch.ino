/*
* ============================================================================
* PROJECT: Positional Servo Control (Feed-Forward Loop)
* ENGINEER: [Student Name]
* HARDWARE: Arduino Uno R4/R3 + Servo Motor (SG90)
* ============================================================================
* DESCRIPTION:
* This firmware implements a "feed-forward" control system for positional actuation.
* 1. SENSING: Samples an analog voltage divider (Potentiometer) via the ADC.
* 2. PROCESSING: Linearly interpolates the 10-bit input to an 180-degree output range.
* 3. ACTUATION: Uses the Servo library to move the servo to the desired angle.
*/
// --- 1. LIBRARY INCLUSION ---
// The Servo library provides high-level functions to control the servo motor.
// This abstracts away the low-level PWM pulse timing calculations.
#include <Servo.h>
// --- 2. HARDWARE ABSTRACTION LAYER (HAL) ---
// Define pin constants for the sensor input and actuator output.
const int PIN_SENSOR = A2; // Analog input from the potentiometer.
const int PIN_SERVO = 9; // Digital PWM pin connected to the servo signal wire.
// Create a Servo object instance. This object will represent the physical servo motor.
Servo myServo;
// --- 3. SYSTEM VARIABLES (Global State) ---
int rawSensorValue = 0; // Stores the raw digital representation of voltage (0 to 1023).
int servoAngle = 0; // Stores the calculated angle for the servo (0 to 180 degrees).
// --- 4. INITIALIZATION ROUTINE (void setup) ---
void setup() {
 // --- GPIO Configuration ---
 // Set the Potentiometer pin to High-Impedance Input mode.
 pinMode(PIN_SENSOR, INPUT);
 // Attach the Servo object to the physical PWM pin.
 // This initializes the hardware timer on pin 9 for servo control.
 myServo.attach(PIN_SERVO);
 // --- Telemetry Interface ---
 Serial.begin(9600);
 delay(100); // Wait for Serial to stabilize (Crucial for R4)
 Serial.println(">>> SYSTEM BOOT: Positional Control Active <<<");
}
// --- 5. MAIN CONTROL LOOP (void loop) ---
void loop() {
 // ------------------------------------------------------------------------------------
 // STEP A: DATA ACQUISITION (The Input Layer)
 // ------------------------------------------------------------------------------------
 // Read the analog voltage from the potentiometer (0V maps to 0, 5V maps to 1023).
 rawSensorValue = analogRead(PIN_SENSOR);
 // ------------------------------------------------------------------------------------
 // STEP B: SIGNAL PROCESSING (The Logic Layer)
 // ------------------------------------------------------------------------------------
 // We must convert the input domain (0-1023) to the output domain (0-180 degrees).
 // The map() function performs linear interpolation: y = mx + c.
 // Input Range: [rawSensorValue, 0, 1023] -> Output Range: [0, 180].
 servoAngle = map(rawSensorValue, 0, 1023, 0, 180);
 // ------------------------------------------------------------------------------------
 // STEP C: PHYSICAL ACTUATION (The Output Layer)
 // ------------------------------------------------------------------------------------
 // Command the servo to move to the calculated angle.
 // The Servo library translates 'servoAngle' into a precise PWM pulse width.
 myServo.write(servoAngle);
 // ------------------------------------------------------------------------------------
 // STEP D: TELEMETRY & VISUALIZATION
 // ------------------------------------------------------------------------------------
 // Format: "Label:Value, Label:Value" for Serial Plotter visualization.
 Serial.print("Input_Raw:");
 Serial.print(rawSensorValue);
 Serial.print(",");
 Serial.print("Servo_Angle:");
 Serial.println(servoAngle);
 // ------------------------------------------------------------------------------------
 // STEP E: TIMING CONTROL (Stability)
 // ------------------------------------------------------------------------------------
 // Pause for 20ms to allow the servo to move to position and prevent Serial buffer overflow.
 delay(20);
}
