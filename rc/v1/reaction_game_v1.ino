#include <Arduino.h>
 
#define LED1 8   // PB0 (pin 14) - LED (P1)
#define LED3 9   // PB1 (pin 15) - LED (P2)
#define LED2 10  // PB2 (pin 16) - LED (центр)
#define BUTTON1 2 // PD2 (pin 4, digital 2) - Кнопка P1
#define BUTTON2 3 // PD3 (pin 5, digital 3) - Кнопка P2
 
int scoreP1 = 0;
int scoreP2 = 0;
 
void blinkLED(int led, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(led, HIGH);
    delay(delayMs);
    digitalWrite(led, LOW);
    delay(delayMs);
  }
}
 
void normalRound() {
  // Рандомная задержка 1-5 секунд
  unsigned long waitDuration = random(1000, 5001);
  unsigned long waitStart = millis();
 
  int cheatCountP1 = 0;
  int cheatCountP2 = 0;
  bool blockedP1 = false;
  bool blockedP2 = false;
 
  byte prevStateB1 = HIGH;
  byte prevStateB2 = HIGH;
 
  // Фаза ожидания: мониторим спам кнопок
  while (millis() - waitStart < waitDuration) {
    byte currentB1 = digitalRead(BUTTON1);
    if (currentB1 == LOW && prevStateB1 == HIGH) { 
      cheatCountP1++;
      if (cheatCountP1 > 2) blockedP1 = true;
      while (digitalRead(BUTTON1) == LOW);  
      delay(20); 
    }
    prevStateB1 = currentB1;
 
    byte currentB2 = digitalRead(BUTTON2);
    if (currentB2 == LOW && prevStateB2 == HIGH) {
      cheatCountP2++;
      if (cheatCountP2 > 2) blockedP2 = true;
      while (digitalRead(BUTTON2) == LOW);
      delay(20);
    }
    prevStateB2 = currentB2;
  }
 
  // Зажигаем центральный LED — старт!
  digitalWrite(LED2, HIGH);
 
  unsigned long reactionStart = millis();
  bool pressed = false;
 
  // Фаза реакции (5 секунд)
  while (millis() - reactionStart < 5000 && !pressed) {
    if (!blockedP1 && digitalRead(BUTTON1) == LOW) {
      scoreP1++;
      digitalWrite(LED2, LOW);
      digitalWrite(LED1, HIGH);
      delay(1000);
      digitalWrite(LED1, LOW);
      pressed = true;
    } else if (!blockedP2 && digitalRead(BUTTON2) == LOW) {
      scoreP2++;
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, HIGH);  
      delay(1000);
      digitalWrite(LED3, LOW);
      pressed = true;
    }
  }
 
  if (!pressed) {
    digitalWrite(LED2, LOW);  // Таймаут
  }
}
 
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
 
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
 
  randomSeed(analogRead(A0));  // Seed для рандома
 
  // Стартовый сигнал: моргание центра 3 раза 
  blinkLED(LED2, 3, 500);
}
 
void loop() {
  // Сброс всех LED перед раундом
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
 
  normalRound();
 
  // Проверка победы (до 3 очков)
  if (scoreP1 >= 3) {
    blinkLED(LED1, 5, 100);  // Победа P1 — 5 быстрых миганий 
    scoreP1 = 0;
    scoreP2 = 0;
    delay(2000);  // Пауза перед новым матчем
  } else if (scoreP2 >= 3) {
    blinkLED(LED3, 5, 100);  // Победа P2 — 5 быстрых миганий 
    scoreP1 = 0;
    scoreP2 = 0;
    delay(2000);
  }
 
  // Пауза перед следующим раундом
  delay(1000);
}
