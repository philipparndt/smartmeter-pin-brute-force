# smartmeter-pin-brute-force

Brute force the PIN of a smart meter using an ESP32.

Highly inspired by https://pastebin.com/Rcxix0aX but converted to run on an ESP32. 

Flash the firmware:
```
cd pio
pio run --target upload
```

## Hardware

- ESP32POE
- AZDelivery 3 x 0,96 Zoll OLED Display I2C SSD1306 Chip 128 x 64 Pixel I2C Display
- Hichi TTL - IR
