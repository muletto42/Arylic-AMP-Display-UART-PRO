/* Arylic-AMP-Display-UART-PRO
 * 0,91" OLED-Anzeige für Arylic AMP-Geräte
 * Schnittstelle zu Arylic Amp PRO v4
 * Version: 2.1.6
 * Letztes Update: 02/08/2023 19.07
 */

 #include <Wire.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_SSD1306.h>
 #include <HardwareSerial.h>
 
 // OLED-Konfiguration (Ändere dies, wenn du ein anderes Display verwendest)
 #define SCREEN_WIDTH 128       // OLED-Bildschirmbreite in Pixel
 #define SCREEN_HEIGHT 32       // OLED-Bildschirmhöhe in Pixel
 #define SCREEN_ADDR 0x3C       // 7-Bit-I2C-Adresse des Displays
 
 // Zuordnung der seriellen Kommunikation zwischen AMP > ESP > OLED (Ändere bei Bedarf die Pins)
 #define SDA_PIN 21             // SDA-Datenpin für SSD1306 Display
 #define SCL_PIN 22             // SCL-Taktpin für SSD1306 Display
 #define RX_PIN 16              // Serial2 RX mit Amp TX verbinden
 #define TX_PIN 17              // Serial2 TX mit Amp RX verbinden
 #define LED_PIN 12             // Steuerpin für hinterleuchteten VU-Meter
 
 // Anzeige-Modi (dispMode)
 #define SOURCE 1
 #define VOLUME 2
 #define CHN 3
 #define BAS 4   
 #define TRE 5
 #define BLANK 6
 
 // GLOBALE VARIABLEN FÜR PROGRAMMEINSTELLUNGEN
 byte dispMode = SOURCE;
 byte prevdispMode = 1;
 unsigned long dispModeTemp_timer = 0;
 unsigned long currentMillis = 0;
 unsigned long standbyBlinkInterval = 2000;         // Blinkintervall für STANDBY (Millisekunden)
 const unsigned long DISP_UPDATE_INTERVAL = 1000;   // Intervall für Anzeigeaktualisierung zwischen den Modi
 bool dispModeTempSource = true;
 
 // Variablen zur Verzögerung für MUTE-Befehl MUT:1; abhängig vom Eingang
 const unsigned long MUTE_DELAY = 700;      // Verzögerung in Millisekunden (anpassbar)
 bool muteMode = false;                     // Speichert den MUTE-Zustand
 bool receivedMuteOnCommand = false;
 unsigned long muteOnTime = 0;
 
 // Variablen zur Speicherung empfangener Daten und UART-Zustände
 bool initialDataReceived = false;         // Marker für Erhalt der ersten STA-Daten
 bool bluetoothConnected = false;          // Status der Bluetooth-Verbindung
 bool virtualBassEnabled = false;          // Status von Virtual Bass
 bool beepEnabled = false;                 // Status des Signaltons
 bool standbyMode = false;                 // Status des STANDBY-Modus
 int dispBass = 0;
 int dispTreble = 0;
 int dispVolume = 0;
 String dispChannel = "";
 String dispSource = "x_x!"; // merkt sich den vorherigen Source-Status vor dem Loop-Start
 
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Display-Objekt erstellen
 HardwareSerial uart(2);   // Verwendung von Hardware Serial2
 
 /*---------------------------------------------------------------------------------------------------
                      Funktion zur Verarbeitung empfangener UART-Kommandos
 ---------------------------------------------------------------------------------------------------*/
 void processUARTCommand(const String& commandType, const String& commandValue) {
    // Logik zum Verarbeiten der UART-Kommandos
    // und Aktualisieren der Anzeigewerte entsprechend
    
   if (commandType == "SRC") {
     // Zuordnung empfangener Werte zu Ausgabetexten
     if (commandValue == "BT;") {
         dispSource = "BT";
     } else if (commandValue == "LINE-IN;") {
         dispSource = "LINE";
     } else if (commandValue == "USB;") {
         dispSource = "USB";
     } else if (commandValue == "NET;") {
         dispSource = "WIFI";
     }
     showSource();
 
   } else if (commandType == "VOL") {
     dispVolume = commandValue.toInt();
     if (dispVolume == 100) {
       showNumberParamTwo("VOLUME", "MAX");
     } else {
       showNumberParam("Volume", dispVolume);
     }
 
   } else if (commandType == "BAS") {
     dispBass = commandValue.toInt();
     showNumberParam("Bass", dispBass);
 
   } else if (commandType == "TRE") {
     dispTreble = commandValue.toInt();
     showNumberParam("Treble", dispTreble);
 
   } else if (commandType == "CHN") {
     if (commandValue == "L;") {
         dispChannel = "LEFT";
     } else if (commandValue == "R;") {
         dispChannel = "RIGHT";
     } else if (commandValue == "S;") {
         dispChannel = "STEREO";
     } else {
         dispChannel = "Unknown";
     }
     showNumberParamTwo("CHN", dispChannel);
 
   } else if (commandType == "LED") {
     if (commandValue == "1;") {
       showNotification("LED ON");
       digitalWrite(LED_PIN, HIGH);
       Serial.println("LED eingeschaltet");
     } else if (commandValue == "0;") {
       showNotification("LED OFF");
       digitalWrite(LED_PIN, LOW);
       Serial.println("LED ausgeschaltet");
     }
 
   } else if (commandType == "BTC") {
     if (commandValue == "1;") {
       bluetoothConnected = true;
       showNotification("Connected");
     } else if (commandValue == "0;") {
       bluetoothConnected = false;
       showNotification("Disconnect");
     }
 
   } else if (commandType == "VBS") {
     if (commandValue == "1;") {
       virtualBassEnabled = true;
       showNotification("VBS ON");
     } else if (commandValue == "0;") {
       virtualBassEnabled = false;
       showNotification("VBS OFF");
     }
 
   } else if (commandType == "BEP") {
     if (commandValue == "1;") {
       beepEnabled = true;
       showNotification("BEEP ON");
     } else if (commandValue == "0;") {
       beepEnabled = false;
       showNotification("BEEP OFF");
     }
   }
 
   // STANDBY-Zustand anzeigen
   if (commandType == "SYS" && commandValue == "STANDBY;") {
     standbyMode = true;
     showStandby();
   }
 
   // MUTE-Verarbeitung mit Verzögerung
   if (commandType == "MUT" && commandValue == "1;") {
       muteMode = true;
       receivedMuteOnCommand = true;
       muteOnTime = millis();
   } else if (commandType == "MUT" && commandValue == "0;") {
       muteMode = false;
       receivedMuteOnCommand = false;
       if (receivedMuteOnCommand) {
           showMute();
           receivedMuteOnCommand = false;
       } else {
           showSource();
       }
   }
 }
 
 // Funktion zum Umschalten auf den nächsten Anzeige-Modus
 void switchDisplayMode() {
   dispMode++;
   if (dispMode > BLANK) {
     dispMode = SOURCE;
   }
 }
 
 /*---------------------------------------------------------------------------------------------------
                                     Setup-Konfiguration
 ---------------------------------------------------------------------------------------------------*/
 void setup() {
   Serial.begin(115200);
   Serial.println("Startet...");
 
   // UART-Verbindung zum AMP initialisieren
   uart.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
   uart.setTimeout(2000);
 
   // Erste Initialdaten auslesen, z. B. Source-Zustand
   while (!initialDataReceived) {
     if (uart.available() > 0) {
       String receivedData = uart.readStringUntil('\n');
       receivedData.trim();
 
       if (receivedData.startsWith("STA:")) {
         String commandValue = receivedData.substring(4);
 
         if (commandValue.startsWith("BT")) {
           dispSource = "BT";
         } else if (commandValue.startsWith("LINE-IN")) {
           dispSource = "LINE";
         } else if (commandValue.startsWith("USB")) {
           dispSource = "USB";
         } else if (commandValue.startsWith("NET")) {
           dispSource = "WIFI";
         }
         initialDataReceived = true;
       }
     }
   }
 
   // Pins initialisieren
   pinMode(LED_PIN, OUTPUT);
 
   // OLED-Display starten
   display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR);
   display.clearDisplay();
 
   display.setTextSize(2);
   display.setTextColor(SSD1306_WHITE);
   display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
   display.println("Starting..");
   display.display();
   delay(2000);
   display.clearDisplay();
 
   Serial.println("Initialisierung abgeschlossen...");
   uart.flush();
 }
 
 /*---------------------------------------------------------------------------------------------------
                                    Hauptprogrammschleife
 ---------------------------------------------------------------------------------------------------*/
 void loop() {
   // UART-Daten lesen
   if (uart.available() > 0) {
     String receivedData = uart.readStringUntil('\n');
     receivedData.trim();
     Serial.println("Empfangen: " + receivedData);
 
     int separatorIndex = receivedData.indexOf(':');
     if (separatorIndex > 0 && separatorIndex < receivedData.length() - 1) {
         String commandType = receivedData.substring(0, separatorIndex);
         String commandValue = receivedData.substring(separatorIndex + 1);
 
         processUARTCommand(commandType, commandValue);
     }
   }
 
   if (!initialDataReceived) return;
 
   if (dispMode != prevdispMode) {
     switchDisplayMode();
     prevdispMode = dispMode;
   }
 
   currentMillis = millis();
 
   if (standbyMode) {
     showStandby();
     if (uart.available() > 0) {
       String receivedData = uart.readStringUntil('\n');
       receivedData.trim();
       if (receivedData.startsWith("STA:")) {
         standbyMode = false;
         dispMode = SOURCE;
       }
     }
   } else if (muteMode) {
     if (receivedMuteOnCommand && (millis() - muteOnTime) >= MUTE_DELAY) {
       showMute();
       receivedMuteOnCommand = false;
     }
   } else {
     if (dispMode != prevdispMode || currentMillis > (dispModeTemp_timer + DISP_UPDATE_INTERVAL)) {
       switch (dispMode) {
         case SOURCE:
           showSource();
           break;
         case VOLUME:
           showNumberParam("Volume", dispVolume);
           break;
         case CHN:
           showNumberParamTwo("Channel", dispChannel);
           break;
         case BAS:
           showNumberParam("Bass", dispBass);
           break;
         case TRE:
           showNumberParam("Treble", dispTreble);
           break;
         default:
           display.clearDisplay();
           display.display();
           break;
       }
       prevdispMode = dispMode;
       dispModeTemp_timer = currentMillis;
       dispModeTempSource = false;
     }
   }
 }
 
 /*---------------------------------------------------------------------------------------------------
                        Funktionen zur Darstellung auf dem OLED-Display
 ---------------------------------------------------------------------------------------------------*/
 void showNumberParamTwo(String parmName, String parmValue) {
   if (!dispModeTempSource) {
     display.clearDisplay();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.invertDisplay(false);
     display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
     display.print(parmName + ":" + parmValue);
     display.display();
     dispModeTemp_timer = millis();
     delay(500);
     if (muteMode) showMute();
   }
 }
 
 void showNumberParam(String parmName, int parmValue) {
   if (!dispModeTempSource) {
     display.clearDisplay();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.invertDisplay(false);
     display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
     display.print(parmName + ":" + parmValue);
     display.display();
     dispModeTemp_timer = millis();
     delay(500);
     if (muteMode) showMute();
   }
 }
 
 void showSource() {
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
   display.print("Mode:" + String(dispSource));
   display.display();
   dispModeTemp_timer = millis();
   dispModeTempSource = true;
 }
 
 void showNotification(const String& message) {
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
   display.println(message);
   display.display();
   delay(2000);
   if (muteMode) {
     showMute();
   } else {
     showSource();
   }
 }
 
 // Anzeige für STANDBY-Modus mit Blinken
 void showStandby() {
   static unsigned long prevBlinkMillis = 0;
   static bool displayStandby = true;
 
   unsigned long currentMillis = millis();
 
   if (currentMillis - prevBlinkMillis >= standbyBlinkInterval) {
     displayStandby = !displayStandby;
     prevBlinkMillis = currentMillis;
   }
 
   if (displayStandby) {
     display.clearDisplay();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
     display.print("  STANDBY ");
     display.display();
   } else {
     display.clearDisplay();
     display.display();
   }
 }
 
 void showMute() {
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor((SCREEN_WIDTH) / 1, (SCREEN_HEIGHT - 34));
   display.print(" MUTE ON ");
   display.display();
 }
 