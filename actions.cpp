#include "actions.h"
#include <Arduino.h>
#include <lvgl.h>
int shouldStart = 1;
void action_charging(lv_event_t *e) {

    lv_obj_t * btn = lv_event_get_target(e);

    lv_obj_t * label = lv_obj_get_child(btn, 0);
    if(shouldStart)
    {
      
      lv_label_set_text(label, "Stop");
      Serial.println("Start Charging....");
      shouldStart = 0;
    }else{
      lv_label_set_text(label, "Start");
      Serial.println("Stop Charging....");
      shouldStart = 1;
    }
}

#include "actions.h"

void action_power(lv_event_t *e) {
    // TODO: Implement action power here
}

