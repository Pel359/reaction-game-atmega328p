#include <Arduino.h>

#define LED1_1 8   // Первый индикатор  P1 (PB0)
#define LED1_2 11  // Второй индикатор  P1 (PB3)
#define LED1_3 12  // Третий индикатор  P1 (PB4)
#define LED2   10  // Центр (PB2)
#define LED2_1 9   // Первый индикатор  P2 (PB1)
#define LED2_2 13  // Второй индикатор  P2 (PB5)
#define LED2_3 7   // Третий индикатор  P2 (PD7)
#define BUTTON1 2  // Кнопка P1
#define BUTTON2 3  // Кнопка P2

int scoreP1 = 0;
int scoreP2 = 0;

// Обновляем индикаторы счёта
void updateScoreLEDsP1() {
  digitalWrite(LED1_1, scoreP1 >= 1 ? HIGH : LOW);
  digitalWrite(LED1_2, scoreP1 >= 2 ? HIGH : LOW);
  digitalWrite(LED1_3, scoreP1 >= 3 ? HIGH : LOW);
}

void updateScoreLEDsP2() {
  digitalWrite(LED2_1, scoreP2 >= 1 ? HIGH : LOW);
  digitalWrite(LED2_2, scoreP2 >= 2 ? HIGH : LOW);
  digitalWrite(LED2_3, scoreP2 >= 3 ? HIGH : LOW);
}

// Анимация победы в матче (поочерёдное мигание всех трёх индикаторов 5 раз)
void victoryAnimationP1() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED1_1, HIGH); delay(200); digitalWrite(LED1_1, LOW);
    digitalWrite(LED1_2, HIGH); delay(200); digitalWrite(LED1_2, LOW);
    digitalWrite(LED1_3, HIGH); delay(200); digitalWrite(LED1_3, LOW);
  }
}

void victoryAnimationP2() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED2_1, HIGH); delay(200); digitalWrite(LED2_1, LOW);
    digitalWrite(LED2_2, HIGH); delay(200); digitalWrite(LED2_2, LOW);
    digitalWrite(LED2_3, HIGH); delay(200); digitalWrite(LED2_3, LOW);
  }
}

void normalRound() {
  // Рандомная задержка перед стартом
  unsigned long waitDuration = random(1000, 5001);
  unsigned long waitStart = millis();

  int cheatCountP1 = 0;
  int cheatCountP2 = 0;
  bool blockedP1 = false;
  bool blockedP2 = false;

  byte prevStateB1 = HIGH;
  byte prevStateB2 = HIGH;

  // Фаза ожидания (анти-спам)
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

  // Старт раунда — зажигаем центр
  digitalWrite(LED2, HIGH);

  unsigned long reactionStart = millis();
  bool pressed = false;

  // Фаза реакции
  while (millis() - reactionStart < 5000 && !pressed) {
    if (!blockedP1 && digitalRead(BUTTON1) == LOW) {
      scoreP1++;
      updateScoreLEDsP1();   // Зажигаем следующий индикатор (и он остаётся гореть)
      digitalWrite(LED2, LOW);
      pressed = true;
    } else if (!blockedP2 && digitalRead(BUTTON2) == LOW) {
      scoreP2++;
      updateScoreLEDsP2();   // Зажигаем следующий индикатор (и он остаётся гореть)
      digitalWrite(LED2, LOW);
      pressed = true;
    }
  }

  if (!pressed) {
    digitalWrite(LED2, LOW);  
  }
}

void setup() {
  pinMode(LED1_1, OUTPUT);
  pinMode(LED1_2, OUTPUT);
  pinMode(LED1_3, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED2_1, OUTPUT);
  pinMode(LED2_2, OUTPUT);
  pinMode(LED2_3, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  // Стартовый сигнал: 3 моргания центра
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED2, HIGH);
    delay(500);
    digitalWrite(LED2, LOW);
    delay(500);
  }

  // На старте счёт 0 — все индикаторы погашены
  updateScoreLEDsP1();
  updateScoreLEDsP2();
}

void loop() {
  // Перед каждым раундом обновляем индикаторы (чтобы они продолжали гореть)
  updateScoreLEDsP1();
  updateScoreLEDsP2();
  digitalWrite(LED2, LOW);  

  normalRound();
// Проверка победы в матче (до 3 очков)
  if (scoreP1 >= 3) {
    victoryAnimationP1();  // Анимация победы
    scoreP1 = 0;
    scoreP2 = 0;
    // Гасим все индикаторы для нового матча
    digitalWrite(LED1_1, LOW);
    digitalWrite(LED1_2, LOW);
    digitalWrite(LED1_3, LOW);
    digitalWrite(LED2_1, LOW);
    digitalWrite(LED2_2, LOW);
    digitalWrite(LED2_3, LOW);
    delay(2000);
  } else if (scoreP2 >= 3) {
    victoryAnimationP2();
    scoreP1 = 0;
    scoreP2 = 0;
    digitalWrite(LED1_1, LOW);
    digitalWrite(LED1_2, LOW);
    digitalWrite(LED1_3, LOW);
    digitalWrite(LED2_1, LOW);
    digitalWrite(LED2_2, LOW);
    digitalWrite(LED2_3, LOW);
    delay(2000);
  }

  delay(1000);  // Пауза перед следующим раундом
}
