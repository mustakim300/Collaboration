#include <TuyaWifi.h>
#include <SoftwareSerial.h>

TuyaWifi my_device;
SoftwareSerial DebugSerial(8,9);
int wifi_key_pin = 7;
unsigned char led_state = 0;


int red = 6;
int green = 5;
int blue = 3;


/* Data point define */
#define DPID_VALUE1  101
#define DPID_VALUE2  102
#define DPID_VALUE3  103
long dp_1 = 0;
long dp_2 = 0;
long dp_3 = 0;

unsigned char dp_array[][2] = 
{
    {DPID_VALUE1, DP_TYPE_VALUE},
    {DPID_VALUE2, DP_TYPE_VALUE},
    {DPID_VALUE3, DP_TYPE_VALUE},
};

unsigned char pid[] = {"wxiufodios50ktaf"};
unsigned char mcu_ver[] = {"1.0.0"};

/* last time */
unsigned long last_time = 0;

void setup()
{
    Serial.begin(9600);
    DebugSerial.begin(9600);

    //Initialize led port, turn off led.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    pinMode(red, OUTPUT);
    digitalWrite(red, LOW);
    pinMode(green, OUTPUT);
    digitalWrite(green, LOW);
    pinMode(blue, OUTPUT);
    digitalWrite(blue, LOW);
    
    //Initialize networking keys.
    pinMode(wifi_key_pin, INPUT_PULLUP);

    //Enter the PID and MCU software version
    my_device.init(pid, mcu_ver);
    //incoming all DPs and their types array, DP numbers
    my_device.set_dp_cmd_total(dp_array, 3);
    //register DP download processing callback function
    my_device.dp_process_func_register(dp_process);
    //register upload all DP callback function
    my_device.dp_update_all_func_register(dp_update_all);

    last_time = millis();
}

void loop()
{
    my_device.uart_service();

    //Enter the connection network mode when Pin7 is pressed.
    if (digitalRead(wifi_key_pin) == LOW) {
        delay(80);
        if (digitalRead(wifi_key_pin) == LOW) {
            my_device.mcu_set_wifi_mode(SMART_CONFIG);
        }
    }

    /* LED blinks when network is being connected */
    if ((my_device.mcu_get_wifi_work_state() != WIFI_LOW_POWER) && (my_device.mcu_get_wifi_work_state() != WIFI_CONN_CLOUD) && (my_device.mcu_get_wifi_work_state() != WIFI_SATE_UNKNOW)) {
        if (millis() - last_time >= 500) {
            last_time = millis();
            /* Toggle current LED status */
            if (led_state == LOW) {
                led_state = HIGH;
            } else {
                led_state = LOW;
            }

            digitalWrite(LED_BUILTIN, led_state);
        }
    }

    delay(10);
}

unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length)
{
    switch (dpid)
    {
    
        case DPID_VALUE1:
            DebugSerial.println("Value type:");
            dp_1 = my_device.mcu_get_dp_download_data(DPID_VALUE1, value, length);
            DebugSerial.println();
            analogWrite(red,dp_1);
            my_device.mcu_dp_update(DPID_VALUE1, dp_1, 1);
        break;
        
        case DPID_VALUE2:
            DebugSerial.println("Value type:");
            dp_2 = my_device.mcu_get_dp_download_data(DPID_VALUE2, value, length);
            DebugSerial.println();
            analogWrite(green,dp_2);
            /* After processing the download DP command, the current status should be reported. */
            my_device.mcu_dp_update(DPID_VALUE2, dp_2, 1);
        break;


         case DPID_VALUE3:
            DebugSerial.println("Value type:");
            dp_3 = my_device.mcu_get_dp_download_data(DPID_VALUE3, value, length);
            DebugSerial.println();
            analogWrite(blue,dp_3);
            /* After processing the download DP command, the current status should be reported. */
            my_device.mcu_dp_update(DPID_VALUE3, dp_3, 1);
        break;

        default:
            break;
    }
    
    return SUCCESS;
}

void dp_update_all(void)
{
  my_device.mcu_dp_update(DPID_VALUE1, dp_1, 1);
  my_device.mcu_dp_update(DPID_VALUE2, dp_2, 1);
  my_device.mcu_dp_update(DPID_VALUE3, dp_3, 1);
}
