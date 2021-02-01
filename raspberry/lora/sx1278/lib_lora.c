/**
 *
 */
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>

//#include <base64.h>
#include "lib_lora.h"
#include "sx1278_reg.h"


static int __implicit;

static long __frequency;

/**
 * Select receiver, using LORA_SS_PIN
 */
static void selectreceiver() {
    digitalWrite(LORA_SS_PIN, LOW);
}

/**
 * UnSelect receiver, using LORA_SS_PIN
 */
static void unselectreceiver() {
    digitalWrite(LORA_SS_PIN, HIGH);
}

/**
 * Read the current value of a register.
 * @param reg Register index.
 * @return Value of the register.
 */
unsigned char lora_read_reg(unsigned char addr) {
    unsigned char spibuf[2];


    spibuf[0] = addr & 0x7F; // 0111 1111
    spibuf[1] = 0x00;

    selectreceiver();
    wiringPiSPIDataRW(RASP_PI4_SPI_CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

/**
 * Write a value to a register.
 * @param reg Register index.
 * @param val Value to write.
 */
void lora_write_reg(unsigned char addr, unsigned char value) {
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80; // 1000 0000
    spibuf[1] = value;

    selectreceiver();
    wiringPiSPIDataRW(RASP_PI4_SPI_CHANNEL, spibuf, 2);
    unselectreceiver();
}

/**
 * Perform physical reset on the Lora chip
 */
void lora_reset(void) {
    digitalWrite(LORA_REST_PIN, LOW);
    delay(1);
    digitalWrite(LORA_REST_PIN, HIGH);
    delay(10);
}

/**
 * Configure explicit header mode.
 * Packet size will be included in the frame.
 */
void lora_explicit_header_mode(void) {
    __implicit = 0;
    lora_write_reg(REG_MODEM_CONFIG_1, lora_read_reg(REG_MODEM_CONFIG_1) & 0xfe);
}

/**
 * Configure implicit header mode.
 * All packets will have a predefined size.
 * @param size Size of the packets.
 */
void lora_implicit_header_mode(int size) {
    __implicit = 1;
    lora_write_reg(REG_MODEM_CONFIG_1, lora_read_reg(REG_MODEM_CONFIG_1) | 0x01);
    lora_write_reg(REG_PAYLOAD_LENGTH, size);
}

/**
 * Sets the radio transceiver in idle mode.
 * Must be used to change registers and access the FIFO.
 */
void lora_idle(void) {
    lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

/**
 * Sets the radio transceiver in sleep mode.
 * Low power consumption and FIFO is lost.
 */
void lora_sleep(void) {
    lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

/**
 * Sets the radio transceiver in receive mode.
 * Incoming packets will be received.
 */
void lora_receiving(void) {
    lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

/**
 * Configure power level for transmission
 * @param level 2-17, from least to most power
 */
void lora_set_tx_power(int level) {
    // RF9x module uses PA_BOOST pin
    if (level < 2) {
        level = 2;
    } else if (level > 17) {
        level = 17;
    }
    lora_write_reg(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

/**
 * Set carrier frequency.
 * @param frequency Frequency in Hz
 */
void lora_set_frequency(long frequency) {
    __frequency = frequency;

    uint64_t frf = ((uint64_t) frequency << 19) / 32000000; // uint64_t??

    lora_write_reg(REG_FRF_MSB, (unsigned char)(frf >> 16));
    lora_write_reg(REG_FRF_MID, (unsigned char)(frf >> 8));
    lora_write_reg(REG_FRF_LSB, (unsigned char)(frf >> 0));
}

/**
 * Set spreading factor.
 * @param sf 6-12, Spreading factor to use.
 */
void lora_set_spreading_factor(int sf) {
    if (sf < 6) {
        sf = 6;
    } else if (sf > 12) {
        sf = 12;
    }

    if (sf == 6) {
        lora_write_reg(REG_DETECTION_OPTIMIZE, 0xc5);
        lora_write_reg(REG_DETECTION_THRESHOLD, 0x0c);
    } else {
        lora_write_reg(REG_DETECTION_OPTIMIZE, 0xc3);
        lora_write_reg(REG_DETECTION_THRESHOLD, 0x0a);
    }

    lora_write_reg(REG_MODEM_CONFIG_2, (lora_read_reg(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
}

/**
 * Set bandwidth (bit rate)
 * @param sbw Bandwidth in Hz (up to 500000)
 */
void lora_set_bandwidth(long sbw) {
    int bw;

    if (sbw <= 7.8E3) {
        bw = 0;
    } else if (sbw <= 10.4E3) {
        bw = 1;
    } else if (sbw <= 15.6E3) {
        bw = 2;
    } else if (sbw <= 20.8E3) {
        bw = 3;
    } else if (sbw <= 31.25E3) {
        bw = 4;
    } else if (sbw <= 41.7E3) {
        bw = 5;
    } else if (sbw <= 62.5E3) {
        bw = 6;
    } else if (sbw <= 125E3) {
        bw = 7;
    } else if (sbw <= 250E3) {
        bw = 8;
    } else {
        bw = 9;
    }
    lora_write_reg(REG_MODEM_CONFIG_1, (lora_read_reg(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

/**
 * Set coding rate
 * @param denominator 5-8, Denominator for the coding rate 4/x
 */
void lora_set_coding_rate(int denominator) {
    if (denominator < 5) {
        denominator = 5;
    } else if (denominator > 8) {
        denominator = 8;
    }

    int cr = denominator - 4;
    lora_write_reg(REG_MODEM_CONFIG_1, (lora_read_reg(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

/**
 * Set the size of preamble.
 * @param length Preamble length in symbols.
 */
void lora_set_preamble_length(long length) {
    lora_write_reg(REG_PREAMBLE_MSB, (unsigned char)(length >> 8));
    lora_write_reg(REG_PREAMBLE_LSB, (unsigned char)(length >> 0));
}

/**
 * Change radio sync word.
 * @param sw New sync word to use.
 */
void lora_set_sync_word(int sw) {
    lora_write_reg(REG_SYNC_WORD, sw);
}

/**
 * Enable appending/verifying packet CRC.
 */
void lora_enable_crc(void) {
    lora_write_reg(REG_MODEM_CONFIG_2, lora_read_reg(REG_MODEM_CONFIG_2) | 0x04);
}

/**
 * Disable appending/verifying packet CRC.
 */
void lora_disable_crc(void) {
    lora_write_reg(REG_MODEM_CONFIG_2, lora_read_reg(REG_MODEM_CONFIG_2) & 0xfb);
}

/**
 * Perform hardware initialization.
 */
int lora_init(void) {
    /*
     * Configure CPU hardware to communicate with the radio chip
     */
    wiringPiSetup(); // if wiringPiSetuped this function do nothing.
    //pinMode(LORA_DIO_0_PIN, INPUT);
    pinMode(LORA_SS_PIN, OUTPUT);
    pinMode(LORA_REST_PIN, OUTPUT);

    if (0 != wiringPiSPISetup(RASP_PI4_SPI_CHANNEL, RASP_PI4_SPI_SPEED)) {
        printf("wiringPiSPISetup errno : %d \n", errno );
    }

    /*
     * Perform hardware reset.
     */
    lora_reset();

    /*
     * Check version.
     */
    unsigned char version;
    unsigned char i = 0;
    while (i++ < TIMEOUT_RESET) {
        version = lora_read_reg(REG_VERSION);
        printf("[LR] ver: 0x%x\n", version);
        if (version == 0x12) {
            printf("Version 0x12 - Sx1278 - OK\n");
            break;
        }
        delay(1000); // 1s
    }
    assert(i <= TIMEOUT_RESET + 1); // at the end of the loop above, the max value i can reach is TIMEOUT_RESET + 1

    // put in sleep mode
    lora_sleep();

    // set base addresses
    lora_write_reg(REG_FIFO_RX_BASE_ADDR, 0);
    lora_write_reg(REG_FIFO_TX_BASE_ADDR, 0);

    // set LNA boost
    lora_write_reg(REG_LNA, lora_read_reg(REG_LNA) | 0x03);

    // set auto AGC
    lora_write_reg(REG_MODEM_CONFIG_3, 0x04);

    // set output power to 17 dB
    lora_set_tx_power(17);

    // put in standby mode
    lora_idle();
    return 1;
}

/**
 * Send a packet.
 * @param buf Data to be sent
 * @param size Size of data.
 */
void lora_send_packet(unsigned char *buf, int size) {
    /*
     * Transfer data to radio.
     */
    lora_idle();
    lora_write_reg(REG_FIFO_ADDR_PTR, 0);

    for (int i = 0; i < size; i++) {
        lora_write_reg(REG_FIFO, *buf++);
    }

    lora_write_reg(REG_PAYLOAD_LENGTH, size);

    /*
     * Start transmission and wait for conclusion.
     */
    lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
    while ((lora_read_reg(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0) {
        delay(2);
    }

    lora_write_reg(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
}

/**
 * Read a received packet.
 * @param buf Buffer for the data.
 * @param size Available size in buffer (bytes).
 * @return Number of bytes received (zero if no packet available).
 */
int lora_receive_packet(unsigned char *buf, int size) {
    int len = 0;

    /*
     * Check interrupts.
     */
    int irq = lora_read_reg(REG_IRQ_FLAGS);
    lora_write_reg(REG_IRQ_FLAGS, irq);
    if ((irq & IRQ_RX_DONE_MASK) == 0) {
        return 0;
    }
    if (irq & IRQ_PAYLOAD_CRC_ERROR_MASK) {
        return 0;
    }

    /*
     * Find packet size.
     */
    if (__implicit) {
        len = lora_read_reg(REG_PAYLOAD_LENGTH);
    } else {
        len = lora_read_reg(REG_RX_NB_BYTES);
    }


    /*
     * Transfer data from radio.
     */
    lora_idle();
    lora_write_reg(REG_FIFO_ADDR_PTR, lora_read_reg(REG_FIFO_RX_CURRENT_ADDR));
    if (len > size) {
        len = size;
    }
    for (int i = 0; i < len; i++) {
        *buf++ = lora_read_reg(REG_FIFO);
    }

    return len;
}

/**
 * Returns non-zero if there is data to read (packet received).
 */
int is_lora_received(void) {
    if (lora_read_reg(REG_IRQ_FLAGS) & IRQ_RX_DONE_MASK) {
        return 1;
    }
    return 0;
}

/**
 * Return last packet's RSSI.
 */
int lora_packet_rssi(void) {
    return (lora_read_reg(REG_PKT_RSSI_VALUE) - (__frequency < 868E6 ? 164 : 157));
}

/**
 * Return last packet's SNR (signal to noise ratio).
 */
float lora_packet_snr(void) {
    return ((int8_t) lora_read_reg(REG_PKT_SNR_VALUE)) * 0.25;
}

/**
 * Shutdown hardware.
 */
void lora_close(void) {
    lora_sleep();
//   close(__spi);  FIXME: end hardware features after lora_close
//   close(__cs);
//   close(__rst);
//   __spi = -1;
//   __cs = -1;
//   __rst = -1;
}

void lora_dump_registers(void) {
    int i;
    printf("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    for (i = 0; i < 0x40; i++) {
        printf("%02X ", lora_read_reg(i));
        if ((i & 0x0f) == 0x0f) {
            printf("\n");
        }
    }
    printf("\n");
}

