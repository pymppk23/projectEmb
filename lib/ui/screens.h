#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *buzzer;
    lv_obj_t *nextbt;
    lv_obj_t *bwbt;
    lv_obj_t *fwbt;
    lv_obj_t *lbt;
    lv_obj_t *rbt;
    lv_obj_t *obj0;
    lv_obj_t *nextbt_1;
    lv_obj_t *pwmbuz;
    lv_obj_t *perbuz;
    lv_obj_t *pwmmo;
    lv_obj_t *permo;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_BUZZER = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_buzzer();
void tick_screen_buzzer();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/