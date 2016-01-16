#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <UTFT.h>
#include <DS3231.h>

//////////////////////////SD
File myfile;
int SDpin = 11;

////////////////////////////////////////////////////////////////Часы////////
DS3231  rtc(SDA, SCL);
Time  t, now_min, now_time;
int n_min = 99;
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////Регистр Сдвига
//Пин DS of 74HC595
int DS_Pin = 8;
//Пин ST_CP of 74HC595
int ST_CP_Pin = 9;
//Пин SH_CP of 74HC595
int SH_CP_Pin = 10;
//Состояние реле (0 - включено, 1 - выключено)
byte relay_state_1;
byte relay_state_2;
byte current_relay_state_1 = B11111111;
byte current_relay_state_2 = B11111111;
//byte relay_state_all[2][8] = {
//  {0,0,0,0,0,0,0,0},
//  {0,0,0,0,0,0,0,0},
//};
byte relay_state_all[2][8];
boolean relay_change_st;
///////////////////////////////////////////////////////////////////////////////

//Режим сигнализации (TRUE - включена)
boolean AlarmState;

////////////////////////////////////////////////////////////RFID//////////////
//(uidDec == 3272815379 || uidDec == 875110550)
unsigned long uidDec, uidDecTemp;
#define RST_PIN    7  
#define SS_PIN    53   
MFRC522 mfrc522(SS_PIN, RST_PIN);
// Массив для хранения 50 "Card UID" / уникальных номеров NFC карт, считанных из EEPROM. 
unsigned long CardUIDeEPROMread[] = {0,1,2,3,4,5,6,7,8,9,10};
int ARRAYindexUIDcard; // Индекс для массива CardUIDeEPROMread.
int EEPROMstartAddr; // Стартовая ячейка памяти для записи / чтения EEPROM "Card UID".
int LockSwitch; // Замок / Переключатель / Блокиратор.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////Экран////////////////////
const byte ROWS = 4;
const byte COLS = 2;
extern uint8_t BigFont[];
extern uint8_t RusSmallFont[];
extern uint8_t RusSmallSymbolFont[];
UTFT myGLCD(CTE32HR,38,39,40,41);
char* menu_screen[ROWS][COLS] = {
  {"Main","Options"},
  {"Relay","Time"},
  {"Signalisation","123"},
  {"GSM","456"},
};
boolean MainScreen_Mode = true;
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////СТИК//////////////////////////
#define axis_X 2    // Ось Х подключена к Analog 0
#define axis_Y 3    // Ось Y подключена к Analog 1
#define BUTTON_PIN 2    // Ось Z (кнопка джойстика) подключена к Digital 2
boolean buttonWasUp = true;  // была ли кнопка отпущена?
boolean ledEnabled = false;  // включен ли свет?
boolean stick_move = false;
boolean stick_release_X = true;
boolean stick_release_Y = true;
boolean stick_slow_mode_X = true;
boolean stick_slow_mode_Y = true;
int mode_count_X,mode_count_Y;
int stick_poz_x,stick_poz_y = 0;
int current_stick_poz_x, current_stick_poz_y = 99;
int value_X, value_Y, value_Z = 0;    // Переменные для хранения значений осей
unsigned long time_X, time_Y, time_double_click;
unsigned long time_select_draw;
int d_click_count;
boolean d_click = false;
boolean d_click_sinch = false;
const byte shift_r = 2;


