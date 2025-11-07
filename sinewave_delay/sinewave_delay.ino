// تعريف أرجل الليدات (يجب أن تكون تدعم PWM - مثل 9 و 10)
const int LED1_PIN = 9;
const int LED2_PIN = 10;

// =========================================================
// تحديد الأزمنة المطلوبة بالمايكرو ثانية (μs)
// =========================================================

// توقيتات الليد الأول (LED 1)
// 4,800,000 مايكرو ثانية = 4.8 ثانية
const unsigned long LED1_FADE_UP_TIME_US   = 4800; 
const unsigned long LED1_FADE_DOWN_TIME_US = 4800; 
const unsigned long LED1_OFF_TIME_US       = 500; // 1.0 ثانية إطفاء

// توقيتات الليد الثاني (LED 2)
// 2,000,000 مايكرو ثانية = 2.0 ثانية
const unsigned long LED2_FADE_UP_TIME_US   = 4800; 
const unsigned long LED2_FADE_DOWN_TIME_US = 4800; 
const unsigned long LED2_OFF_TIME_US       = 500;     // 500 مايكرو ثانية إطفاء

// دالة لحساب الفاصل الزمني بين كل خطوة PWM (0-255)
// بما أن هناك 255 خطوة، فإن زمن الخطوة = المدة الكلية / 255
unsigned long calculateStepInterval(unsigned long totalTime) {
  // نقسم على 255 بدلاً من 256 لأننا ننتقل من 0 إلى 255 (256 قيمة)
  // لكن التدرج يتم على 255 خطوة
  return totalTime / 255; 
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
}

void loop() {
  // -------------------------
  // 1. دورة الليد الأول (LED 1)
  // -------------------------
  
  // أ. التدرج الصاعد (Fade Up)
  unsigned long stepInterval1_Up = calculateStepInterval(LED1_FADE_UP_TIME_US);
  for (int brightness = 0; brightness <= 255; brightness += 1) {
    analogWrite(LED1_PIN, brightness);
    // توقف كل شيء حتى تنقضي المدة (Blocking)
    delayMicroseconds(stepInterval1_Up); 
  }
  
  // ب. التدرج الهابط (Fade Down)
  unsigned long stepInterval1_Down = calculateStepInterval(LED1_FADE_DOWN_TIME_US);
  for (int brightness = 255; brightness >= 0; brightness -= 1) {
    analogWrite(LED1_PIN, brightness);
    delayMicroseconds(stepInterval1_Down);
  }
  
  // ج. فترة الإطفاء (OFF Time)
  // تأكد من إيقاف الليد الثاني
  analogWrite(LED2_PIN, 0); 
  delayMicroseconds(LED1_OFF_TIME_US);

  // -------------------------
  // 2. دورة الليد الثاني (LED 2)
  // -------------------------
  
  // أ. التدرج الصاعد (Fade Up)
  unsigned long stepInterval2_Up = calculateStepInterval(LED2_FADE_UP_TIME_US);
  for (int brightness = 0; brightness <= 255; brightness += 1) {
    analogWrite(LED2_PIN, brightness);
    delayMicroseconds(stepInterval2_Up);
  }
  
  // ب. التدرج الهابط (Fade Down)
  unsigned long stepInterval2_Down = calculateStepInterval(LED2_FADE_DOWN_TIME_US);
  for (int brightness = 255; brightness >= 0; brightness -= 1) {
    analogWrite(LED2_PIN, brightness);
    delayMicroseconds(stepInterval2_Down);
  }

  // ج. فترة الإطفاء (OFF Time)
  delayMicroseconds(LED2_OFF_TIME_US);
}
