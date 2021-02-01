#include <stdio.h>
#include <wiringPi.h>
#include "lib_lora.h"
#include <string.h>
//#include "arc4.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

uint8_t SENSOR_ID = 0x01;     // address of this device

// ARC4 START
#define MAX 16
#define ARC4_PASS "Live&Code"
#define ARC4_PASS_LENGTH 9
static uint8_t sbox[MAX], key[MAX];

void swap(uint8_t *a, uint8_t *b) {
    uint8_t t = *a;
    *a = *b;
    *b = t;
}

/**
 *
 * @param text
 * @param text_len
 * @param passwd
 * @param pass_len
 * @param output leng the same input length
 */
void ARC4(const uint8_t *text, uint8_t text_len, const char *passwd,
        uint8_t pass_len, uint8_t *output) {

    for (uint8_t a = 0; a < MAX; a++) {
        key[a] = passwd[a % pass_len];
        sbox[a] = a;
    }

    for (uint8_t a = 0, b = 0; a < MAX; a++) {
        b = (b + sbox[a] + key[a]) % MAX;
        swap(&sbox[a], &sbox[b]);
    }

    for (uint8_t a = 0, i = 0, j = 0, k; a < text_len; a++) {
        i = (i + 1) % MAX;
        j = (j + sbox[i]) % MAX;
        swap(&sbox[i], &sbox[j]);
        k = sbox[(sbox[i] + sbox[j]) % MAX];
        output[a] = text[a] ^ k;
    }
}

// ARC4 END

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t send_thread, revice_thread;
int counter = 0;
pthread_mutex_t lock;

void* sending_func(void *arg) {
    uint8_t msg[4] = { SENSOR_ID, 0x01, 0xAA, 0xAA };
    uint8_t msg_encrypt[4];
    ARC4(msg, 4, ARC4_PASS, ARC4_PASS_LENGTH, msg_encrypt);

    while (1) {
        pthread_mutex_lock(&lock);
        {
            if (counter >= (60 * 10) ) {
                lora_send_packet(msg_encrypt, 4);
                counter = 0;
            } else {
                counter++;
            }
        }
        pthread_mutex_unlock(&lock);
        sleep(1); // 1 second
    }

    return NULL;
}

void* recive_func(void *arg) {
    float sensor_temp;
    int sensor_rssi;

    unsigned char buf[16];
    int num_bytes_received = 0;
    printf("TIME;GW RSSI;Sensor Temp;Sensor RSSI\n");
    while (1) {

        lora_receiving();    // put into receive mode
        while (is_lora_received()) {
            pthread_mutex_lock(&lock);
            {
                num_bytes_received = lora_receive_packet(buf, sizeof(buf));

                uint8_t msg_decrypt[16] = { 0 };

                ARC4(buf, 16, ARC4_PASS, ARC4_PASS_LENGTH, msg_decrypt);

                printf(">> num_bytes_received: %d \n", num_bytes_received);

#if 0
                printf("msg_raw_val: ");
                for (uint8_t i = 0; i < 16; i++) {
                    printf("0x%.2x ", buf[i]);
                }
                printf("\n");

                printf("msg_decrypt: ");
                for (uint8_t i = 0; i < 16; i++) {
                    printf("0x%.2x ", msg_decrypt[i]);
                }
#endif
                ((uint8_t*) &sensor_temp)[0] = msg_decrypt[1];
                ((uint8_t*) &sensor_temp)[1] = msg_decrypt[2];
                ((uint8_t*) &sensor_temp)[2] = msg_decrypt[3];
                ((uint8_t*) &sensor_temp)[3] = msg_decrypt[4];

                sensor_rssi = msg_decrypt[5] * (-1);

                time_t t = time(NULL);
                struct tm *tm = localtime(&t);
                char s[64];
                assert(strftime(s, sizeof(s), "%c", tm));
                printf("%s;%d;%f;%d\n", s, lora_packet_rssi(), sensor_temp, sensor_rssi);
                if(sensor_temp < 0 || sensor_temp > 100 ) {
                    counter = (60*10); // force resent data
                } else {
                    char buffer[1024] = { '\0' };
                    sprintf(buffer, "mosquitto_pub -d -q 1 -h \"192.168.0.12\" -t \"v1/devices/me/telemetry\" -u \"d2LqzzNLNR4mWKUCQGoi\" -m \"{\"temperature\":%.2f}\"", sensor_temp);
                    system(buffer);
                }
            }
            pthread_mutex_unlock(&lock);
        }//is_lora_received

        //sleep(1);
    }
    return NULL;
}
/**
 * Test lora reg function
 */
void test_reg() {
    lora_init();
    lora_set_frequency(433000000);
    lora_dump_registers();
}

int lora_gw() {

    // test_base64();
    //test_socc_thread();

    lora_init();
    lora_set_frequency(433000000);
    //lora_enable_crc();

    //lora_dump_registers();

    int error;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    error = pthread_create(&send_thread, NULL, &sending_func, NULL);
    if (error != 0) {
        printf("\nThread can't be created :[%s]", strerror(error));
    }

    error = pthread_create(&revice_thread, NULL, &recive_func, NULL);
    if (error != 0) {
        printf("\nThread can't be created :[%s]", strerror(error));
    }

    printf("\nLora gw is runing...\n");

    //lora_set_spreading_factor(6);

    pthread_join(send_thread, NULL);
    pthread_join(revice_thread, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}

int main() {
    return lora_gw();
}