/////////////////////////////////////////////////////////////////////////////
boolean switchOn = false;
boolean cardRead = false;
void setup() {
  Serial.begin(9600); 
  Serial.println("Waiting for card...");
  SPI.begin();
  mfrc522.PCD_Init(); 
  pinMode(DS_Pin, OUTPUT); 
  pinMode(ST_CP_Pin, OUTPUT);
  pinMode(SH_CP_Pin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SDpin, OUTPUT);
  EEPROMreadUIDcard(); // Запускаем функцию, для перезаписи массива CardUIDeEPROMread, данными из EEPROM.
   // Если хотите увидеть в Serial мониторе все записанные № Card-UID, раскомментируйте строку.
  for(int i = 0; i <= 9; i++)Serial.print(i),Serial.print(" ---- "),Serial.println(CardUIDeEPROMread[i]);
  //for (int i = 0; i < 512; i++)EEPROM.write(i, 0); // EEPROM Clear / Очистить / стереть EEPROM. 
  //Инициируем дисплей
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.clrScr();
  rtc.begin();
  // Инициализация SD
  if (!SD.begin(SDpin)) {
    Serial.println("initialization failed!");
  }
  alarm_load();
  config_load();
  header();
  MainScreen();
  shiftWrite();
  
  //rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(21, 29, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(24, 12, 2015);
}
void alarm_load(){
  int index = 0;
  myfile = SD.open("alarm.txt");
  if (myfile)
  {
    int input = myfile.parseInt();
    AlarmState = input;
    myfile.close();
  }
}
void config_load(){
  int index = 0;
  switch (AlarmState){
    case 0:
    myfile = SD.open("relay0.txt");
    break;
    case 1:
    myfile = SD.open("relay1.txt");
    break;
  }
  if (myfile)
  {
    for (index = 0; index <= 15; index++) 
    {
    int input = myfile.parseInt();
    if (index < 8) relay_state_all[0][index] = input;
    else if (index >= 8) relay_state_all[1][index-8] = input;
    }
    myfile.close();
  }
}

void loop() {
  MainScreen();
  stickMove();
  //Serial.print("D_CLICK:");
  //Serial.println(d_click);
// Поиск новой NFC карты / Look for new cards.
if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Выбор карточки / Select one of the cards.
if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Выдача серийного номера карточки "UID".
for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec*256+uidDecTemp; 
  } 
if (uidDec == 875110550 || LockSwitch > 0)EEPROMwriteUIDcard(); // Запускаем функцию записи в EEPROM "Card UID" пользователей.

// Для режима управления.
   if (LockSwitch == 0) // только если находимся в "режиме управления".
  {
    // Подготавливаемся для перебора массива CardUIDeEPROMread.
    for(ARRAYindexUIDcard = 0; ARRAYindexUIDcard <= 9; ARRAYindexUIDcard++) 
    {
      // фильтр, на тот случай если мы не запишим все 50 переменных массива, в не записанных переменных массива будут значение 0.    
      if (CardUIDeEPROMread[ARRAYindexUIDcard] > 0) // Перебираем весь массив CardUIDeEPROMread, фильтруем от 0.
      {
        //  Перебираем весь массив CardUIDeEPROMread, Если поднесённая карта совпадёт с одной из 50  переменных массива.       
        if (CardUIDeEPROMread[ARRAYindexUIDcard] == uidDec) // Если обнаружено совпадение с поднесенной картой.
        {
          CommandsCARD(); // Запускаем функцию для выполнения любых действий, в зависимости от "Card UID" поднесённой карты.
          break; // Выходим, останавливаем проверку / перебор / поиск, в массиве CardUIDeEPROMread.
          // Полезная вещь! Итак, в момент перебора массива, в случае совпадения с поднесенным "Card UID", мы останавливаем 
          // цикл  for, с помощью break, и в значении ARRAYindexUIDcard сохранится индекс массива, в котором находился "Card UID".
          // Знания значения индекса массива, упростит выполнение других задач. 
        }
      }
    }   
    if (ARRAYindexUIDcard == 10);
    delay(2000);    
    ARRAYindexUIDcard = 0;
  }
}
void CommandsCARD(){
  SD.remove("alarm.txt");
  myfile = SD.open("alarm.txt", FILE_WRITE);
  switch (AlarmState){
   case 0:
    Serial.println("ON");
    AlarmState = 1;
    myfile.print(AlarmState);
    myfile.close();
    config_load();
    shiftWrite();
    break;
  case 1:
    Serial.println("OFF");
    AlarmState = 0;
    myfile.print(AlarmState);
    myfile.close();
    config_load();
    shiftWrite();
    break;
  }
}

