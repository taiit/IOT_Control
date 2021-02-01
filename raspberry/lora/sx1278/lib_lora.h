/**
 * lib_lora.h
 */
#ifndef __LIB_LORA_H__
#define __LIB_LORA_H__

#define LORA_REST_PIN               2
#define LORA_SS_PIN                 3
#define LORA_DIO_0_PIN              0

// The Pi has 2 channels; 0 and 1 ????
#define RASP_PI4_SPI_CHANNEL        0
#define RASP_PI4_SPI_SPEED          500000


/*
 * Transceiver modes
 */
#define MODE_LONG_RANGE_MODE           0x80
#define MODE_SLEEP                     0x00
#define MODE_STDBY                     0x01
#define MODE_TX                        0x03
#define MODE_RX_CONTINUOUS             0x05
#define MODE_RX_SINGLE                 0x06

/*
 * PA configuration
 */
#define PA_BOOST                       0x80

/*
 * IRQ masks
 */
#define IRQ_TX_DONE_MASK               0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK     0x20
#define IRQ_RX_DONE_MASK               0x40

#define PA_OUTPUT_RFO_PIN              0
#define PA_OUTPUT_PA_BOOST_PIN         1

#define TIMEOUT_RESET                  100

/**
 * Read the current value of a register.
 * @param reg Register index.
 * @return Value of the register.
 */
unsigned char lora_read_reg(unsigned char addr);

/**
 * Write a value to a register.
 * @param reg Register index.
 * @param val Value to write.
 */
void lora_write_reg(unsigned char addr, unsigned char value);

/**
 * Perform physical reset on the Lora chip
 */
void lora_reset(void);

/**
 * Configure explicit header mode.
 * Packet size will be included in the frame.
 */
void lora_explicit_header_mode(void);

/**
 * Configure implicit header mode.
 * All packets will have a predefined size.
 * @param size Size of the packets.
 */
void lora_implicit_header_mode(int size);
/**
 * Sets the radio transceiver in idle mode.
 * Must be used to change registers and access the FIFO.
 */
void lora_idle(void);

/**
 * Sets the radio transceiver in sleep mode.
 * Low power consumption and FIFO is lost.
 */
void lora_sleep(void);

/**
 * Sets the radio transceiver in receive mode.
 * Incoming packets will be received.
 */
void lora_receiving(void);

/**
 * Configure power level for transmission
 * @param level 2-17, from least to most power
 */
void lora_set_tx_power(int level);

/**
 * Set carrier frequency.
 * @param frequency Frequency in Hz
 */
void lora_set_frequency(long frequency);

/**
 * Set spreading factor.
 * @param sf 6-12, Spreading factor to use.
 */
void lora_set_spreading_factor(int sf);

/**
 * Set bandwidth (bit rate)
 * @param sbw Bandwidth in Hz (up to 500000)
 */
void lora_set_bandwidth(long sbw);

/**
 * Set coding rate
 * @param denominator 5-8, Denominator for the coding rate 4/x
 */
void lora_set_coding_rate(int denominator);

/**
 * Set the size of preamble.
 * @param length Preamble length in symbols.
 */
void lora_set_preamble_length(long length);

/**
 * Change radio sync word.
 * @param sw New sync word to use.
 */
void lora_set_sync_word(int sw);

/**
 * Enable appending/verifying packet CRC.
 */
void lora_enable_crc(void);

/**
 * Disable appending/verifying packet CRC.
 */
void lora_disable_crc(void);

/**
 * Perform hardware initialization.
 */
int lora_init(void);


/**
 * Send a packet.
 * @param buf Data to be sent
 * @param size Size of data.
 */
void lora_send_packet(unsigned char *buf, int size);

/**
 * Read a received packet.
 * @param buf Buffer for the data.
 * @param size Available size in buffer (bytes).
 * @return Number of bytes received (zero if no packet available).
 */
int lora_receive_packet(unsigned char *buf, int size);

/**
 * Returns non-zero if there is data to read (packet received).
 */
int is_lora_received(void);
/**
 * Return last packet's RSSI.
 */
int lora_packet_rssi(void);
/**
 * Return last packet's SNR (signal to noise ratio).
 */
float lora_packet_snr(void);
/**
 * Shutdown hardware.
 */
void lora_close(void);

void lora_dump_registers(void);


#endif // __LIB_LORA_H__
