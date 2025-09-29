#include "ui.h"
#include <Arduino_GFX_Library.h>
#include <HTTPClient.h>
#include "event.h"
#include <WiFi.h>
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

#define GFX_BL 32
Arduino_DataBus *bus = new Arduino_ESP32SPI(2, 15, 18, 23);
Arduino_GFX *gfx = new Arduino_ILI9342(bus, 4, 1);
#define CANVAS

#endif /* !defined(DISPLAY_DEV_KIT) */
#include "touch.hpp"

static uint32_t screenWidth;
static uint32_t screenHeight;
static uint32_t bufSize;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// int LED1 =26 ;
// int *pLED1 = &LED1;
// int LED2 =22 ;
// int *pLED2 = &LED2;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
#ifndef DIRECT_MODE
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
#endif // #ifndef DIRECT_MODE

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

// const char* id = "God Hydro";
// const char* pass = "0610187446";

int pwmPin1 = 32;
int pwmPin2 = 33;
int pwmChannel1 = 0;
int pwmChannel2 = 1;
int pwmFreq = 5000;
int pwmResolution = 8;

void setup()
{
  Serial.begin(115200);
  // WiFi.begin(id, pass);
  //   while (WiFi.status() != WL_CONNECTED) {
  //     delay(500);
  //     Serial.print(".");
  //   }
  //   Serial.println("\nConnected to WiFi!");

  ledcSetup(pwmChannel1, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin1, pwmChannel1);

  ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin2, pwmChannel2);

  // pinMode(LED1, OUTPUT);
  // pinMode(LED2, OUTPUT);

  // Serial.setDebugOutput(true);
  // while(!Serial);

  Serial.println("Arduino_GFX LVGL Widgets example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);
  gfx->setRotation(2);
#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  touch_init(gfx->width(), gfx->height(), gfx->getRotation());

  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

#ifdef DIRECT_MODE
  bufSize = screenWidth * screenHeight;
#else
  bufSize = screenWidth * 40;
#endif

#ifdef ESP32
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * bufSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf)
  {
    // remove MALLOC_CAP_INTERNAL flag try again
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * bufSize, MALLOC_CAP_8BIT);
  }
#else
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * bufSize);
#endif
  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
#ifdef DIRECT_MODE
    disp_drv.direct_mode = true;
#endif
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    lv_timer_create(time_ud, 500, NULL);

    lv_obj_add_event_cb(objects.fwbt, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.bwbt, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.lbt, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.rbt, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.nextbt, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.nextbt_1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.pwmbuz, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.pwmmo, event_handler, LV_EVENT_ALL, NULL);
    Serial.println("Setup done");
  }
}

void loop()
{

  lv_timer_handler(); /* let the GUI do its work */

#ifdef DIRECT_MODE
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#else
  gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#endif
#endif // #ifdef DIRECT_MODE

#ifdef CANVAS
  gfx->flush();
#endif

  delay(5);
}