void shiftWrite(){
  switch (AlarmState){
   case 0:
   SD.remove("relay0.txt");
   myfile = SD.open("relay0.txt", FILE_WRITE);
   break;
   case 1:
   SD.remove("relay1.txt");
   myfile = SD.open("relay1.txt", FILE_WRITE);
   break;
  }
  byte r_st[2]; 
  for (int j=0; j<2; j++)
  {
    for (int i=0; i<8; i++)
    {
      bitWrite(r_st[j], i, relay_state_all[j][i]);
      myfile.print(relay_state_all[j][i]);
      myfile.print(";");
    }
  }
  myfile.close();
  digitalWrite(ST_CP_Pin, LOW);
  shiftOut(DS_Pin, SH_CP_Pin, MSBFIRST, r_st[1]);
  shiftOut(DS_Pin, SH_CP_Pin, MSBFIRST, r_st[0]);  
  digitalWrite(ST_CP_Pin, HIGH);
  relay_change_st = true;
  switch (AlarmState){
   case 0:
      myGLCD.setColor(255, 0, 0);
      myGLCD.fillCircle(16,35,6);
      myGLCD.print("B""\x91""K""\x88", 25, 27);
      break;
   case 1:
      myGLCD.setColor(0, 255, 0);
      myGLCD.fillCircle(16,35,6);
      myGLCD.print("BK""\x88"" ", 25, 27);
      break; 
   }
}



/////////////////////////////////////////////////////////////////////////////////ФУНКЦИЯ ДЛЯ РАБОТЫ С КАРТАМИ////////////////////////
// Делаем функцию, перезаписи массива CardUIDeEPROMread, данными из EEPROM.
void EEPROMreadUIDcard()
{
  for(int i = 0; i <= 9; i++)
  {
    byte val = EEPROM.read(EEPROMstartAddr+3);   
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr+2); 
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr+1); // увеличиваем EEPROMstartAddr на 1.
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val; 
    val = EEPROM.read(EEPROMstartAddr); 
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;

    ARRAYindexUIDcard++; // увеличиваем на 1.
    EEPROMstartAddr +=5; // увеличиваем на 5.
  }

  ARRAYindexUIDcard = 0;
  EEPROMstartAddr = 0; 
  uidDec = 0;
  LockSwitch = 0;      
}

