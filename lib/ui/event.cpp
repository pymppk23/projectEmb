#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "event.h"

extern int pwmChannel1;
extern int pwmChannel2;

// IP ของ ESP32 มอเตอร์
// String motorServer = "http://10.98.106.202";

// ฟังก์ชันส่งคำสั่งไป ESP32 มอเตอร์
void sendCommand(String cmd)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String motorServer = "http://10.98.106.202";
    http.begin(motorServer + "/" + cmd);
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      Serial.printf("Server response: %s\n", http.getString().c_str());
    }
    else
    {
      Serial.printf("HTTP Error: %d\n", httpCode);
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected!");
  }
}

// ==== Callback ของปุ่ม (EEZ Studio จะเรียกฟังก์ชันนี้เวลาปุ่มถูกกด) ====

void event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  int *pLED = (int *)lv_event_get_user_data(e);

  if (obj == objects.fwbt && code == LV_EVENT_PRESSED) ////เปลี่ยนจากคลิก เป็น กดค้างวิ่ง ปล่อยหยุดวิ่ง
  {
    sendCommand("forward");
  }
  else if (obj == objects.fwbt && code == LV_EVENT_RELEASED)

    if (obj == objects.bwbt && code == LV_EVENT_PRESSED)
    {
      sendCommand("backward");
    }
    else if (obj == objects.fwbt && code == LV_EVENT_RELEASED)

      if (obj == objects.lbt && code == LV_EVENT_PRESSED)
      {
        sendCommand("left");
      }
      else if (obj == objects.fwbt && code == LV_EVENT_RELEASED)

        if (obj == objects.rbt && code == LV_EVENT_PRESSED)
        {
          sendCommand("right");
        }
        else if (obj == objects.fwbt && code == LV_EVENT_RELEASED)

  // next ===================================================================
  if (obj == objects.nextbt && code == LV_EVENT_CLICKED)
  {
    lv_scr_load(objects.buzzer);
  }
  if (obj == objects.nextbt_1 && code == LV_EVENT_CLICKED)
  {
    lv_scr_load(objects.main);
  }

  else if (obj == objects.pwmbuz && code == LV_EVENT_VALUE_CHANGED)
  {
    int val1 = lv_slider_get_value(objects.pwmbuz);
    int persen1 = (val1 * 100) / 255;
    lv_label_set_text(objects.perbuz, (String(persen1) + "%").c_str());
    ledcWrite(pwmChannel1, val1);
    Serial.println(val1);
  }

  else if (obj == objects.pwmmo && code == LV_EVENT_VALUE_CHANGED)
  {
    int val2 = lv_slider_get_value(objects.pwmmo);
    int persen2 = (val2 * 100) / 255;
    lv_label_set_text(objects.permo, (String(persen2) + "%").c_str());
    ledcWrite(pwmChannel2, val2);

    Serial.println(val2);
  }
}

void time_ud(lv_timer_t *timer)
{
}