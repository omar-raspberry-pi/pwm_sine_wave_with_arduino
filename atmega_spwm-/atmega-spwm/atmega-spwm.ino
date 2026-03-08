#include <avr/io.h>
#include <avr/interrupt.h>

// إعدادات التردد 60Hz
const int icr = 800; 
const int samples = 83; 
int LUT[samples];

// متغيرات التحكم
volatile int num = 0;
volatile bool phase = 0;
float percentMod = 0.70; // نسبة التعديل (يمكنك رفعها لـ 0.85 للحصول على 220 فولت)

void setup() {
  // بناء جدول الساين (SPWM Lookup Table)
  for (int i = 0; i < samples; i++) {
    // نضع أصفاراً في أطراف الجدول لضمان الـ Dead Time تلقائياً
    if (i < 8 || i > 92) {
      LUT[i] = 0;
    } else {
      LUT[i] = int(sin(i * PI / samples) * icr * percentMod);
    }
  }

  // إعداد المخارج
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // إعداد Timer1 (Phase Correct PWM)
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10); // Prescaler 1
  ICR1 = icr;

  // تفعيل المقاطعة
  TIMSK1 = _BV(TOIE1);
  sei();
}

ISR(TIMER1_OVF_vect) {
  // التبديل بين الفيزات (نصف الموجة)
  if (num >= samples) {
    num = 0;
    phase = !phase;
  }

  // التحكم بالمخارج
  if (phase == 0) {
    OCR1B = 0;           // إغلاق الجهة الثانية فوراً
    OCR1A = LUT[num];    // تشغيل الجهة الأولى
  } else {
    OCR1A = 0;           // إغلاق الجهة الأولى فوراً
    OCR1B = LUT[num];    // تشغيل الجهة الثانية
  }
  num++;
}

void loop() {
  // المعالج الآن حر تماماً ومستقر
}
