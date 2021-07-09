#include <Arduino.h>
#include <TM1638plus.h>
#include <PingPongMatch.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <string.h>
#include <Config.h>
#include <CaptiveRequestHandler.h>
#include <WifiConfigPersistor.h>
#include <DNSServer.h>

const uint8_t strobeTm = D1; // strobe = GPIO connected to strobe line of module
const uint8_t clockTm = D2;  // clock = GPIO connected to clock line of module
const uint8_t dioTm = D5;    // data = GPIO connected to data line of module
const bool highFreq = false; //default false,, If using a high freq CPU > ~100 MHZ set to true.

TM1638plus tm(strobeTm, clockTm, dioTm, highFreq);

unsigned long lastButtonPressTime = millis();
unsigned long debounceTimeMs = 200;

const uint8_t pointsToWin = 21;
const uint8_t servingsForEachPlayer = 5;
const uint8_t differenceToWin = 2;

uint8_t lastButtonPressed = 0;

const uint8_t addPointToPlayerLeftValue = 1;
const uint8_t changeSidesValue = 2;
const uint8_t startMatchValue = 12;
const uint8_t undoLastPointValue = 16;
const uint8_t showIPValue = 32;
const uint8_t changeServingSideValue = 64;
const uint8_t addPointToPlayerRightValue = 128;

/* buttons contains a byte with values of button s8s7s6s5s4s3s2s1
  HEX  :  Switch no : Binary
  0x01 : S1 Pressed  0000 0001   //add point to player left
  0x02 : S2 Pressed  0000 0010   //change sides (points and servings)
  0x04 : S3 Pressed  0000 0100   //start match (both pressed)
  0x08 : S4 Pressed  0000 1000   //start match 
  0x10 : S5 Pressed  0001 0000   //undo last point
  0x20 : S6 Pressed  0010 0000   //show IP
  0x40 : S7 Pressed  0100 0000   //change servings only
  0x80 : S8 Pressed  1000 0000   //add point to player right
*/
PingPongMatch match(pointsToWin, servingsForEachPlayer, differenceToWin);

PlayerSide weHaveAWinner = PlayerSide::Undefined;
char winPhrase[] = "    Gana";

void HappyBirthdayPaul();
void ClearDisplay();
void RefreshDisplayValues();
void DisplayAlternativeUntilRestart(char *, uint8_t, uint8_t, long);
void StartMatch();
void RefreshServings();
void SetupCaptivePortalMode(const char *portalName);
void ConnectToWifi();
void ConfigureServerEndpoints();
void ScrollIPDuringMs(unsigned long ms);

String valueToReplace;
String processor(const String &var);

const char *filename = "/config.txt";
WifiConfigPersistor wifiConfigPersistor(filename);
AsyncWebServer server(80);
DNSServer dnsServer;
bool serverOn = false;

void setup()
{
  Serial.begin(115200);

  if (!LittleFS.begin())
  {
    Serial.println(F("An Error has occurred while mounting LittleFS"));
    return;
  }

  tm.displayBegin();
  tm.reset();

  HappyBirthdayPaul();

  uint8_t buttons = tm.readButtons();
  //The captive portal will be set only if at least one button is pressed when booting
  //after the config file is saved, the esp will be restarted automatically
  if (buttons != 0)
  {
    SetupCaptivePortalMode("esp-captive");
    while (true)
    {
      //The only way to get out of this will be a manual reset OR to fill in the credentials (it will reboot automatically)
      yield();
      dnsServer.processNextRequest();
    }
  }

  ConnectToWifi();

  StartMatch();
  RefreshDisplayValues();
  RefreshServings();
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

  if (buttons == showIPValue)
  {
    ScrollIPDuringMs(6000);
  }
  if (buttons == addPointToPlayerLeftValue)
  {
    match.AddPointTo(PlayerSide::Left);
    if (match.IsWinner(PlayerSide::Left))
    {
      weHaveAWinner = PlayerSide::Left;
      strcpy(winPhrase, "Gana    ");
    }
  }
  if (buttons == addPointToPlayerRightValue)
  {
    match.AddPointTo(PlayerSide::Right);
    if (match.IsWinner(PlayerSide::Right))
    {
      weHaveAWinner = PlayerSide::Right;
      strcpy(winPhrase, "    Gana");
    }
  }

  if (buttons == startMatchValue)
  {
    StartMatch();
  }

  if (buttons == undoLastPointValue)
  {
    match.UndoLastPoint();
  }

  if (buttons == changeSidesValue)
  {
    match.ChangeSides();
  }

  if (buttons == changeServingSideValue)
  {
    match.ChangeServingSidesOnly();
  }

  if (weHaveAWinner != PlayerSide::Undefined)
  {
    DisplayAlternativeUntilRestart(winPhrase, match.GetPoints(PlayerSide::Left), match.GetPoints(PlayerSide::Right), 1500);
  }
  else
  {
    RefreshDisplayValues();
  }
  RefreshServings();
}

