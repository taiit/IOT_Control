
#include <stdio.h>
#include <wiringPi.h>
#include <base64.h>
#include <string.h>
#include "lib_lora.h"
#include "socc_thread.h"

void test_base64() {
    //
    // Test base64
    //
    const unsigned char data[] = "Simple test file";
    unsigned char *encode_data, *decode_data;
    size_t len;

    encode_data = base64_encode(data, (size_t)strlen((const char*)data), &len);
    decode_data = base64_decode(encode_data, len, &len);

    printf("encode_data: %s \n", encode_data);
    printf("decode_data: %s \n", decode_data);

    free(encode_data);
    free(decode_data);

}

int main() {
    int num_bytes_received = 0;
    unsigned char buf[512];
    unsigned char out[512];

    test_base64();
    test_socc_thread();

    lora_init();
    lora_set_frequency(433000000);
    lora_enable_crc();

    lora_dump_registers();

//    lora_set_spreading_factor(6);
    memset(out, 0, 512);
    while(1) {
        lora_receiving();    // put into receive mode
        while (is_lora_received()) {
            num_bytes_received = lora_receive_packet(buf, sizeof(buf));
            buf[num_bytes_received] = 0;
            printf("Received: %s\n", buf);
            //int b64_to_bin(const char * in, int size, uint8_t * out, int max_len);
            //b64_to_bin((const char *)buf, num_bytes_received, out, 512);
            printf("Out: ");
            for(int i = 0; i < 512; i++) {
                printf("%d ", out[i]);
            }
            printf("\n");
            //printf("out: %s \n", out);
        }
        delay(10);
    }
}
