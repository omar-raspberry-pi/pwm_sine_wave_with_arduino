// تعريف أرجل الليدات (يجب أن تكون تدعم PWM - مثل 9 و 10)
const int LED1_PIN = 9;
const int LED2_PIN = 10;

// =========================================================
// 1. تحديد الأزمنة المطلوبة بالمايكرو ثانية (μs)
// =========================================================

// توقيتات الليد الأول (LED 1)
const unsigned long LED1_FADE_UP_TIME_US   = 4800; // 4.8 ثانية: مدة تدرج الإضاءة (0 -> 255)
const unsigned long LED1_FADE_DOWN_TIME_US = 4800; // 4.8 ثانية: مدة تدرج الخفوت (255 -> 0)
const unsigned long LED1_OFF_TIME_US       = 500; // 1.0 ثانية: مدة الإطفاء بعد الخفوت

// توقيتات الليد الثاني (LED 2)
const unsigned long LED2_FADE_UP_TIME_US   = 4800; // 2.0 ثانية: مدة تدرج الإضاءة
const unsigned long LED2_FADE_DOWN_TIME_US = 4800; // 2.0 ثانية: مدة تدرج الخفوت
const unsigned long LED2_OFF_TIME_US       = 500;     // 500 مايكرو ثانية: مدة الإطفاء المطلوبة

// =========================================================
// 2. متغيرات التحكم في الحالة والتوقيت
// =========================================================

int brightness = 0;            // مستوى السطوع الحالي (0-255)
int fadeDirection = 1;         // اتجاه التدرج (1 صعود، -1 نزول)
int currentLED = 0;            // لتحديد الليد النشط حالياً (LED1_PIN أو LED2_PIN)
unsigned long previousMicros = 0; // لتخزين آخر وقت تم فيه تغيير السطوع أو الحالة

// متغير يحمل الفاصل الزمني المحسوب بين كل خطوة سطوع (0-255)
unsigned long currentStepInterval = 0; 

// 6 حالات لتغطية جميع المراحل
// 0: إعداد LED1 صعود | 1: تنفيذ LED1 صعود/خفوت | 2: انتظار إطفاء LED1 
// 3: إعداد LED2 صعود | 4: تنفيذ LED2 صعود/خفوت | 5: انتظار إطفاء LED2 
int currentState = 0; 

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  analogWrite(LED1_PIN, 0); 
  analogWrite(LED2_PIN, 0);
}

void loop() {
  unsigned long currentMicros = micros();

  switch (currentState) {
    
    // ------------------- LED 1 -------------------
    
    case 0: // إعداد LED1 (حساب زمن خطوة الصعود)
      // لحساب الفاصل الزمني: (المدة الإجمالية / 255 خطوة)
      currentStepInterval = LED1_FADE_UP_TIME_US / 255;
      currentLED = LED1_PIN;
      fadeDirection = 1; // صعود
      brightness = 0;
      previousMicros = currentMicros;
      currentState = 1; // الانتقال للتنفيذ
      break; 
      
    case 1: // تنفيذ تدرج LED1 صعوداً وهبوطاً
      if (fadeDirection == 1) { // تدرج صاعد (Fade UP)
        if (currentMicros - previousMicros >= currentStepInterval) {
          previousMicros += currentStepInterval; 
          brightness += fadeDirection;
          analogWrite(currentLED, brightness);
          
          if (brightness >= 255) {
            currentStepInterval = LED1_FADE_DOWN_TIME_US / 255; // حساب زمن خطوة الخفوت
            fadeDirection = -1; // تغيير الاتجاه
            previousMicros = currentMicros; // إعادة ضبط التوقيت لمرحلة الخفوت
          }
        }
      } else { // تدرج هابط (Fade DOWN)
        if (currentMicros - previousMicros >= currentStepInterval) {
          previousMicros += currentStepInterval;
          brightness += fadeDirection;
          analogWrite(currentLED, brightness);
          
          if (brightness <= 0) {
            currentState = 2; // الانتقال لانتظار الإطفاء
            previousMicros = currentMicros; // إعادة ضبط التوقيت لمرحلة الانتظار
          }
        }
      }
      break;
      
    case 2: // انتظار إطفاء LED1 (LED1_OFF_TIME_US)
      if (currentMicros - previousMicros >= LED1_OFF_TIME_US) {
        currentState = 3; // الانتقال لإعداد LED2
      }
      break;
      
    // ------------------- LED 2 -------------------
    
    case 3: // إعداد LED2 (حساب زمن خطوة الصعود)
      currentStepInterval = LED2_FADE_UP_TIME_US / 255;
      currentLED = LED2_PIN;
      fadeDirection = 1; // صعود
      brightness = 0;
      previousMicros = currentMicros;
      currentState = 4; // الانتقال للتنفيذ
      break;
      
    case 4: // تنفيذ تدرج LED2 صعوداً وهبوطاً
      if (fadeDirection == 1) { // تدرج صاعد (Fade UP)
        if (currentMicros - previousMicros >= currentStepInterval) {
          previousMicros += currentStepInterval; 
          brightness += fadeDirection;
          analogWrite(currentLED, brightness);
          
          if (brightness >= 255) {
            currentStepInterval = LED2_FADE_DOWN_TIME_US / 255; // حساب زمن خطوة الخفوت
            fadeDirection = -1; // تغيير الاتجاه
            previousMicros = currentMicros; // إعادة ضبط التوقيت لمرحلة الخفوت
          }
        }
      } else { // تدرج هابط (Fade DOWN)
        if (currentMicros - previousMicros >= currentStepInterval) {
          previousMicros += currentStepInterval;
          brightness += fadeDirection;
          analogWrite(currentLED, brightness);
          
          if (brightness <= 0) {
            currentState = 5; // الانتقال لانتظار الإطفاء
            previousMicros = currentMicros; // إعادة ضبط التوقيت لمرحلة الانتظار
          }
        }
      }
      break;
      
    case 5: // انتظار إطفاء LED2 (LED2_OFF_TIME_US)
      if (currentMicros - previousMicros >= LED2_OFF_TIME_US) {
        currentState = 0; // العودة لدورة LED1
      }
      break;
  }
}