void ScrollIPDuringMs(unsigned long ms)
{
  unsigned long previousMillis_display = 0;
  const long interval_display = 1000;
  char textScroll[18];
  strcpy(textScroll, WiFi.localIP().toString().c_str());
  unsigned long showIPStartTime = millis();
  tm.displayText(textScroll);
  while (millis() - showIPStartTime < 1000)
  {
    yield();
  }
  showIPStartTime = millis();
  while (millis() - showIPStartTime < ms - 1000)
  {
    yield();
    tm.displayText(textScroll);
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis_display >= interval_display)
    {
      previousMillis_display = currentMillis;
      if (strlen(textScroll) > 0)
      {
        memmove(textScroll, textScroll + 1, strlen(textScroll));
        tm.displayText("        "); //Clear display or last character will drag across screen
      }
      else
      {
        return;
      }
    }
  }
}

void ConnectToWifi()
{
  WiFi.mode(WIFI_STA);
  Config wifiConfig = wifiConfigPersistor.Retrieve();
  WiFi.begin(wifiConfig.ssid, wifiConfig.pass);
  Serial.println("");
  Serial.print("trying to connect with credentials: ");
  Serial.print(wifiConfig.ssid);
  Serial.print(" ");
  Serial.println(wifiConfig.pass);

  tm.displayText("WiFi con");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!\n");
    ClearDisplay();
    tm.displayText("WIFI BAD");
    delay(2000);
  }
  else
  {
    ClearDisplay();

    ConfigureServerEndpoints();

    server.begin();
    tm.displayText("WIFI");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    delay(2000);
  }
  ClearDisplay();
}

void ConfigureServerEndpoints()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println(request->url());
              request->send(LittleFS, "/dashboard.html", String(), false, processor);
            });

  server.on("/bootstrap.css", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println(request->url());
              request->send(LittleFS, "/bootstrap.css", "text/css");
            });
}

void SetupCaptivePortalMode(const char *portalName)
{
  WiFi.softAP(portalName);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler(&wifiConfigPersistor)).setFilter(ON_AP_FILTER); //only when requested from AP
  tm.displayText(WiFi.softAPIP().toString().c_str());
  server.begin();
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
      tm.setLED(i, 7 - i < number);
    }
  }
}

void StartMatch()
{
  match.Start();
  weHaveAWinner = PlayerSide::Undefined;
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

  char valuesUpper[9];
  char valuesLower[5];
  snprintf(valuesUpper, 5, "% 4d", match.GetPoints(PlayerSide::Left));
  snprintf(valuesLower, 5, "% 4d", match.GetPoints(PlayerSide::Right));
  strcat(valuesUpper, valuesLower);
  tm.displayText(valuesUpper);
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

String processor(const String &var)
{
  if (var == "POINTS_LEFT")
  {
    return String(match.GetPoints(PlayerSide::Left));
  }
  if (var == "POINTS_RIGHT")
  {
    return String(match.GetPoints(PlayerSide::Right));
  }
  if ((var == "WINNER_LEFT" && weHaveAWinner == PlayerSide::Left) ||
      (var == "WINNER_RIGHT" && weHaveAWinner == PlayerSide::Right))
  {
    return String("WINNER!!");
  }
  return String();
}
