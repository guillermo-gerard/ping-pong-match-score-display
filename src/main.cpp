#include <Arduino.h>
#include <TM1638plus.h>
#include <PingPongMatch.h>

#define STROBE_TM D1    // strobe = GPIO connected to strobe line of module
#define CLOCK_TM D2     // clock = GPIO connected to clock line of module
#define DIO_TM D5       // data = GPIO connected to data line of module
const bool high_freq = false; //default false,, If using a high freq CPU > ~100 MHZ set to true.

TM1638plus tm(STROBE_TM, CLOCK_TM, DIO_TM, high_freq);

unsigned long lastButtonPressTime = millis();
unsigned long debounceTimeMs = 200;

const uint8_t pointsToWin = 21;
const uint8_t servingsForEachPlayer = 5;
const uint8_t differenceToWin = 2;

uint8_t lastButtonPressed = 0;

const uint8_t addPointToPlayerLeftValue = 1;
const uint8_t addPointToPlayerRightValue = 128;
const uint8_t startMatchValue = 12;
const uint8_t undoLastPointValue = 48;
/* buttons contains a byte with values of button s8s7s6s5s4s3s2s1
  HEX  :  Switch no : Binary
  0x01 : S1 Pressed  0000 0001   //add point to player left
  0x02 : S2 Pressed  0000 0010   //
  0x04 : S3 Pressed  0000 0100   //start match (both pressed)
  0x08 : S4 Pressed  0000 1000   //start match 
  0x10 : S5 Pressed  0001 0000   //undo last point (both pressed)
  0x20 : S6 Pressed  0010 0000   //undo last point
  0x40 : S7 Pressed  0100 0000   //
  0x80 : S8 Pressed  1000 0000   //add point to player right
*/
PingPongMatch match(pointsToWin, servingsForEachPlayer, differenceToWin);

bool weHaveAWinner = false;
char *winPhrase = "    Gana";

void HappyBirthdayPaul();
void ClearDisplay();
void RefreshDisplayValues();
void DisplayAlternativeUntilRestart(char *, uint8_t, uint8_t, long);
void StartMatch();
void RefreshServings();

void setup()
{
  Serial.begin(115200);

  tm.displayBegin();
  tm.reset();

  HappyBirthdayPaul();
}

void loop()
{

  if (lastButtonPressTime - millis() < debounceTimeMs)
  {
    return;
  }

  uint8_t buttons = tm.readButtons();
  //the button(s) has not been released yet
  if (buttons == lastButtonPressed)
  {
    return;
  }

  lastButtonPressed = buttons;
  lastButtonPressTime = millis();

  if (buttons == addPointToPlayerLeftValue)
  {
    match.AddPointTo(PlayerSide::Left);
    if (match.IsWinner(PlayerSide::Left))
    {
      weHaveAWinner = true;
      winPhrase = "Gana    ";
    }
  }
  if (buttons == addPointToPlayerRightValue)
  {
    match.AddPointTo(PlayerSide::Right);
    if (match.IsWinner(PlayerSide::Right))
    {
      weHaveAWinner = true;
      winPhrase = "    Gana";
    }
  }
  if (buttons == startMatchValue)
  {
    StartMatch();
  }
  if (buttons == undoLastPointValue)
  {
    //todo
  }

  if (weHaveAWinner)
  {
    DisplayAlternativeUntilRestart(winPhrase, match.GetPoints(PlayerSide::Left), match.GetPoints(PlayerSide::Right), 1500);
  }
  else
  {
    RefreshDisplayValues();
  }
  RefreshServings();
}

void RefreshServings()
{
  uint8_t number = match.GetServingNumber();
  if (match.GetServingSide() == PlayerSide::Left)
  {
    for (int i = 0; i < 8; i++)
    {
      tm.setLED(i, i < number);
    }
  }
  else
  {
    for (int i = 7; i >= 0; i--)
    {
      tm.setLED(i, 7-i < number);
    }
  }
}

void StartMatch()
{
  match.Start();
  weHaveAWinner = false;
}

void DisplayAlternativeUntilRestart(char *winPhrase, uint8_t leftPoints, uint8_t rightPoints, long delayPeriod)
{
  while (tm.readButtons() != startMatchValue)
  {
    ClearDisplay();
    tm.displayText(winPhrase);
    delay(delayPeriod);
    RefreshDisplayValues();
    delay(delayPeriod);
  }
  StartMatch();
}

void RefreshDisplayValues()
{
  ClearDisplay();
  tm.DisplayDecNumNibble(match.GetPoints(PlayerSide::Left), match.GetPoints(PlayerSide::Right), true);
}

void ClearDisplay()
{
  tm.displayText("        ");
}

void HappyBirthdayPaul()
{
  ClearDisplay();
  tm.displayText("FELIZ");
  delay(2000);
  ClearDisplay();
  tm.displayText("CUMPLE");
  delay(2000);
  ClearDisplay();
  tm.displayText("PAUL");
  delay(4000);
}