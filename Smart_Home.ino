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
Time  t;
int now_min, now_hour, now_date, now_month, now_year;
int n_min = 99;
int n_hour = 99;
boolean time_changed = false;
int now_date_str[3][3];
char* now_date_simb[2][3] = {
	{":","",""},
	{".",".",""},
};
boolean min1, hour1 = false;
boolean date1, month1, year1 = false;
char* dayoftheweek[8][2] = {
	{ "", ""},
	{ "MONDAY", "\x89""o""\xA2""e""\x99""e""\xA0\xAC\xA2\x9D\x9F"},
	{ "TUESDAY", "B""\xA4""op""\xA2\x9D\x9F"},
	{ "WEDNESDAT", "Cpe""\x99""a"},
	{ "THURSDAY", "\x8D""e""\xA4\x97""ep""\x98"},
	{ "FRIDAY", "\x89\xAF\xA4\xA2\x9D\xA6""a"},
	{ "SATURDAY", "Cy""\x96\x96""o""\xA4""a"},
	{ "SUNDAY", "Boc""\x9F""pece""\xA2\xAC""e"},
};
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
//Задаем разметку главного экрана.0-виджет,1-меню
 byte M_Scr_Mark[4][3] = {
  {1, 1, 1},
  {1, 1, 1},
  {1, 1, 1},
  {2, 2, 2},
};
//храним позиции для разметки
int y_position[2][20];
int x_position[2][20];
int numofcolls,numofrows;
int y_zone_coord[] = {50, 297};
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
unsigned long CardUIDeEPROMread[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int ARRAYindexUIDcard; // Индекс для массива CardUIDeEPROMread.
int EEPROMstartAddr; // Стартовая ячейка памяти для записи / чтения EEPROM "Card UID".
int LockSwitch; // Замок / Переключатель / Блокиратор.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////Экран////////////////////
extern uint8_t BigFont[];
extern uint8_t RusSmallFont[];
extern uint8_t RusSmallSymbolFont[];
UTFT myGLCD(CTE32HR, 38, 39, 40, 41);
char* mainscreen_label[4][3] = {
	{ "123", "Oc""\x97""e""\xA9""e""\xA2\x9D""e", "Po""\x9C""e""\xA4\x9F\x9D",}, //...,Освещение, Розетки
	{"","",""},
	{ "Hacoc", "Pe""\x9C""ep""\x97""yap", "" }, //Насос, Резервуар
	{ "Hac""\xA4""po""\x9E\x9F\x9D", "", "" }, //Настройки,.....
};
char* mainscreen_widget_description[4][3] = {
	{"", "\x8A\xA3""pa""\x97\xA0""e""\xA2\x9D""e oc""\x97""e""\xA9""e""\xA2\x9D""e""\xA1", "\x8A\xA3""pa""\x97\xA0""e""\xA2\x9D""e po""\x9C""e""\xA4\x9F""a""\xA1\x9D" },
	{"", "", ""},
	{"", "", "\x8A\xA3""pa""\x97\xA0""e""\xA2\x9D""e po""\x9C""e""\xA4\x9F""a""\xA1\x9D"},
	{"Me""\xA2\xAE"" ""\xA2""ac""\xA4""poe""\x9F"" ""\x99""o""\xA3""o""\xA0\xA2\x9D\xA4""e""\xA0\xAC\xA2\xAB""x ""\xA3""apa""\xA1""e""\xA4""po""\x97", "", "" },
};
char* Settings_Menu[5][2] = {
	{ "\x82""a""\xA4""a ""\x9D"" Bpe""\xA1\xAF", "" },
	{"",""},
	{"3","8"},
	{"4","9"},
	{ "5", "B""\xAB""xo""\x99"},
};
char* SettingsTime_Menu[] = { "        ", "        ", "        ", "Time NTP", "Ha""\x9C""a""\x99"};
boolean MainScreen_Mode = true;
boolean light_widget_mode, power_widget_mode = false;
boolean SettingScreen_Mode = false;
boolean SettingsTime_Mode, SettingsTime_Edit, SettingsDate_Edit, SettingsDOW_Edit = false;
boolean widget_activ = false; //Индикатор активации виджета
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
int mode_count_X, mode_count_Y;
int stick_poz_x, stick_poz_y = 0;
int widget_stick_poz_x, widget_stick_poz_y = 0;
int current_stick_poz_x, current_stick_poz_y = 2;
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
  for (int i = 0; i <= 9; i++)Serial.print(i), Serial.print(" ---- "), Serial.println(CardUIDeEPROMread[i]);
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
  MainScreen_draw();
  shiftWrite();

  //rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(21, 29, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(24, 12, 2015);
}
//При включении считываем состояние сигнализации (включена или выключена)
void alarm_load() {
  int index = 0;
  myfile = SD.open("alarm.txt");
  if (myfile)
  {
    int input = myfile.parseInt();
    AlarmState = input;
    myfile.close();
  }
}
//Загружаем конфигурацию состояний Реле в зависимости от состояния сигнализации
void config_load() {
  int index = 0;
  switch (AlarmState) {
    case 0:								//Если сигнализация Выключена
      myfile = SD.open("relayoff.txt");
      break;
	case 1:								//Если сигнализация Включена
      myfile = SD.open("relayon.txt");
      break;
  }
  if (myfile)
  {
    for (index = 0; index <= 15; index++)
    {
      int input = myfile.parseInt();
      if (index < 8) relay_state_all[0][index] = input;
      else if (index >= 8) relay_state_all[1][index - 8] = input;
    }
    myfile.close();
  }
}

