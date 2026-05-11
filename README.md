# ⚙️ Arduino Servo & Stepper Motor Kontrolü — HW2

Arduino Uno R4 ile potansiyometre kontrollü servo motor ve A4988 sürücüsü ile buton kontrollü stepper motor uygulamaları. Wokwi simülatöründe test edilmiş, gerçek donanımda da çalıştırılmıştır.

## 📸 Simülatör Görüntüleri

| Servo Kontrolü | Stepper + Buton |
|----------------|-----------------|
| ![Servo](Servo/HW2-Servo%20(1).png) | ![Stepper](Stepper%20with%20Buttons/HW2-%20Stepper_Motor_with_Buttons%20(1).png) |

## 📁 Proje Yapısı

```
├── Servo/
│   ├── sketch.ino
│   ├── HW2-Servo (1).png
│   ├── HW2-Servo (3).png
│   └── HW2-Servo.mp4
│
├── Stepper/
│   ├── stepper_basic/stepper_basic.ino
│   ├── HW2- Stepper_Motor (1-4).png
│   └── HW2-Stepper_Motor.mp4
│
└── Stepper with Buttons/
    ├── stepper_control/stepper_control.ino
    ├── HW2- Stepper_Motor_with_Buttons (1-4).png
    └── HW2-Stepper_Motor_Buton.mp4
```

## 🔵 Bölüm 1 — Servo Motor (Feed-Forward Kontrol)

Potansiyometre değeri ADC ile okunarak 0–180° servo açısına dönüştürülür.

### Donanım

| Bileşen | Detay |
|---------|-------|
| Mikrodenetleyici | Arduino Uno R4 |
| Sensör | Potansiyometre (A2) |
| Aktüatör | SG90 Servo Motor (Pin 9) |
| Simülatör | Wokwi |

### Çalışma Mantığı

1. `analogRead(A2)` → 0–1023 ham değer okur
2. `map(value, 0, 1023, 0, 180)` → 0°–180° açıya dönüştürür
3. `myServo.write(angle)` → Servo motoru konumlandırır
4. Serial Monitor'da `Input_Raw` ve `Servo_Angle` telemetri çıktısı

### Kod

```cpp
#include <Servo.h>

const int PIN_SENSOR = A2;
const int PIN_SERVO  = 9;
Servo myServo;

void setup() {
  pinMode(PIN_SENSOR, INPUT);
  myServo.attach(PIN_SERVO);
  Serial.begin(9600);
}

void loop() {
  int raw   = analogRead(PIN_SENSOR);
  int angle = map(raw, 0, 1023, 0, 180);
  myServo.write(angle);
  Serial.print("Input_Raw:"); Serial.print(raw);
  Serial.print(",Servo_Angle:"); Serial.println(angle);
  delay(20);
}
```

---

## 🔴 Bölüm 2 — Stepper Motor (Temel)

A4988 sürücüsü ile STEP/DIR pinleri üzerinden stepper motor kontrolü. Her tur 200 adım, yön otomatik değişir.

### Donanım

| Bileşen | Detay |
|---------|-------|
| Mikrodenetleyici | Arduino Uno R4 |
| Sürücü | A4988 |
| STEP Pin | D3 |
| DIR Pin | D2 |
| Adım/Tur | 200 (1.8°/adım) |
| Hız | 800 µs gecikme |

### Çalışma Mantığı

1. `STEP` pinine HIGH/LOW pulse gönderilir
2. Her 200 adımda bir tur tamamlanır
3. 500ms beklenip yön otomatik tersine çevrilir

### Kod

```cpp
#define STEP_PIN 3
#define DIR_PIN  2

const unsigned int  STEP_DELAY_US = 800;
const unsigned long STEPS_PER_REV = 200;

void step() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(STEP_DELAY_US);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(STEP_DELAY_US);
}

void loop() {
  for (unsigned long i = 0; i < STEPS_PER_REV; i++) step();
  delay(500);
  digitalWrite(DIR_PIN, !digitalRead(DIR_PIN));
}
```

---

## 🟢 Bölüm 3 — Stepper Motor + 4 Buton

4 renkli buton ile stepper motoru ileri/geri/dur/başlat kontrolü. Debounce mekanizması ve non-blocking adım üretimi içerir.

### Donanım

| Bileşen | Detay |
|---------|-------|
| Mikrodenetleyici | Arduino Uno R4 |
| Sürücü | A4988 |
| Buton Sayısı | 4 (INPUT_PULLUP) |

### Buton Haritası

| Pin | Renk | İşlev |
|-----|------|-------|
| D13 | 🔵 Mavi | İleri |
| D12 | 🔴 Kırmızı | Geri |
| D11 | 🟡 Sarı | Durdur |
| D10 | 🟢 Yeşil | Başlat |

### Çalışma Mantığı

1. `INPUT_PULLUP` → buton basılı değilken HIGH, basılıyken LOW
2. 50ms debounce ile yanlış tetikleme önlenir
3. Non-blocking `micros()` ile motor çalışırken butonlar dinlenir
4. Son yön hatırlanır, Başlat butonu aynı yönde devam ettirir

### Kod

```cpp
#define STEP_PIN     3
#define DIR_PIN      2
#define BTN_FORWARD  13
#define BTN_BACKWARD 12
#define BTN_STOP     11
#define BTN_START    10

bool motorRunning = false;
bool dirForward   = true;

void loop() {
  if (buttonPressed(BTN_FORWARD,  lastPressBtn1)) { dirForward = true;  motorRunning = true; }
  if (buttonPressed(BTN_BACKWARD, lastPressBtn2)) { dirForward = false; motorRunning = true; }
  if (buttonPressed(BTN_STOP,     lastPressBtn3)) { motorRunning = false; }
  if (buttonPressed(BTN_START,    lastPressBtn4)) { motorRunning = true; }

  if (motorRunning) {
    unsigned long now = micros();
    if (now - lastStepTime >= STEP_DELAY) {
      lastStepTime = now;
      stepState = !stepState;
      digitalWrite(STEP_PIN, stepState ? HIGH : LOW);
    }
  }
}
```

---

## 🛠️ Kullanılan Kütüphaneler

- `Servo.h` — Servo motor PWM kontrolü
- Stepper için harici kütüphane gerekmez (STEP/DIR manuel kontrol)

## 🔬 Simülatör

Tüm devreler **Wokwi** online simülatöründe test edilmiştir.

## 📚 Ders Bilgisi
**EE304 — Embedded Systems  |  HW2**

## 👩‍💻 Geliştirici
**Beyza Erdem** — 2211051049
