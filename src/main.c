#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

#define PROCESS_SIZE 100

#define LED_1 13
#define LED_2 12
#define LED_3 14

#define RGB_RED 17
#define RGB_GREEN 26
#define RGB_BLUE 25

char *TAG = "BLE-Server";
uint8_t ble_addr_type;
void ble_app_advertise(void);

int animation_queue[PROCESS_SIZE];
int animation_first = -1;
int animation_last = -1;

int speed = 1;

// Animation queue functions //

// Add process to end of the queue
void addAnimation(int insert_animation)
{
    if (animation_last == PROCESS_SIZE - 1){
        printf("Overflow\n");
    }
    else
    {
        if (animation_first == - 1){
            animation_first = 0;
        }
        printf("Animation inserted: ");
        printf("%d \n", insert_animation);
        animation_last = animation_last + 1;
        animation_queue[animation_last] = insert_animation;
    }
} 
 
// delete first process in queue
void doneAnimation()
{
    animation_first = animation_first + 1;
} 
 
// show value of fist process in queue
int show_fist()
{
    if (animation_first == - 1){
        return -1;
    }
    else
    {
        return animation_queue[animation_first];
    }
} 

// Show all values in queue
void show_all()
{
    if (animation_first == - 1){
        printf("No animation in queue\n");
        return;
    }
    else
    {
        printf("Animations queue: \n");
        for (int i = animation_first; i <= animation_last; i++)
            printf("%d ", animation_queue[i]);
        printf("\n");
    }
} 

// Initialize led, set the GPIO output
void set_lights () {
    esp_rom_gpio_pad_select_gpio(LED_1);
    esp_rom_gpio_pad_select_gpio(LED_2);
    esp_rom_gpio_pad_select_gpio(LED_3);
    esp_rom_gpio_pad_select_gpio(RGB_GREEN);
    esp_rom_gpio_pad_select_gpio(RGB_BLUE);
    esp_rom_gpio_pad_select_gpio(RGB_RED);

    ESP_ERROR_CHECK(gpio_set_direction(LED_1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED_2, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED_3, GPIO_MODE_OUTPUT));

    ESP_ERROR_CHECK(gpio_set_direction(RGB_GREEN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(RGB_BLUE, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(RGB_RED, GPIO_MODE_OUTPUT));

    return;
}

// Turn off all lights
void off_lights () {
    ESP_ERROR_CHECK(gpio_set_level(LED_1, 0));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 0));
    ESP_ERROR_CHECK(gpio_set_level(LED_3, 0));

    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 0));
    ESP_ERROR_CHECK(gpio_set_level(RGB_BLUE, 0));
    ESP_ERROR_CHECK(gpio_set_level(RGB_GREEN, 0));

    return;
}

// Animation 1 - Countdown to the start
void start_countdown () {
    set_lights();

    const TickType_t animSpeed = 1000 / portTICK_PERIOD_MS;

    ESP_ERROR_CHECK(gpio_set_level(LED_1, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_1, 1));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_1, 1));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 1));
    ESP_ERROR_CHECK(gpio_set_level(LED_3, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(RGB_GREEN, 1));
    vTaskDelay(animSpeed / speed);
}

// Animation 2 - Countdown to the start for Formula racing
void formula_countdown() {

    set_lights();

    const TickType_t animSpeed = 1000 / portTICK_PERIOD_MS;

    ESP_ERROR_CHECK(gpio_set_level(LED_1, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_3, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 1));
    vTaskDelay(animSpeed / speed);

}

// Animation 3 - Higway direction light
void highway_lights() {

    set_lights();

    const TickType_t animSpeed = 1000 / portTICK_PERIOD_MS;

    ESP_ERROR_CHECK(gpio_set_level(LED_1, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_1, 0));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_2, 0));
    ESP_ERROR_CHECK(gpio_set_level(LED_3, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(LED_3, 0));
    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 1));
    vTaskDelay(animSpeed / speed);

}

