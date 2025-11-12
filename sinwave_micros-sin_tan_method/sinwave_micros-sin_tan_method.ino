#include <math.h>

// =======================================================
// 1. **قِيَم التَّعْديل السَّهْل (EASY CONFIGURATION)** ⚙️
// =======================================================

// --- تعيين الأطراف المستخدمة في تسلسل التشغيل ---
const int switchA_PIN = 9;  // الطرف (PIN) الذي سيتم تشغيل الموجة عليه أولاً (الموجة الصفراء في راسم الذبذبات)
const int switchB_PIN = 10; // الطرف (PIN) الذي سيتم تشغيل الموجة عليه ثانياً (الموجة الزرقاء في راسم الذبذبات)

// --- توقيتات التسلسل الزمني (بالميكروثانية: 1 ثانية = 1,000,000 ميكروثانية) ---
const unsigned long switchA_ON_TIME_us  = 9500; // مدة تشغيل الموجة A
const unsigned long switchA_OFF_TIME_us = 500;  // مدة الإيقاف الفاصلة بين A و B
const unsigned long switchB_ON_TIME_us  = 9500; // مدة تشغيل الموجة B
const unsigned long switchB_OFF_TIME_us = 500;  // مدة الإيقاف الأخيرة قبل التكرار

// --- قِيَم موجة Sin/Tan (التردد) ---
const int SINE_LUT_SIZE = 128;           // حجم جدول البحث (لا ينصح بتغييره)
const int STEP_INTEGER = 80;            // خطوة التقدم في الجدول (تؤثر على تردد الموجة)
const unsigned long WAVE_UPDATE_INTERVAL_us = 32; // الفاصل الزمني لتحديث الموجة (يحدد تردد الموجة، 32us تقريباً 31.25kHz)


// =======================================================
// 2. التعريفات والمتغيرات الداخلية
// =======================================================

const float PI_VAL = 3.14159265;

// متغييرات حالة التسلسل
unsigned long previousMicros = 0;
int currentState = 0; // 0, 1, 2, 3, 4

// جداول البحث وقيم المؤشر
int SINE_LUT[SINE_LUT_SIZE];
int TAN_LUT[SINE_LUT_SIZE];
volatile int angleIndex_A = 0;
volatile int angleIndex_B = 0;
unsigned long lastWaveUpdateMicros = 0;

// =======================================================
// 3. دالة ملء جداول البحث
// =======================================================

void fill_lookup_tables() {
    for (int i = 0; i < SINE_LUT_SIZE; i++) {
        float angle = (float)i * 2.0 * PI_VAL / (float)SINE_LUT_SIZE;
        
        // قيم الجيب (Sin)
        float sinValue = sin(angle);
        SINE_LUT[i] = (int)((sinValue + 1.0) * 511.5);
        
        // قيم الظل (Tan)
        float tanValue = tan(angle);
        tanValue = constrain(tanValue, -2.0, 2.0);
        TAN_LUT[i] = (int)((tanValue + 2.0) * 1023.0 / 4.0);
    }
}

// =======================================================
// 4. دالة تحديث الموجة (تنفذ عملية PWM باستخدام الجداول)
// =======================================================

void updateWave(int pin, volatile int& angleIndex) {
    int sineReference = SINE_LUT[angleIndex];
    int tanThreshold = TAN_LUT[angleIndex];

    // هذا هو منطق توليد الموجة نفسه
    if (sineReference > tanThreshold) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }

    // تحديث مؤشر الزاوية
    angleIndex += STEP_INTEGER;

    if (angleIndex >= SINE_LUT_SIZE) {
        angleIndex -= SINE_LUT_SIZE;
    }
}

// =======================================================
// 5. دالة setup
// =======================================================

void setup() {
    pinMode(switchA_PIN, OUTPUT);
    pinMode(switchB_PIN, OUTPUT);

    fill_lookup_tables();
    
    // التأكد من أن الأطراف في وضع الإيقاف عند البدء
    digitalWrite(switchA_PIN, LOW);
    digitalWrite(switchB_PIN, LOW);
}

// =======================================================
// 6. دالة loop (آلة الحالة + تحديث الموجة)
// =======================================================

void loop() {
    unsigned long currentMicros = micros();
    bool waveA_Active = false; // flag لتتبع حالة الموجة A
    bool waveB_Active = false; // flag لتتبع حالة الموجة B

    // 1. آلة الحالة: التحكم في فترة تشغيل/إيقاف الموجة
    switch (currentState) {

        case 0: // الحالة 0: التحضير للبدء
            // نضمن إيقاف كلتا الموجتين قبل البدء
            digitalWrite(switchA_PIN, LOW); 
            digitalWrite(switchB_PIN, LOW); 
            
            angleIndex_A = 0; 
            previousMicros = currentMicros;
            currentState = 1;
            break;

        case 1: // الحالة 1: فترة تشغيل الموجة A (الطرف 9)
            waveA_Active = true; 
            digitalWrite(switchB_PIN, LOW); // **تأكيد إيقاف الموجة B (الطرف 10) بشكل صريح**

            if (currentMicros - previousMicros >= switchA_ON_TIME_us) {
                digitalWrite(switchA_PIN, LOW); // إيقاف الموجة A
                previousMicros = currentMicros;
                currentState = 2;
            }
            break;

        case 2: // الحالة 2: الفترة الفاصلة (كلاهما OFF)
            digitalWrite(switchA_PIN, LOW); // تأكيد إيقاف A
            digitalWrite(switchB_PIN, LOW); // تأكيد إيقاف B
            
            if (currentMicros - previousMicros >= switchA_OFF_TIME_us) {
                angleIndex_B = 0; 
                previousMicros = currentMicros;
                currentState = 3;
            }
            break;

        case 3: // الحالة 3: فترة تشغيل الموجة B (الطرف 10)
            waveB_Active = true;
            digitalWrite(switchA_PIN, LOW); // **تأكيد إيقاف الموجة A (الطرف 9) بشكل صريح**

            if (currentMicros - previousMicros >= switchB_ON_TIME_us) {
                digitalWrite(switchB_PIN, LOW); // إيقاف الموجة B
                previousMicros = currentMicros;
                currentState = 4;
            }
            break;

        case 4: // الحالة 4: انتظار فترة الإيقاف الأخيرة (كلاهما OFF)
            digitalWrite(switchA_PIN, LOW); // تأكيد إيقاف A
            digitalWrite(switchB_PIN, LOW); // تأكيد إيقاف B
            
            if (currentMicros - previousMicros >= switchB_OFF_TIME_us) {
                previousMicros = currentMicros;
                currentState = 0; // العودة للبداية
            }
            break;
    }


    // 2. تحديث الموجة: يتم تشغيل الموجة بتردد ثابت فقط إذا كانت علامتها مرفوعة (ON)
    if (currentMicros - lastWaveUpdateMicros >= WAVE_UPDATE_INTERVAL_us) {
        lastWaveUpdateMicros = currentMicros;

        if (waveA_Active) {
            updateWave(switchA_PIN, angleIndex_A);
        }

        if (waveB_Active) {
            updateWave(switchB_PIN, angleIndex_B);
        }
    }
}
