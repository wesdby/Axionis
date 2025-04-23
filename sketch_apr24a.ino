#include <QTRSensors.h>
// Створення об'єкта датчика лінії
QTRSensors qtr;
// Кількість датчиків, які використовуються
const uint8
t SensorCount = 5;
_
uint16
_
t sensorValues[SensorCount];
// Піни для драйвера моторів
#define LEFT
MOTOR
_
_
#define LEFT
MOTOR
_
_
#define RIGHT
MOTOR
_
_
#define RIGHT
MOTOR
_
#define LEFT
MOTOR
_
_
#define RIGHT
MOTOR
PIN1 2
PIN2 3
PIN1 4
PIN2 5
_
PWM 9
PWM 10
_
_
// Кнопка старту
#define START
BUTTON 7
_
// Змінні для роботи
bool isRunning = false;
int lastError = 0;
int integral = 0;
const int BASE
_
SPEED = 150; // Базова швидкість (0-255)
// Константи для PID-регулятора
float Kp = 0.07; // Пропорційний коефіцієнт
float Ki = 0; // Інтегральний коефіцієнт
float Kd = 1.5; // Диференціальний коефіцієнт
3. Налаштування в функції setup()
void setup() {
qtr.setTypeAnalog();
qtr.setSensorPins((const uint8
_
t[]){A0, A1, A2, A3, A4}, SensorCount);
// Налаштування пінів моторів як виходів
pinMode(LEFT
MOTOR
_
_
PIN1, OUTPUT);
pinMode(LEFT
MOTOR
_
_
PIN2, OUTPUT);
pinMode(RIGHT
MOTOR
_
_
PIN1, OUTPUT);
pinMode(RIGHT
MOTOR
_
_
PIN2, OUTPUT);
pinMode(LEFT
MOTOR
_
_
PWM, OUTPUT);
pinMode(RIGHT
MOTOR
_
_
PWM, OUTPUT);
// Налаштування кнопки старту з внутрішнім підтягуючим резистором
pinMode(START
BUTTON, INPUT
_
_
PULLUP);
// Початок серійного зв'язку для налагодження
Serial.begin(9600);
Serial.println("Натисніть кнопку для калібрування сенсорів...
");
// Чекаємо натискання кнопки для калібрування
while (digitalRead(START
_
BUTTON) == HIGH) {
delay(10);
}
// Калібрування сенсорів
calibrateSensors();
}
4. Функція калібрування сенсорів
void calibrateSensors() {
Serial.println("Калібрування сенсорів...
");
Serial.println("Рухайте болідом вздовж лінії в обидві сторони");
// Блимання світлодіодом для індикації калібрування
pinMode(LED
_
BUILTIN, OUTPUT);
// Калібрування протягом 10 секунд
for (uint16
_
t i = 0; i < 400; i++) {
// Блимання світлодіодом
if (i % 40 == 0) {
digitalWrite(LED
_
BUILTIN, !digitalRead(LED
_
BUILTIN));
}
qtr.calibrate();
delay(25);
}
digitalWrite(LED
_
BUILTIN, LOW);
// Вивід мінімальних значень калібрування
Serial.println("Мінімальні значення:");
for (uint8
_
t i = 0; i < SensorCount; i++) {
Serial.print(qtr.calibrationOn.minimum[i]);
Serial.print(" ");
}
Serial.println();
// Вивід максимальних значень калібрування
Serial.println("Максимальні значення:");
for (uint8
_
t i = 0; i < SensorCount; i++) {
Serial.print(qtr.calibrationOn.maximum[i]);
Serial.print(" ");
}
Serial.println();
Serial.println("Калібрування завершено!");
Serial.println("Натисніть кнопку для запуску боліда...
");
// Чекаємо відпускання кнопки
while (digitalRead(START
_
BUTTON) == LOW) {
delay(10);
// Чекаємо повторного натискання кнопки для старту
while (digitalRead(START
_
BUTTON) == HIGH) {
delay(10);
}
}
// Невелика затримка перед стартом
delay(1000);
isRunning = true;
Serial.println("Старт!");
}
5. Основний цикл з PID-регулятором
void loop() {
// Керування стартом і зупинкою
if (digitalRead(START
_
BUTTON) == LOW && !isRunning) {
delay(50); // Для усунення дребезгу контактів
if (digitalRead(START
_
BUTTON) == LOW) {
isRunning = true;
Serial.println("Старт!");
delay(1000); // Затримка перед початком руху
}
} else if (digitalRead(START
_
BUTTON) == LOW && isRunning) {
delay(50);
if (digitalRead(START
_
BUTTON) == LOW) {
isRunning = false;
Serial.println("Зупинка!");
driveMotors(0, 0); // Зупинка моторів
delay(1000);
}
}
// Якщо болід запущено, виконуємо алгоритм руху
if (isRunning) {
// Зчитування позиції (0-4000)
uint16
_
t position = qtr.readLineBlack(sensorValues);
// Обчислення помилки
int error = position - 2000; // 2000 - це позиція центру
// Вивід даних для налагодження
Serial.print("Позиція: ");
Serial.print(position);
Serial.print(" Помилка: ");
Serial.println(error);
// Обчислення PID-значень
// Пропорційна складова
int P = error;
// Інтегральна складова
integral = integral + error;
// Обмеження інтегралу для запобігання перенасичення
if (integral > 1000) integral = 1000;
if (integral < -1000) integral = -1000;
int I = integral;
// Диференціальна складова
int D = error - lastError;
lastError = error;
// Обчислення PID-значення
int pidValue = Kp * P + Ki * I + Kd * D;
// Застосування PID до швидкості моторів
int leftMotorSpeed = BASE
_
SPEED - pidValue;
int rightMotorSpeed = BASE
_
SPEED + pidValue;
// Обмеження швидкості
if (leftMotorSpeed > 255) leftMotorSpeed = 255;
if (leftMotorSpeed < 0) leftMotorSpeed = 0;
if (rightMotorSpeed > 255) rightMotorSpeed = 255;
if (rightMotorSpeed < 0) rightMotorSpeed = 0;
// Керування моторами
driveMotors(leftMotorSpeed, rightMotorSpeed);
} else {
// Зупинка моторів
driveMotors(0, 0);
}
delay(10); // Коротка затримка для стабільності
}
6. Функція керування моторами
void driveMotors(int leftSpeed, int rightSpeed) {
// Керування лівим мотором
if (leftSpeed >= 0) {
digitalWrite(LEFT
_
digitalWrite(LEFT
MOTOR
MOTOR
_
_
_
PIN1, HIGH);
PIN2, LOW);
} else {
digitalWrite(LEFT
MOTOR
_
_
digitalWrite(LEFT
MOTOR
_
_
PIN1, LOW);
PIN2, HIGH);
leftSpeed = -leftSpeed;
}
// Керування правим мотором
if (rightSpeed >= 0) {
digitalWrite(RIGHT
_
digitalWrite(RIGHT
MOTOR
MOTOR
_
_
_
} else {
digitalWrite(RIGHT
MOTOR
_
_
PIN1, HIGH);
PIN2, LOW);
PIN1, LOW);
digitalWrite(RIGHT
MOTOR
_
_
rightSpeed = -rightSpeed;
PIN2, HIGH);
}
// Встановлення швидкості через ШІМ
analogWrite(LEFT
MOTOR
_
_
PWM, leftSpeed);
analogWrite(RIGHT
MOTOR
_
_
PWM, rightSpeed);
}