// Animation 4 - Traffic light
void traffic_light() {

    set_lights();

    const TickType_t animSpeed = 1000 / portTICK_PERIOD_MS;

    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(RGB_GREEN, 1));
    vTaskDelay(animSpeed / speed);
    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 0));
    vTaskDelay(animSpeed / speed);

}

// Animation 5 - Colors off RGB
void RGB_only() {

    set_lights();

    const TickType_t animSpeed = 1000 / portTICK_PERIOD_MS;
    // Red
    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    // Blue
    ESP_ERROR_CHECK(gpio_set_level(RGB_BLUE, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    // Green
    ESP_ERROR_CHECK(gpio_set_level(RGB_GREEN, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    // Purple
    ESP_ERROR_CHECK(gpio_set_level(RGB_RED, 1));
    ESP_ERROR_CHECK(gpio_set_level(RGB_BLUE, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();
    // Turquoise
    ESP_ERROR_CHECK(gpio_set_level(RGB_GREEN, 1));
    ESP_ERROR_CHECK(gpio_set_level(RGB_BLUE, 1));
    vTaskDelay(animSpeed / speed);
    off_lights();

}

// Process Animations 
void process_animations(void *pvParameter) {

    int cnt = 1;

    while(1) {

        switch (show_fist())
        {
        case 11:
            ESP_LOGE("Animation", "ANIMATION %d START\n", cnt);
            start_countdown();
            off_lights();
            doneAnimation();
            ESP_LOGE("Animation", "ANIMATION %d DONE\n", cnt);
            cnt++;
            break;
        case 12:
            ESP_LOGE("Animation", "ANIMATION %d START\n", cnt);
            formula_countdown();
            off_lights();
            doneAnimation();
            ESP_LOGE("Animation", "ANIMATION %d DONE\n", cnt);
            cnt++;
            break;
        case 13:
            ESP_LOGE("Animation", "ANIMATION %d START\n", cnt);
            highway_lights();
            off_lights();
            doneAnimation();
            ESP_LOGE("Animation", "ANIMATION %d DONE\n", cnt);
            cnt++;
            break;
        case 14:
            ESP_LOGE("Animation", "ANIMATION %d START\n", cnt);
            traffic_light();
            off_lights();
            doneAnimation();
            ESP_LOGE("Animation", "ANIMATION %d DONE\n", cnt);
            cnt++;
            break;
        case 15:
            ESP_LOGE("Animation", "ANIMATION %d START\n", cnt);
            RGB_only();
            off_lights();
            doneAnimation();
            ESP_LOGE("Animation", "ANIMATION %d DONE\n", cnt);
            cnt++;
            break;
        default:
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;
        }
    }

}

// Write data to ESP32
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int receivedValue = (int)ctxt->om->om_data[0];

    switch (receivedValue)
    {
    // Animations
    case 11: case 12: case 13: case 14: case 15:
        addAnimation(receivedValue);
        printf("Recived number %d\n", receivedValue);
        break;
    // Animation speed
    case 1 : case 2: case 3:
        speed = receivedValue;
        printf("Recived speed %d\n", receivedValue);
        break;
    
    default:
        printf("Unexpected data: ");
        printf("%hhu", ctxt->om->om_data[0]);
        printf("\n");
        break;
    }
    
    return 0;
}

// Array of pointers to other service definitions
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x180),                
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xDEAD),           
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Define BLE connection
void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);

}

void ble_app_on_sync()
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();                    
}

void host_task()
{
    nimble_port_run(); 
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());         
    nimble_port_init();                      
    ble_svc_gap_device_name_set("BLE-Server"); 
    ble_svc_gap_init();                        
    ble_svc_gatt_init();                      
    ble_gatts_count_cfg(gatt_svcs);            
    ble_gatts_add_svcs(gatt_svcs);             
    ble_hs_cfg.sync_cb = ble_app_on_sync;      
    nimble_port_freertos_init(host_task);      
    xTaskCreate(&process_animations, "process_task", 2048, NULL, 5, NULL);
    return;
}