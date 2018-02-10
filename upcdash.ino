#include <DHT.h>
#include <DHT_U.h>
//Please install Adafruit Unified Sensor Library before compiling!
#define DHTTYPE DHT11
//Using DHT11 temperature and humidity module.It converts DHT11's original 4-pins to board's 3-pins
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const int DHT_PIN = 8;
//DHT11-DATA pin connected to Arduino Nano's Digital Pin 8.
DHT dht(DHT_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
//The IIC address of PCF8574 LCD screen is 0x27,LCD1602 has 2 rows and 16 charactors per row.
DS3231 Clock;
bool Century = false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
byte degree[] = {0x18, 0x18, 0x07, 0x08, 0x08, 0x08, 0x08, 0x07}; //The customized character centimetre.
byte nian[] = { 0x08, 0x0f, 0x12, 0x0f, 0x0a, 0x1f, 0x02, 0x02 };//The customized charactor Chinese "年"
byte yue[] = {0x1f, 0x11, 0x1f, 0x11, 0x1f, 0x11, 0x15, 0x13}; //The customized charactor Chinese "月"
byte ri[] = {0x1f, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x1f}; //The customized charactor Chinese "日"
byte shi[] = {0x04, 0x0A, 0x0A, 0x11, 0x11, 0x11, 0x11, 0x0E}; //The customized charactor of humidity
byte wen[] = {0x04, 0x0A, 0x0E, 0x0A, 0x0E, 0x11, 0x11, 0x0E};
byte Year, Month, Date, DoW, Hour, Minute, Second, digit;
/*-------Function name: GetDateStuff--------
   The main function:
   Receive date data from PC's serial,using
   the format of YYMMDDwHHMMSS and return it
   to the DS3231 RTC Module and user PC's
   serial port.
*/
void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW,
                  byte& Hour, byte& Minute, byte& Second) {
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];
  byte j = 0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] - 48;
  Temp2 = (byte)InString[1] - 48;
  Year = Temp1 * 10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] - 48;
  Temp2 = (byte)InString[3] - 48;
  Month = Temp1 * 10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] - 48;
  Temp2 = (byte)InString[5] - 48;
  Day = Temp1 * 10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;
  // now Hour
  Temp1 = (byte)InString[7] - 48;
  Temp2 = (byte)InString[8] - 48;
  Hour = Temp1 * 10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] - 48;
  Temp2 = (byte)InString[10] - 48;
  Minute = Temp1 * 10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] - 48;
  Temp2 = (byte)InString[12] - 48;
  Second = Temp1 * 10 + Temp2;
}
/*----------Function name:freeRam()----------
   Main function:use ATMega328's internal
   function to show the usage of 2KB SRAM
   inside the ATMega328 Chip.
*/
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
void ReadDS3231()
{
  int temp, humi, second, minute, hour, date, month, year;
  //Read I2C data from DS3231 module.
  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();
  temp = dht.readTemperature();
  humi = dht.readHumidity();
  //Start print on the LCD screen.
  //The first will be printed is the date(eg.YY-MM-DD)
  lcd.setCursor(0, 0);
  lcd.print(year, DEC);
  lcd.write(byte(1));
  lcd.print(month, DEC);
  lcd.write(byte(2));
  lcd.print(date, DEC);
  lcd.write(byte(3));
  lcd.print("   ");
  //The next is time,format as HH:MM
  if (hour < 10) {
    lcd.print('0');
  }
  lcd.print(hour, DEC);
  //use a interger to blink the column.
  if (digit != 0) {
    lcd.print(':');
    digit = 0;
  }
  else if (digit == 0) {
    lcd.print(" ");
    digit = 1;
  }
  if (minute < 10) {
    lcd.print('0');
  }
  lcd.print(minute, DEC);
  lcd.setCursor(0, 1);
  //Print the temperature.
  lcd.write(byte(4));
  lcd.print(temp);
  lcd.write(byte(0));
  lcd.print("  ");
  /**The UPC Dashboard function block


   **/
  if (hour <= 22 && hour >= 12) {
    lcd.print("O");
  }
  else {
    lcd.print("X");
  }
  if (hour >= 6 && hour <= 22) {
    lcd.print("O");
  }
  else {
    lcd.print("X");
  }
  if (hour >= 6 && hour <= 19) {
    lcd.print("O");
  }
  else {
    lcd.print("X");
  }
  if (hour >= 6 && hour <= 20)
  {
    if (hour >= 10 && hour <= 12) {
      lcd.print("O");
    }
    else if (hour >= 6 && hour <= 7) {
      lcd.print("O");
    }
    else if (hour >= 20 && hour <= 22) {
      lcd.print("O");
    }

  }
  else {
    lcd.print("X");
  }
  lcd.print("  ");
  lcd.write(byte(5));
  lcd.print(humi);
  lcd.print('%');
  delay(1000);
}
void setup() {
  dht.begin();
  for (int i; i < 6 && i > 1; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  // Start the I2C interface
  Wire.begin();
  lcd.init();
  // Print the welcome message and check the sensor and RTC modules.
  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.print("U");
  lcd.print("P");
  lcd.print("C ");
  lcd.print("D");
  lcd.print("a");
  lcd.print("s");
  lcd.print("h");
  lcd.print("b");
  lcd.print("o");
  lcd.print("a");
  lcd.print("r");
  lcd.print("d");
  lcd.setCursor(0, 1);
  lcd.print(" Now Booting... ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DESIGNED BY YKST");
  lcd.setCursor(0, 1);
  lcd.print(" www.upc.edu.cn ");
  //Create the LCD's custom charactor.
  lcd.createChar(0, degree);
  lcd.createChar(1, nian);
  lcd.createChar(2, yue);
  lcd.createChar(3, ri);
  lcd.createChar(4, wen);
  lcd.createChar(5, shi);

  Serial.begin(9600);
  if (Clock.getYear() < 10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC ERR,SET FORM");
    lcd.setCursor(0, 1);
    lcd.print(" YYMMDDwHHMMSS ");
    GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);
    Clock.setClockMode(false);
    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setDoW(DoW);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
  }
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  if (isnan(temp) || isnan(humi)) {
    lcd.setCursor(0, 0);
    lcd.print("BAD DHT11 SENSOR");
    lcd.setCursor(0, 1);
    lcd.print(" PLZ CONN TO D8 ");
    while (1) {

    }
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SELF TEST OK ");
  lcd.setCursor(0, 1);
  lcd.print("2048 TO,");
  lcd.print(freeRam());
  lcd.print(" FR");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FIRM_VER:ver1829");
  lcd.setCursor(0, 1);
  lcd.print("Now will boot...");
  delay(1000);
  lcd.backlight();
  lcd.clear();
  delay(1000);
}
void loop() {
  ReadDS3231();
}

