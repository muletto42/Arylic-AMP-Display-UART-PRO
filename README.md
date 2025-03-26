Übersetzung von ChatGPT
# Arylic-AMP-Display-UART-PRO

**0,91" OLED-Display für Arylic AMP-Geräte**

Anzeige-Steuerung mit ESP32 und 0,91” OLED-Display

Dieses Projekt nutzt ein ESP32S zusammen mit einem 0,91” OLED-Display, um Informationen über den Status eines Audioverstärkers anzuzeigen. Der Code ermöglicht den Empfang von Befehlen über eine UART-Kommunikation (Seriell), um zwischen verschiedenen Anzeigemodi zu wechseln, wie z. B. Signalquelle, Lautstärke, Bass, Höhen und mehr. Zusätzlich wurde eine STANDBY-Funktion implementiert, die eine "STANDBY"-Meldung auf dem Display anzeigt und alle 2 Sekunden blinkt, bis ein Befehl zum Wechsel zurück in den SOURCE-Modus empfangen wird.
Verwendete Komponenten

    ESP32 (in diesem Beispiel wurde ein NodeMCU ESP32S verwendet)

    OLED-Display (ein I2C-OLED-Display mit 128x32 Pixeln wurde genutzt)

    Audioverstärker mit der Fähigkeit, UART-Befehle über seinen seriellen Port zu senden (up2Stream PRO_V4)

**Verbindungen**

Verbinde die Komponenten wie folgt:

    OLED-Display:
        VCC-Pin → 5V
        GND-Pin → GND
        SDA-Pin → SDA-Pin des ESP32 (z. B. Pin 21)
        SCL-Pin → SCL-Pin des ESP32 (z. B. Pin 22)

    ESP32-Modul:
        VCC → 5V
        GND → GND
        RX → TX des Verstärkers (z. B. Pin 16)
        TX → RX des Verstärkers (z. B. Pin 17)

    Audioverstärker:
        Seriellen Port des Verstärkers mit der UART des ESP32 verbinden (z. B. GND, RX_PIN und TX_PIN)

**Verwendete Bibliotheken**

Der Code nutzt folgende Bibliotheken für den korrekten Betrieb des OLED-Displays und der UART-Kommunikation:
    Wire.h – Für die I2C-Kommunikation mit dem OLED-Display
    Adafruit_GFX.h – Für die Handhabung von Grafiken und Text auf dem OLED-Display
    Adafruit_SSD1306.h – Zur Steuerung des OLED-Displays
    HardwareSerial.h – Für die serielle UART-Kommunikation mit dem Audioverstärker

**Funktionen**
    Der Code empfängt Befehle über UART und wechselt zwischen verschiedenen Anzeigemodi.
    Anzeigemodi umfassen: Signalquelle, Lautstärke, Bass, Höhen und einen noch nicht implementierten "BLANK"-Modus zum Ausschalten des Displays.
    Der STANDBY-Modus zeigt eine "STANDBY"-Meldung an und lässt das Display alle 2 Sekunden blinken, bis ein Befehl zum Wechsel in den SOURCE-Modus empfangen wird.
    Es können bestimmte Befehle verarbeitet werden (SRC, VOL, BAS, TRE, CHN, LED, BTC, VBS, BEP), um die Anzeige entsprechend zu aktualisieren.

**Verwendung**

    Verbinde die Komponenten gemäß der "Verbindungen"-Sektion.
    Lade den Code mit der Arduino-IDE auf den ESP32 hoch.
    Schließe den Audioverstärker an und stelle sicher, dass er UART-Befehle über den seriellen Port sendet.
    Beobachte die Anzeige und steuere die verschiedenen Anzeigemodi, indem du die entsprechenden Befehle über UART sendest.

**Hinweise**
    Falls du weitere Befehle oder Funktionen hinzufügen möchtest, kannst du die Funktion processUARTCommand anpassen, um neue Befehle zu verarbeiten und die Anzeige entsprechend zu aktualisieren.

**Credits**
Dieses Projekt wurde von Richard Mequert [Zerous] erstellt und basiert auf der Arbeit von ResinChem Tech sowie der Dokumentation der verwendeten Bibliotheken.

-------

# Arylic-AMP-Display-UART-PRO
0.91" Oled Display for Arylic AMP Devices

**Control de Visualización con ESP32 y Display OLED 0.91”**

Este proyecto utiliza un ESP32S junto con un Display OLED 0.91” para mostrar información sobre el estado de un amplificador de audio. El código permite recibir comandos a través de una comunicación UART (Serial) para cambiar entre diferentes modos de visualización, como el origen de la señal, el volumen, el bajo, el agudo y más. Además, se ha implementado una función de modo STANDBY que muestra un mensaje de "STANDBY" en pantalla y parpadea cada 2 segundos hasta que se recibe un comando para volver al modo SOURCE.

**Componentes utilizados**

- ESP32 (se ha utilizado un NodeMCU ESP32S en este ejemplo)
- Display OLED (Se ha utilizado un display I2C OLED de 128x32 píxeles)
- Amplificador de Audio con capacidad de enviar comandos UART a través de su puerto serial (up2Stream PRO\_V4)

**Conexiones**

Conectar los componentes siguiendo estas conexiones:

- Display OLED:
  - Pin VCC a 5V
  - Pin GND a GND
  - Pin SDA a pin SDA del ESP32 (por ejemplo, pin 21)
  - Pin SCL a pin SCL del ESP32 (por ejemplo, pin 22)
- Módulo ESP32:
  - VCC a 5V
  - GND a GND
  - RX al TX del Amplificador (por ejemplo, pin 16)
  - TX al RX del Amplificador (por ejemplo, pin 17)
- Amplificador de Audio:
  - Conectar el puerto serial del amplificador a la UART del ESP32 (por ejemplo, GND, RX\_PIN y TX\_PIN)

**Librerías utilizadas**

El código utiliza las siguientes librerías para el correcto funcionamiento del display OLED y la comunicación UART:

- Wire.h: Para la comunicación I2C con el display OLED.
- Adafruit\_GFX.h: Para el manejo de gráficos y texto en el display OLED.
- Adafruit\_SSD1306.h: Para el control del display OLED.
- HardwareSerial.h: Para la comunicación UART (Serial) con el amplificador de audio.

**Funcionalidades**

- El código permite recibir comandos a través de la comunicación UART y cambiar entre diferentes modos de visualización.
- Los modos de visualización incluyen el origen de la señal, el volumen, el bajo, el agudo y un modo BLANK (aun no implementado) para apagar la pantalla.
- Se ha implementado un modo STANDBY que muestra un mensaje de "STANDBY" en pantalla y parpadea cada 2 segundos hasta que se recibe un comando para volver al modo SOURCE.
- El código es capaz de procesar comandos específicos (SRC, VOL, BAS, TRE, CHN, LED, BTC, VBS, BEP) y actualizar la información en pantalla en consecuencia.

**Uso**

1. Conecta los componentes como se ha indicado en la sección "Conexiones".
1. Sube el código al ESP32 utilizando el IDE de Arduino.
1. Conecta el amplificador de audio y asegúrate de que esté enviando comandos UART a través del puerto serial.
1. Observa la información en pantalla y controla los diferentes modos de visualización enviando los comandos correspondientes a través de la comunicación UART.

**Notas**

- Si deseas agregar más comandos o funcionalidades, puedes modificar la función **processUARTCommand** para manejar los nuevos comandos y actualizar la información en pantalla según sea necesario.

**Créditos**

Este proyecto ha sido creado por Richard Mequert [Zerous] y está inspirado en el trabajo de ResinChem Tech y la documentación de las librerías utilizadas.
