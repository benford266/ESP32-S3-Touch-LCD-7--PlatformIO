#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>

#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <demos/lv_demos.h>

// Extend IO Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5     // USB select pin

void setup()
{
    // String to write to screen
    String title = "PlatformIO LVGL Example For WaveShare ESP32 Display Panel";

    Serial.begin(115200);

    pinMode(GPIO_INPUT_IO_4, OUTPUT);

    /* Initialize IO expander */
    Serial.println("Initialize IO expander");
    ESP_IOExpander_CH422G *expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    expander->init();
    expander->begin();

    Serial.println("Set the IO0-7 pin to output mode.");
    expander->enableAllIO_Output();
    expander->digitalWrite(TP_RST , HIGH);
    expander->digitalWrite(LCD_RST , HIGH);
    expander->digitalWrite(LCD_BL , HIGH);
    delay(100);
    
    // GT911 initialization, must be added, otherwise the touch screen will not be recognized  
    // Initialization begin
    expander->digitalWrite(TP_RST , LOW);
    delay(100);
    digitalWrite(GPIO_INPUT_IO_4, LOW);
    delay(100);
    expander->digitalWrite(TP_RST , HIGH);
    delay(200);
    // Initialization end

    Serial.println(title + " start");
    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
    
#if LVGL_PORT_AVOID_TEAR
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif

    panel->begin();

    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());
    Serial.println("Create UI");


    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /* Create a simple label */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, title.c_str());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lvgl_port_unlock();

    Serial.println(title + " end");
}

void loop()
{
    delay(1000);
}
