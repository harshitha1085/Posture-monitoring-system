#include <Wire.h>
#include <MPU6050_light.h>

#define SDA_PIN 4     // GP4
#define SCL_PIN 5     // GP5
#define LED_PIN 16    // GP16

MPU6050 mpu(Wire);

// ------ POSTURE SETTINGS ------
// CHANGE: We now set the calibration target to 180 (Flat)
// The sensor will count DOWN from 180 as you sit up.
const float FLAT_TARGET_ANGLE = 180.0;

// Trigger Alarm if angle drops below 65 degrees
const float SLOUCH_LIMIT = 65.0;  

float angleOffset = 0; 

const unsigned long BAD_TIME = 4000;      // 4 seconds grace period
bool postureBad = false;
unsigned long badStartTime = 0;

// ---------- CALIBRATION ----------
void calibrateReference() {
  Serial.println("\n-----------------------------------------");
  Serial.println("✅ CALIBRATION STARTED");
  Serial.println("👉 KEEP DEVICE FLAT ON TABLE (COMPONENT SIDE UP).");
  Serial.println("👉 We will set this FLAT position to exactly 180.0 degrees.");
  Serial.println("-----------------------------------------\n");
  
  // Flash LED to signal start
  for(int i=0; i<5; i++) {
    digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW); delay(50);
  }
  
  delay(1000); // Wait for user to stop touching it

  float sum = 0;
  for (int i = 0; i < 100; i++) {
    mpu.update();
    sum += mpu.getAngleX();
    delay(5);
  }

  float rawAverage = sum / 100.0;
  
  // MATH: We want the result to be 180.
  // So: 180 = Offset - Raw
  // Therefore: Offset = 180 + Raw
  angleOffset = FLAT_TARGET_ANGLE + rawAverage;

  Serial.print("✅ Raw Sensor Value: ");
  Serial.println(rawAverage);
  Serial.print("✅ Offset Calculated: ");
  Serial.println(angleOffset);
  Serial.println("✅ SYSTEM READY.");
  Serial.println("👉 You may now attach the device to your neck.");
  Serial.println("-----------------------------------------\n");

  digitalWrite(LED_PIN, HIGH); delay(1000); digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(); 

  Serial.println("✅ Raspberry Pi Pico Posture Correction System");

  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("❌ MPU6050 NOT DETECTED! CHECK WIRING.");
    while (1) {
      digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW); delay(50);
    }
  }

  Serial.println("✅ MPU6050 Detected!");
  
  // Fast response filter
  mpu.setFilterGyroCoef(0.02);

  Serial.println("✅ Calculating Gyro Offsets...");
  mpu.calcOffsets();

  // IMPORTANT: Keep flat during this step!
  calibrateReference();
}

void loop() {
  mpu.update();

  // 1. Get Raw Angle
  float rawPitch = mpu.getAngleX();

  // 2. Apply Math: Flat(180) -> Upright(90) -> Slouch(65)
  float currentAngle = angleOffset - rawPitch;

  unsigned long now = millis();

  // --------- POSTURE LOGIC ---------
  // Condition: Is the angle 65 OR LOWER?
  if (currentAngle <= SLOUCH_LIMIT) {
    if (!postureBad) {
      postureBad = true;
      badStartTime = now;
    } 
    else if (now - badStartTime >= BAD_TIME) {
      digitalWrite(LED_PIN, HIGH);  // ALARM ON
    }
  } 
  else {
    // Posture is good (> 65)
    postureBad = false;
    badStartTime = 0; 
    digitalWrite(LED_PIN, LOW);   // ALARM OFF
  }

  // -------- SERIAL OUTPUT --------
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    Serial.print("Angle: ");
    Serial.print(currentAngle, 1); 
    Serial.print("°  |  Limit: ");
    Serial.print(SLOUCH_LIMIT, 0);
    Serial.print("°  |  LED: ");
    Serial.println(digitalRead(LED_PIN) ? "ON (BAD)" : "OFF (GOOD)");
    lastPrint = millis();
  }
}