// Делаем функцию, записи в EEPROM "Card UID" пользователей, максимум 50 "Card UID".
void EEPROMwriteUIDcard() {

  if (LockSwitch == 0) // Если находились в режиме управления.
  {
    Serial.println("   START    RECORD   Card UID   CLIENT"); 
    delay(1000);
  } 
  // Для пропуска записи в ячейку памяти.
  if (LockSwitch > 0) // Если находимся в "режиме записи".
  {
    if (uidDec == 875110550) // Если поднесена карта администратора. 
    {
      Serial.println("   SKIP     RECORD   "); 
      Serial.println(EEPROMstartAddr/5); // Выводим № пропущенной ячейки памяти.

      EEPROMstartAddr += 5; // Пропускаем запись в ячейку памяти, если не хотим записывать туда "Card UID".
      delay(200);
    }

    else // Иначе, то есть поднесена карта для записи.

    // "Card UID" / № карты это "длинное число", которое не поместится в одну ячейку памяти EEPROM. 
    // Разрубим "длинное число" на 4 части, и кусками, запишем его в 4 ячейки EEPROM. Начинаем запись с адреса EEPROMstartAddr.

    {
      EEPROM.write(EEPROMstartAddr, uidDec & 0xFF); 
      EEPROM.write(EEPROMstartAddr + 1, (uidDec & 0xFF00) >> 8); 
      EEPROM.write(EEPROMstartAddr + 2, (uidDec & 0xFF0000) >> 16); 
      EEPROM.write(EEPROMstartAddr + 3, (uidDec & 0xFF000000) >> 24);
      // Записали!.
      delay(10);
      // --
      Serial.println("RECORD OK! IN MEMORY "); 
      Serial.println(EEPROMstartAddr/5); // Выводим № записанной ячейки памяти.

      EEPROMstartAddr += 5; // Прибавляем 5 к стартовой ячейки записи.
      delay(2000);
    }   
  }

  LockSwitch++; // Разблокируем режим записи, и заблокируем режим управления. 

  if (EEPROMstartAddr/5 == 9) // если дошли до 10. 
  {
    delay(1000);
    Serial.println("RECORD FINISH"); 
    delay(1000);
    EEPROMstartAddr = 0;   
    uidDec = 0;
    ARRAYindexUIDcard = 0;

    EEPROMreadUIDcard(); // Запускаем функцию, для перезаписи массива CardUIDeEPROMread, данными из EEPROM.
  } 
}
void clockFunk() 
{
  t = rtc.getTime();
  if (n_min != t.min)
    {
      n_min = t.min;
      myGLCD.setBackColor(0, 0, 255);
      myGLCD.setColor(245, 245, 245);
      myGLCD.print(rtc.getTimeStr(FORMAT_SHORT), 390, 6);
      myGLCD.print(rtc.getDateStr(FORMAT_SHORT), 342, 27);
      myGLCD.printNumI(rtc.getTemp(), 6, 6);
	    myGLCD.print("\x7F""C", 38, 6);
    }
  myGLCD.setColor(245, 245, 245);
  
}
void header()
{
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRect(1, 1, 478, 47);
  myGLCD.fillRect(1, 299, 478, 318);
  myGLCD.setColor(245, 245, 245);
  myGLCD.drawRect(0, 0, 479, 48);
  myGLCD.drawRect(0, 298, 479, 319);
  myGLCD.setColor(245, 245, 245);
  myGLCD.drawRoundRect(6, 25, 90, 45);
  myGLCD.drawCircle(16,35,8);  
}
void MainScreen()
{
  int x_poz = 380;
  int y_poz = 60;
  int j = 0;
  switch (stick_poz_x){
    case 1:
      myGLCD.setColor(0, 0, 255);
      myGLCD.drawRect(x_poz, y_poz+17*stick_poz_y, x_poz+16, (y_poz+17*stick_poz_y)+16);
      break;
    case 2:
      myGLCD.setColor(0, 0, 255);
      myGLCD.drawRect(x_poz+17, y_poz+17*stick_poz_y, x_poz+33, (y_poz+17*stick_poz_y)+16);
      break;
  }
  
  if ((current_stick_poz_y != stick_poz_y)||(relay_change_st)||(current_stick_poz_x != stick_poz_x))
  {
    current_stick_poz_y = stick_poz_y;
    current_stick_poz_x = stick_poz_x;
    for (int j = 0; j < 2; j++)
    {
      for (int i = 0; i < 8 ; i++)
      {
      if ( i == stick_poz_y && stick_poz_x == j+1)
      {
        i=i;
      }
      else
      {
        myGLCD.setColor(245, 245, 245);
        myGLCD.drawRect(x_poz+17*j, y_poz+17*i, (x_poz+17*j)+16, (y_poz+17*i)+16);
      }
      
      if (relay_state_all[j][i]==0) myGLCD.setColor(0, 255, 0);
      else if (relay_state_all[j][i]==1) myGLCD.setColor(255, 0, 0);
      myGLCD.fillRect(x_poz+2+17*j, y_poz+2+17*i, x_poz+14+17*j, (y_poz+17*i)+14);
      }
    }
    relay_change_st = false;       
  }
  clockFunk();  

}
void SettingsMenu()
{ 
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  
  //myGLCD.print("Main Menu", LEFT, 0);  
  //myGLCD.print(rtc.getTimeStr(), RIGHT, 20);
  int x_poz = 10;
  int y_poz = 60;
  int j = 0;
  for (int i = 0; i < 4 ; i++)
    {
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.print(menu_screen[i][j], x_poz+5, (y_poz+7)+50*i);
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawRect(x_poz, y_poz+50*i, x_poz+230, (y_poz+50*i)+30);   
   }   
}

