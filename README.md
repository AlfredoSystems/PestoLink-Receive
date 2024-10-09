# PestoLink-Receive

Do you want to Control your ESP32 robot wirelessly over Bluetooth? PestoLink-Receive is the library for you! Once you set up your robot with PestoLink-Receive, you can open the web app [PestoLink-Online](https://github.com/AlfredoSystems/PestoLink-Online), connect to your ESP32 with Bluetooth, and drive your robot in real time!

This library is a BLE alternitive to [AlfredoConnect-Receive](https://github.com/AlfredoElectronics/AlfredoConnect-Receive)

### Installation

1. **Get the Arduino IDE**. If you don't already have it, download it from the [Arduino website](https://www.arduino.cc/en/main/software) and install it.

2. **Download this repository as a ZIP**. Click the green button on this page that says `Code`, then `Download ZIP`.

3. **Add this library to the Arduino IDE**. In the Arduino IDE, Click `Sketch` > `Include Library` > `Add .ZIP Library...`, and select the ZIP file you downloaded.

4. **Configure the Arduino IDE to upload to an ESP32** In the Arduino IDE, click File > Preferences. Paste the following in the Additional Boards Manager URLs field.

`https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

Then, go to Tools > Board > Boards Manager. Search for “ESP32” and install the package “esp32 by Espressif Systems”.

### Robot Code

1. Open the example file `NoU2-ArcadeBot.ino` in the Arduino IDE.

2. find the String `"ESP32 Bluetooth"` and change the text to `"your robot name"` (quotes are required).

3. Select the ESP32 board. Go to Tools > Board and select ESP32 Dev Module from the long list.

4. Select the COM port. Connect the ESP32 to your computer using a micro-USB cable and select the new COM port that appears under Tools > Port. The correct COM port may say (Silicon Labs) next to it. If none of them do, you can unplug and replug the USB to see which COM port disappears and reappears.

5. Upload. Clicking the Upload button (the arrow in the top left pointing right). If the console on the bottom of the window starts showing the text Connecting....._____....., hold down the BOOT button on the ESP32.

### Connecting and Driving

1. Press/click `CONNECT`. A pairing menu will appear, find and select the robot name you chose.

2. After the connection opens, you can now drive your robot!
