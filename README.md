# Alarm gateway node


---

## SETUP info

* **Activate BLE**
  * pio run -t menuconfig 
    *  Component config → Bluetooth (space *)
    *  Bluetooth host → NimBLE (space *)
    *  Press 'S' to save

* **NOTE: If NOT using ESP32-S3**
  * You need to update the plattformio.ini to match your hardware (**S3 is selected by default**)
  
---

## Implemented features

* LVGL graphics
* Alarm logic
  * Activate / deactivate (from reported alarm OR missing heartbeat @ BLE)
    * Buzzer
* Comminucation
  * BLE
    * Sending: Heartbeat/Alarm-state (Tx)
    * Receiving: Alarm info (Rx)

---