void stickMove()
{
  boolean buttonIsUp = digitalRead(BUTTON_PIN);
 
  // ...если «кнопка была отпущена и (&&) не отпущена сейчас»...
  if (buttonWasUp && !buttonIsUp) {
    // ...может это «клик», а может и ложный сигнал (дребезг),
    // возникающий в момент замыкания/размыкания пластин кнопки,
    // поэтому даём кнопке полностью «успокоиться»...
    delay(5);
    // ...и считываем сигнал снова
    buttonIsUp = digitalRead(BUTTON_PIN);
    if (!buttonIsUp) {  // если она всё ещё нажата...
      // ...это клик! Переворачиваем сигнал светодиода
      ledEnabled = !ledEnabled;
      //digitalWrite(LED_PIN, ledEnabled);
      d_click_count = d_click_count + 1;
      if (d_click_sinch == false)
      {
        time_double_click = millis();
        d_click_sinch = true;
      }
      if (d_click_count == 2 && millis() - time_double_click < 100)
      {
        d_click = !d_click;
        d_click_sinch = false;
      }
      else if (millis() - time_double_click >= 1000)
      {
        d_click_count = 0;
        d_click_sinch = false;
      }
      if (MainScreen_Mode && ((stick_poz_x ==1)||(stick_poz_x ==2)))
      {
        relay_state_all[stick_poz_x-1][stick_poz_y] = !relay_state_all[stick_poz_x-1][stick_poz_y];
        shiftWrite();
      }
      //boolean r_registr = bitRead(relay_state_1, stick_poz_y);
     // bitWrite(relay_state_1, stick_poz_y, !r_registr);
      //shiftWrite();
    }
  }
  // запоминаем последнее состояние кнопки для новой итерации
  buttonWasUp = buttonIsUp;
  
  // запоминаем последнее состояние кнопки для новой итерации
  value_X = analogRead(axis_X);    // Считываем аналоговое значение оси Х
  if (value_X > 700 && stick_release_X)
  {
    time_X = millis();
    stick_release_X = !stick_release_X;
    if (stick_poz_x>=7) stick_poz_x = 0;
    else stick_poz_x = stick_poz_x+1;
  }
  if (value_X < 400 && stick_release_X)
  {
    time_X = millis();
    stick_release_X = !stick_release_X;
    if (stick_poz_x<=0) stick_poz_x = 7;
    else stick_poz_x = stick_poz_x-1;   
  }
  if ((value_X > 700 || value_X < 400) && stick_slow_mode_X && millis() - time_X >= 700)
  {
    stick_release_X = true;
    mode_count_X = mode_count_X + 1;
    if (mode_count_X == 5)
    {
      stick_slow_mode_X = false;
      mode_count_X = 0;
    }
  }
  if (!stick_slow_mode_X && millis() - time_X >= 500) stick_release_X = true;      
  if (value_X > 400 && value_X < 600)
  {
   stick_release_X = true;
   stick_slow_mode_X = true; 
   mode_count_X = 0;  
  }
  
  value_Y = analogRead(axis_Y);    // Считываем аналоговое значение оси Х
  if (value_Y > 700 && stick_release_Y)
  {
    time_Y = millis();
    stick_release_Y = !stick_release_Y;
    if (stick_poz_y>=7) stick_poz_y = 0;
    else stick_poz_y = stick_poz_y+1;   
  }
  if (value_Y < 400 && stick_release_Y)
  {
    time_Y = millis();
    stick_release_Y = !stick_release_Y;
    if (stick_poz_y<=0) stick_poz_y = 7;
    else stick_poz_y = stick_poz_y-1;   
  }
  if ((value_Y > 700 || value_Y < 400) && stick_slow_mode_Y && millis() - time_Y >= 700)
  {
    stick_release_Y = true;
    mode_count_Y = mode_count_Y + 1;
    if (mode_count_Y == 5)
    {
      stick_slow_mode_Y = false;
      mode_count_Y = 0;
    }
  }
  if (!stick_slow_mode_Y && millis() - time_Y >= 100) 
  {
    stick_release_Y = true;   
  }
  if (value_Y > 400 && value_Y < 600)
  {
   stick_release_Y = true;
   stick_slow_mode_Y = true; 
   mode_count_Y = 0;  
  }
   
}