void loop() {
	if (MainScreen_Mode) MainScreen();
	if (SettingScreen_Mode || SettingsTime_Mode) SettingsMenu();
	if (light_widget_mode || power_widget_mode) widget_function();
	if (SettingsTime_Edit) Time_Edit();
	if (SettingsDate_Edit) Date_Edit();
	if (SettingsDOW_Edit) DOW_edit();
	stickMove();
	clockFunk();
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
    uidDec = uidDec * 256 + uidDecTemp;
  }
  if (uidDec == 875110550 || LockSwitch > 0)EEPROMwriteUIDcard(); // Запускаем функцию записи в EEPROM "Card UID" пользователей.

  // Для режима управления.
  if (LockSwitch == 0) // только если находимся в "режиме управления".
  {
    // Подготавливаемся для перебора массива CardUIDeEPROMread.
    for (ARRAYindexUIDcard = 0; ARRAYindexUIDcard <= 9; ARRAYindexUIDcard++)
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
void CommandsCARD() {
  SD.remove("alarm.txt");
  myfile = SD.open("alarm.txt", FILE_WRITE);
  switch (AlarmState) {
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

void shiftWrite() {
  switch (AlarmState) {
    case 0:
      SD.remove("relayoff.txt");
      myfile = SD.open("relayoff.txt", FILE_WRITE);
      break;
    case 1:
      SD.remove("relayon.txt");
      myfile = SD.open("relayon.txt", FILE_WRITE);
      break;
  }
  byte r_st[2];
  for (int j = 0; j < 2; j++)
  {
    for (int i = 0; i < 8; i++)
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
  switch (AlarmState) {
    case 0:
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.setColor(255, 0, 0);
		myGLCD.fillCircle(16, 35, 6);
		myGLCD.print("B""\x91""K""\x88", 25, 27);
    break;
    case 1:
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.setColor(0, 255, 0);
		myGLCD.fillCircle(16, 35, 6);
		myGLCD.print("BK""\x88"" ", 25, 27);
      break;
  }
  if (MainScreen_Mode || light_widget_mode || power_widget_mode) relay_work();
}



/////////////////////////////////////////////////////////////////////////////////ФУНКЦИЯ ДЛЯ РАБОТЫ С КАРТАМИ////////////////////////
// Делаем функцию, перезаписи массива CardUIDeEPROMread, данными из EEPROM.
void EEPROMreadUIDcard()
{
  for (int i = 0; i <= 9; i++)
  {
    byte val = EEPROM.read(EEPROMstartAddr + 3);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 2);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 1); // увеличиваем EEPROMstartAddr на 1.
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;

    ARRAYindexUIDcard++; // увеличиваем на 1.
    EEPROMstartAddr += 5; // увеличиваем на 5.
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
      Serial.println(EEPROMstartAddr / 5); // Выводим № пропущенной ячейки памяти.

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
      Serial.println(EEPROMstartAddr / 5); // Выводим № записанной ячейки памяти.

      EEPROMstartAddr += 5; // Прибавляем 5 к стартовой ячейки записи.
      delay(2000);
    }
  }

  LockSwitch++; // Разблокируем режим записи, и заблокируем режим управления.

  if (EEPROMstartAddr / 5 == 9) // если дошли до 10.
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
	if (n_min != t.min || n_hour != t.hour || time_changed)
	{
		now_date_str[0][0] = t.hour;
		now_date_str[0][1]= t.min;
		now_date_str[0][2] = t.sec;
		now_date_str[1][0] = t.date;
		now_date_str[1][1] = t.mon;
		now_date_str[1][2] = t.year;
		now_date_str[2][0] = t.dow;
		n_min = t.min;
		n_hour = t.hour;
		now_hour = t.hour;
		now_min = t.min;
		time_changed = false;
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
	myGLCD.drawCircle(16, 35, 8);
}
void MainScreen_draw(){
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRect(0, y_zone_coord[0], 479, y_zone_coord[1]);
	numofrows = sizeof(M_Scr_Mark) / sizeof(M_Scr_Mark[1]);
	numofcolls = sizeof(M_Scr_Mark) / numofrows;
	Serial.print(numofcolls);
	Serial.println(numofrows);
	int x_length = 480 / numofcolls;
	int n = 0;
	for (int i = 0; i < numofrows; i++)
	{
		if (M_Scr_Mark[i][0] == 2)
		{
			n = n + 1;
		}
	}
	int y_length = (y_zone_coord[1] - y_zone_coord[0] - 22 * n) / (numofrows - n);
	Serial.println(y_length*(numofrows - n) + 22 * n);
	if (y_length*(numofrows - n) + 22 * n > y_zone_coord[1] - y_zone_coord[0])
	{
		y_length = y_length - 1;
	}
	Serial.print(x_length);
	Serial.println(y_length);
	y_position[0][0] = y_zone_coord[0];
	if (M_Scr_Mark[0][0] == 1)
	{
		y_position[1][0] = y_zone_coord[0] + y_length - 1;
	}
	else if (M_Scr_Mark[0][0] == 2)
	{
		y_position[1][0] = y_zone_coord[0] + 21;
	}
	for (int i = 1; i < numofrows; i++)
	{
		if (M_Scr_Mark[i][0] == 2)
		{
			y_position[0][i] = y_position[1][i - 1] + 1;
			y_position[1][i] = y_position[0][i] + 21;
		}
		if (M_Scr_Mark[i][0] == 1)
		{
			y_position[0][i] = y_position[1][i - 1] + 1;
			y_position[1][i] = y_position[0][i] + y_length - 1;
		}
	}
	x_position[0][0] = 0;
	x_position[1][0] = x_length - 1;
	for (int i = 1; i < numofcolls; i++)
	{
		x_position[0][i] = x_position[1][i - 1] + 1;
		x_position[1][i] = x_position[0][i] + x_length - 1;
	}
	myGLCD.setColor(245, 245, 245);
	for (int i = 0; i < numofrows; i++)
	{
		for (int j = 0; j<numofcolls; j++)
		{
			myGLCD.drawRect(x_position[0][j], y_position[0][i], x_position[1][j], y_position[1][i]);
			myGLCD.setBackColor(0, 0, 0);
			myGLCD.print(mainscreen_label[i][j], x_position[0][j] + 3, y_position[0][i] + 3);
		}
	}
}
void MainScreen() //Отризовка главного экрана, области выделения виджетов и меню
{
	if ((current_stick_poz_y != stick_poz_y) || (relay_change_st) || (current_stick_poz_x != stick_poz_x))
	{
		if (MainScreen_Mode){
			//Если находимся на главном экране, то отрисовываем обводку выделенного элемента
			Serial.print(stick_poz_x);
			Serial.print("||");
			Serial.println(stick_poz_y);
			relay_change_st = false;
			myGLCD.setColor(245, 245, 245);
			myGLCD.drawRect(x_position[0][current_stick_poz_x], y_position[0][current_stick_poz_y], x_position[1][current_stick_poz_x], y_position[1][current_stick_poz_y]);
			myGLCD.setColor(0, 0, 0);
			myGLCD.drawRect(x_position[0][current_stick_poz_x] + 1, y_position[0][current_stick_poz_y] + 1, x_position[1][current_stick_poz_x] - 1, y_position[1][current_stick_poz_y] - 1);
			myGLCD.setColor(0, 255, 0);
			myGLCD.drawRect(x_position[0][stick_poz_x], y_position[0][stick_poz_y], x_position[1][stick_poz_x], y_position[1][stick_poz_y]);
			myGLCD.drawRect(x_position[0][stick_poz_x] + 1, y_position[0][stick_poz_y] + 1, x_position[1][stick_poz_x] - 1, y_position[1][stick_poz_y] - 1);
			current_stick_poz_y = stick_poz_y;
			current_stick_poz_x = stick_poz_x;
		}
	}
	Serial.print(now_date_str[0][0]);
	Serial.print("||");
	Serial.print(now_date_str[0][1]);
	Serial.print("||");
	Serial.print(now_date_str[0][2]);
	Serial.print("||");
	Serial.print(now_date_str[1][0]);
	Serial.print("||");
	Serial.print(now_date_str[1][1]);
	Serial.print("||");
	Serial.print(now_date_str[1][2]);
	Serial.println("||");
}
	// Условие для отрисовки обводки текущего реле в виджете Освещения.
	/*if ((current_stick_poz_x != stick_poz_x && light_widget_mode) || widget_activ){
		widget_activ = false;
		myGLCD.setColor(245, 245, 245);
		myGLCD.drawRect(172 + 17 * stick_poz_x, 84, 172 + 15 + 17 * stick_poz_x, 99);
		myGLCD.setColor(0, 0, 0);
		myGLCD.drawRect(172 + 17 * current_stick_poz_x, 84, 172 + 15 + 17 * current_stick_poz_x, 99);
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
	}
	// Условие для отрисовки обводки текущего реле в виджете Розетки.
	if ((current_stick_poz_x != stick_poz_x && light_widget_mode) || widget_activ){
		widget_activ = false;
		myGLCD.setColor(245, 245, 245);
		myGLCD.drawRect(331 + 17 * stick_poz_x, 84, 331 + 15 + 17 * stick_poz_x, 99);
		myGLCD.setColor(0, 0, 0);
		myGLCD.drawRect(331 + 17 * current_stick_poz_x, 84, 331 + 15 + 17 * current_stick_poz_x, 99);
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
	}*/
	// Условие для отрисовки обводки текущего реле в виджете Освещение и Розетки
void widget_function(){
	if ((current_stick_poz_x != stick_poz_x && light_widget_mode) || widget_activ || (current_stick_poz_x != stick_poz_x && power_widget_mode)){
		widget_activ = false;
		int x;
		if (light_widget_mode) x = 172;
		if (power_widget_mode) x = 331;
		myGLCD.setColor(245, 245, 245);
		myGLCD.drawRect(x + 17 * stick_poz_x, 84, x + 15 + 17 * stick_poz_x, 99);
		myGLCD.setColor(0, 0, 0);
		myGLCD.drawRect(x + 17 * current_stick_poz_x, 84, x + 15 + 17 * current_stick_poz_x, 99);
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
	}
	if (stick_poz_y != current_stick_poz_y && light_widget_mode) //Выход из виджета Освещение на главный экран
	{
		current_stick_poz_y = stick_poz_y;
		relay_state_all[0][stick_poz_x] = !relay_state_all[0][stick_poz_x];
		shiftWrite();
	}
	if (stick_poz_y != current_stick_poz_y && power_widget_mode) //Выход из виджета Розетки на главный экран
	{
		current_stick_poz_y = stick_poz_y;
		relay_state_all[1][stick_poz_x] = !relay_state_all[1][stick_poz_x];
		shiftWrite();
	}
}
void relay_work(){
	for (int j = 0; j < 2; j++){
		for (int i = 0; i < 8; i++){
			if (relay_state_all[j][i] == 0) myGLCD.setColor(0, 255, 0); //Включенное реле выделяем Зеленым
			if (relay_state_all[j][i] == 1) myGLCD.setColor(255, 0, 0); //Выключенное реле выделяем Красным
			if (j == 0)
			{
				myGLCD.fillRect(173 + 17 * i, 85, 173 + 13 + 17 * i, 98);
			}
			if (j == 1)
			{
				myGLCD.fillRect(332 + 17 * i, 85, 332 + 13 + 17 * i, 98);
			}
			myGLCD.setColor(245, 245, 245);

		}
	}
}
/*void relay_work() {
  int x_poz = 380;
  int y_poz = 60;
  int j = 0;
  switch (stick_poz_x) {
    case 1:
      myGLCD.setColor(0, 0, 255);
      myGLCD.drawRect(x_poz, y_poz + 17 * stick_poz_y, x_poz + 16, (y_poz + 17 * stick_poz_y) + 16);
      break;
    case 2:
      myGLCD.setColor(0, 0, 255);
      myGLCD.drawRect(x_poz + 17, y_poz + 17 * stick_poz_y, x_poz + 33, (y_poz + 17 * stick_poz_y) + 16);
      break;
  }

  if ((current_stick_poz_y != stick_poz_y) || (relay_change_st) || (current_stick_poz_x != stick_poz_x))
  {
    current_stick_poz_y = stick_poz_y;
    current_stick_poz_x = stick_poz_x;
    for (int j = 0; j < 2; j++)
    {
      for (int i = 0; i < 8 ; i++)
      {
        if ( i == stick_poz_y && stick_poz_x == j + 1)
        {
          i = i;
        }
        else
        {
          myGLCD.setColor(245, 245, 245);
          myGLCD.drawRect(x_poz + 17 * j, y_poz + 17 * i, (x_poz + 17 * j) + 16, (y_poz + 17 * i) + 16);
        }

        if (relay_state_all[j][i] == 0) myGLCD.setColor(0, 255, 0);
        else if (relay_state_all[j][i] == 1) myGLCD.setColor(255, 0, 0);
        myGLCD.fillRect(x_poz + 2 + 17 * j, y_poz + 2 + 17 * i, x_poz + 14 + 17 * j, (y_poz + 17 * i) + 14);
      }
    }
    relay_change_st = false;
  }
  clockFunk();
}*/
void SettingsMenu_draw()
{
	myGLCD.setColor(0,0,0);
	myGLCD.fillRect(0,y_zone_coord[0],479,y_zone_coord[1]);
	int x_poz = 21;
	int y_poz = 49;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j<2; j++)
		{
			myGLCD.setColor(255, 255, 255);
			myGLCD.drawRect(x_poz + (205 + 21)*j, y_poz + 15 + (45)*i, x_poz + 205 + (22 + 205)*j, y_poz + 45 + 45 * i);
			myGLCD.setBackColor(0, 0, 0);
			myGLCD.print(Settings_Menu[i][j], x_poz + 7 + 227*j , y_poz + 23 + 45*i);	
		}
	}
}
void SettingsMenu()
{
	if ((current_stick_poz_y != stick_poz_y) || (current_stick_poz_x != stick_poz_x))
	{
		//Если находимся на главном экране, то отрисовываем обводку выделенного элемента
		Serial.print(stick_poz_x);
		Serial.print("||");
		Serial.println(stick_poz_y);
		relay_change_st = false;
		myGLCD.setColor(245, 245, 245);
		myGLCD.drawRect(21 + (205 + 21)*current_stick_poz_x, 49 + 15 + (45)*current_stick_poz_y, 21 + 205 + (22 + 205)*current_stick_poz_x, 49 + 45 + 45 * current_stick_poz_y);
		myGLCD.setColor(0, 0, 0);
		myGLCD.drawRect(21 + 1 + (205 + 21)*current_stick_poz_x, 49 + 16 + (45)*current_stick_poz_y, 21 + 204 + (22 + 205)*current_stick_poz_x, 49 + 44 + 45 * current_stick_poz_y);
		myGLCD.setColor(0, 255, 0);
		myGLCD.drawRect(21 + (205 + 21)*stick_poz_x, 49 + 15 + (45)*stick_poz_y, 21 + 205 + (22 + 205)*stick_poz_x, 49 + 45 + 45 * stick_poz_y);
		myGLCD.drawRect(21 + 1 + (205 + 21)*stick_poz_x, 49 + 16 + (45)*stick_poz_y, 21 + 204 + (22 + 205)*stick_poz_x, 49 + 44 + 45 * stick_poz_y);
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
	}
}
void SettingsTime_draw()
{
	//Отрисовка времени
	int x_poz = 21;
	int y_poz = 49;
	//myGLCD.setColor(0, 0, 0);
	//myGLCD.fillRect(0, y_zone_coord[0], 479, y_zone_coord[1]);
	for (int i = 1; i < 5; i++)
	{
		myGLCD.setColor(0, 0, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.drawRect(x_poz, y_poz + 15 + (45)*i, x_poz + 205, y_poz + 45 + 45 * i);
		myGLCD.print("          ", 28, y_poz + 23 + 45 * i);
	}

	for (int i = 0; i < 5; i++)
	{
			myGLCD.setColor(255, 255, 255);
			//myGLCD.drawRect(x_poz + (205 + 21)*j, y_poz + 15 + (45)*i, x_poz + 205 + (22 + 205)*j, y_poz + 45 + 45 * i);
			myGLCD.setBackColor(0, 0, 0);
			myGLCD.print(SettingsTime_Menu[i], 255, y_poz + 23 + 45 * i);
	}
	//Выводим текущие значения времени и даты в поля для редактирования
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			x_poz = 255;
			y_poz = 72;
			if (i == 0 && j == 2) continue;
			if ((i == 2 && j == 1) || (i == 2 && j == 2)) continue;
			if (now_date_str[i][j] <10 && i != 2)
			{
				myGLCD.print("0", x_poz + 48 * j, y_poz + 45 * i);
				myGLCD.print(String(now_date_str[i][j]), x_poz + 16 + 48*j, y_poz + 45 * i);
			}
			if (now_date_str[i][j] >= 10 && i != 2)
			{
				myGLCD.print(String(now_date_str[i][j]), x_poz +48*j, y_poz + 45 * i);
			}
			myGLCD.print(String(now_date_simb[i][j]), x_poz + 32 + 48 * j, y_poz + 45 * i);
			if (i == 2 && j == 0)
			{
				myGLCD.print(dayoftheweek[now_date_str[i][j]][1], x_poz + 48 * j, y_poz + 45 * i);
			}
		}
	}
}
void SettingsTime()
{
	int x_poz = 255;
	int y_poz = 72;
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.setColor(255, 255, 255);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 0 && j == 2) continue;
			if ((i == 2 && j == 1) || (i == 2 && j == 2)) continue;
			if (now_date_str[i][j] <10 && i != 2)
			{
				myGLCD.print("0", x_poz + 48 * j, y_poz + 45 * i);
				myGLCD.print(String(now_date_str[i][j]), x_poz + 16 + 48 * j, y_poz + 45 * i);
			}
			if (now_date_str[i][j] >= 10 && i != 2)
			{
				myGLCD.print(String(now_date_str[i][j]), x_poz + 48 * j, y_poz + 45 * i);
			}
			if (i == 2 && j == 0)
			{
				myGLCD.print(dayoftheweek[now_date_str[i][j]][1], x_poz + 48 * j, y_poz + 45 * i);
			}
		}
	}
}
void Time_Edit()
{
	int x_poz = 255;
	int y_poz = 72;
	if (current_stick_poz_y != stick_poz_y || current_stick_poz_x != stick_poz_x)
	{
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
		if (stick_poz_x == 0 && SettingsTime_Edit && hour1)	stick_poz_y = stick_poz_y = now_date_str[0][0], hour1 = false, min1 = true;
		if (stick_poz_x == 1 && SettingsTime_Edit && min1)	stick_poz_y = stick_poz_y = now_date_str[0][1], min1 = false, hour1 = true;
		if (stick_poz_x == 0 && SettingsTime_Edit)			now_date_str[0][0] = stick_poz_y;
		if (stick_poz_x == 1 && SettingsTime_Edit)			now_date_str[0][1] = stick_poz_y;
		myGLCD.setBackColor(0, 0, 0);
		for (int i = 0; i < 2; i++)
		{
			if (i == stick_poz_x) myGLCD.setColor(255, 0, 0);
			if (i != stick_poz_x) myGLCD.setColor(255, 255, 255);
			if (now_date_str[0][i] <10)
			{
				myGLCD.print("0", x_poz + 48 * i, y_poz);
				myGLCD.print(String(now_date_str[0][i]), x_poz + 16 + 48 * i, y_poz);
			}
			if (now_date_str[0][i] >= 10)
			{
				myGLCD.print(String(now_date_str[0][i]), x_poz + 48 * i, y_poz);
			}
		}
	}
}
void Date_Edit()
{
	int x_poz = 255;
	int y_poz = 72;
	if (current_stick_poz_y != stick_poz_y || current_stick_poz_x != stick_poz_x)
	{
		current_stick_poz_y = stick_poz_y;
		current_stick_poz_x = stick_poz_x;
		if (stick_poz_x == 0 && SettingsDate_Edit && date1)		stick_poz_y = now_date_str[1][0], date1 = false, month1 = true, year1 = false;
		if (stick_poz_x == 1 && SettingsDate_Edit && month1)	stick_poz_y = now_date_str[1][1], date1 = false, month1 = false, year1 = true;
		if (stick_poz_x == 2 && SettingsDate_Edit && year1)		stick_poz_y = now_date_str[1][2], date1 = true, month1 = false, year1 = false;
		if (stick_poz_x == 0 && SettingsDate_Edit)				stick_poz_y = now_date_str[1][0] = stick_poz_y;
		if (stick_poz_x == 1 && SettingsDate_Edit)				stick_poz_y = now_date_str[1][1] = stick_poz_y;
		if (stick_poz_x == 2 && SettingsDate_Edit)				stick_poz_y = now_date_str[1][2] = stick_poz_y;
		myGLCD.setBackColor(0, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			if (i == stick_poz_x) myGLCD.setColor(255, 0, 0);
			if (i != stick_poz_x) myGLCD.setColor(255, 255, 255);
			if (now_date_str[1][i] <10)
			{
				myGLCD.print("0", x_poz + 48 * i, y_poz + 45);
				myGLCD.print(String(now_date_str[1][i]), x_poz + 16 + 48 * i, y_poz + 45);
			}
			if (now_date_str[1][i] >= 10)
			{
				myGLCD.print(String(now_date_str[1][i]), x_poz + 48 * i, y_poz + 45);
			}
		}
	}
}
void DOW_edit(){
	int x_poz = 255;
	int y_poz = 72;
	if (current_stick_poz_y != stick_poz_y)
	{
		myGLCD.setColor(0, 0, 0);
		myGLCD.print(dayoftheweek[current_stick_poz_y][1], x_poz, y_poz + 90);
		current_stick_poz_y = stick_poz_y;
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.setColor(255, 0, 0);
		myGLCD.print(dayoftheweek[stick_poz_y][1], x_poz, y_poz + 90);
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
      if (MainScreen_Mode)
      {
		  myGLCD.setColor(245, 245, 245);
		  myGLCD.drawRect(x_position[0][current_stick_poz_x],y_position[0][current_stick_poz_y],x_position[1][current_stick_poz_x],y_position[1][current_stick_poz_y]);
		  myGLCD.setColor(0, 0, 0);
		  myGLCD.drawRect(x_position[0][current_stick_poz_x]+1,y_position[0][current_stick_poz_y]+1,x_position[1][current_stick_poz_x]-1,y_position[1][current_stick_poz_y]-1);
        //relay_state_all[stick_poz_x - 1][stick_poz_y] = !relay_state_all[stick_poz_x - 1][stick_poz_y];
        //shiftWrite();
      }
	  //Выходим из виджета Освещение на главный экран
	  if (light_widget_mode)
	  {
		  MainScreen_Mode = true;
		  light_widget_mode = false;
		  myGLCD.setColor(0, 0, 0);
		  myGLCD.drawRect(172 + 17 * stick_poz_x, 84, 172 + 15 + 17 * stick_poz_x, 99);
		  stick_poz_x = 1;
		  stick_poz_y = 0;
		  current_stick_poz_x = 0;
		  current_stick_poz_y = stick_poz_y;
		  goto button_press_exit;
	  }
	  //Выходим из виджета Розетки на Главный экран
	  if (power_widget_mode)
	  {
		  MainScreen_Mode = true;
		  power_widget_mode = false;
		  myGLCD.setColor(0, 0, 0);
		  myGLCD.drawRect(331 + 17 * stick_poz_x, 84, 331 + 15 + 17 * stick_poz_x, 99);
		  stick_poz_x = 2;
		  stick_poz_y = 0;
		  current_stick_poz_x = 0;
		  current_stick_poz_y = stick_poz_y;
		  goto button_press_exit;  
	  }
	  //Выходим из Меню настроек на Главный экран
	  if (stick_poz_x==1 && stick_poz_y==4 && SettingScreen_Mode)
	  {
		  SettingScreen_Mode = false;
		  MainScreen_Mode = true;
		  MainScreen_draw();
		  relay_work();
		  stick_poz_x = 0;
		  stick_poz_y = 0;
		  current_stick_poz_x = 1;
		  current_stick_poz_y = 1;
	  }
	  //Выходим из Настроек Времени в Меню Настроек
	  if (stick_poz_x == 1 && stick_poz_y == 4 && SettingsTime_Mode)
	  {
		  current_stick_poz_x, current_stick_poz_y = 1;
		  SettingsTime_Mode = false;
		  SettingScreen_Mode = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
		  SettingsMenu_draw();
	  }
	  //Переходим на виджет Освещение
	  if (stick_poz_x == 1 && stick_poz_y == 0 && MainScreen_Mode)
	  {
		  MainScreen_Mode = false;
		  light_widget_mode = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
	  }
	  //Переходим на виджет Розетки
	  if (stick_poz_x == 2 && stick_poz_y == 0 && MainScreen_Mode)
	  {
		  MainScreen_Mode = false;
		  power_widget_mode = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
	  }
	  //Переход на экран настроек
	  if (stick_poz_x == 0 && stick_poz_y == 3 && MainScreen_Mode)
	  {
		  MainScreen_Mode = false;
		  SettingScreen_Mode = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
		  SettingsMenu_draw();
	  }
	  //Подтверждаем введенную Дату
	  if (SettingsDate_Edit)
	  {
		  SettingsDate_Edit = false;
		  SettingsTime_Mode = true;
		  stick_poz_x = 1;
		  stick_poz_y = 1;
		  current_stick_poz_x = 1;
		  current_stick_poz_y = 4;
		  rtc.setDate(now_date_str[1][0], now_date_str[1][1], now_date_str[1][2]);
		  time_changed = true;
		  SettingsTime();
		  goto button_press_exit;
	  }
	  //Подтверждаем введенное время
	  if (SettingsTime_Edit)
	  {
		  SettingsTime_Edit = false;
		  SettingsTime_Mode = true;
		  stick_poz_x = 1;
		  stick_poz_y = 0;
		  current_stick_poz_x = 1;
		  current_stick_poz_y = 4;
		  rtc.setTime(now_date_str[0][0], now_date_str[0][1], t.sec);
		  //time_changed = true;
		  SettingsTime();
		  goto button_press_exit;
	  }
	  //Подтверждаем введенный День Недели
	  if (SettingsDOW_Edit)
	  {
		  SettingsDOW_Edit = false;
		  SettingsTime_Mode = true;
		  rtc.setDOW(stick_poz_y);
		  now_date_str[2][0] = stick_poz_y;
		  stick_poz_x = 1;
		  stick_poz_y = 2;
		  current_stick_poz_x = 1;
		  current_stick_poz_y = 4;
		  //time_changed = true;
		  SettingsTime();
		  goto button_press_exit;
	  }
	  //Редактируем время
	  if (stick_poz_y == 0 && SettingsTime_Mode)
	  {
		  SettingsTime_Mode = false;
		  SettingsTime_Edit = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
		  current_stick_poz_y = 99;
		  hour1 = true;
	  }
	  //Редактируем дату
	  if (stick_poz_y == 1 && SettingsTime_Mode)
	  {
		  SettingsTime_Mode = false;
		  SettingsDate_Edit = true;
		  stick_poz_x = 0;
		  stick_poz_y = 0;
		  current_stick_poz_y = 99;
		  date1 = true;
	  }
	  //Редактируем День Недели
	  if (stick_poz_y == 2 && SettingsTime_Mode)
	  {
		  SettingsTime_Mode = false;
		  SettingsDOW_Edit = true;
		  current_stick_poz_y = stick_poz_y + 1;
		  stick_poz_y = now_date_str[2][0];
	  }
	  //Настройки Даты и Времени
	  if (current_stick_poz_x == 0 && current_stick_poz_y == 0 && SettingScreen_Mode)
	  {
		  SettingScreen_Mode = false;
		  SettingsTime_Mode = true;
		  SettingsTime_draw();
		  stick_poz_x = 1;
		  stick_poz_y = 0;
	  }
      //boolean r_registr = bitRead(relay_state_1, stick_poz_y);
      // bitWrite(relay_state_1, stick_poz_y, !r_registr);
      //shiftWrite();
    }
  }
  button_press_exit:
  // запоминаем последнее состояние кнопки для новой итерации
  buttonWasUp = buttonIsUp;
  // запоминаем последнее состояние кнопки для новой итерации
  value_X = analogRead(axis_X);    // Считываем аналоговое значение оси Х
  int num_x_poz;							//Переменная хранит количество столбцов в текущем Виджете
  int x_centr;
  if (MainScreen) num_x_poz = numofcolls, x_centr = 0;	//Количество столбцов на Главном Экране
  if (SettingScreen_Mode) num_x_poz = 2, x_centr = 0;
  if (light_widget_mode) num_x_poz = 8, x_centr = 0;		//Количество столбцов в виджете Освещения
  if (power_widget_mode) num_x_poz = 8, x_centr = 0;		//Количество столбцов в виджете Розеток
  if (SettingsTime_Mode) num_x_poz = 2, x_centr = 1;		//Кодичество столбцов в Настройках Времени и Даты
  if (SettingsTime_Edit) num_x_poz = 2, x_centr = 0;		//Кодичество столбцов в Настройках Времени (Часы и Минуты)
  if (SettingsDate_Edit) num_x_poz = 3, x_centr = 0;		//Кодичество столбцов в Настройках Даты (День, Месяц, Год)
  if (SettingsDOW_Edit) num_x_poz = 1, x_centr = 0;		//Кодичество столбцов в Настройках Дня Недели
  if (value_X > 700 && stick_release_X)
  {
    time_X = millis();
    stick_release_X = !stick_release_X;
	if (stick_poz_x >= num_x_poz - 1) stick_poz_x = x_centr;
    else stick_poz_x = stick_poz_x + 1;
  }
  if (value_X < 400 && stick_release_X)
  {
    time_X = millis();
    stick_release_X = !stick_release_X;
	if (stick_poz_x <= x_centr) stick_poz_x = num_x_poz - 1;
    else stick_poz_x = stick_poz_x - 1;
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

  value_Y = analogRead(axis_Y);    // Считываем аналоговое значение оси Y
  int num_y_poz;
  int y_centr;
  if (MainScreen) num_y_poz = numofrows, y_centr = 0;
  if (SettingScreen_Mode) num_y_poz = 5, y_centr = 0;
  if (SettingsTime_Mode) num_y_poz = 5, y_centr = 0;
  if (SettingsTime_Edit && stick_poz_x == 0)	num_y_poz = 24, y_centr = 0;			//Количество часов по оси Y
  if (SettingsTime_Edit && stick_poz_x == 1)	num_y_poz = 60, y_centr = 0;			//Количество минут по оси Y 
  if (SettingsDate_Edit && stick_poz_x == 0)	num_y_poz = 32, y_centr = 1;			//Количество Дней по оси Y
  if (SettingsDate_Edit && stick_poz_x == 1)	num_y_poz = 13, y_centr = 1;			//Количество Месяцев по оси Y
  if (SettingsDate_Edit && stick_poz_x == 2)	num_y_poz = 2100, y_centr = 2016;		//Количество Лет по оси
  if (SettingsDOW_Edit)							num_y_poz = 8, y_centr = 1;				//Количество Дней Недели по оси
  if (value_Y > 700 && stick_release_Y)
  {
    time_Y = millis();
    stick_release_Y = !stick_release_Y;
	if (stick_poz_y >= num_y_poz - 1) stick_poz_y = y_centr;
    else stick_poz_y = stick_poz_y + 1;
  }
  if (value_Y < 400 && stick_release_Y)
  {
    time_Y = millis();
    stick_release_Y = !stick_release_Y;
    if (stick_poz_y <= y_centr) stick_poz_y = num_y_poz-1;
    else stick_poz_y = stick_poz_y - 1;
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
