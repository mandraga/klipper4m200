// Minimal Bootloader V2 "GET_STATE" client.
// Usage: sudo ./vendor_protocol_programing_tool ./klipper/out/klipper.bin /dev/ttyS2
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>


#define SEND_BUFFER_BYTE_INDEX_CRC 4104
#define FRAME_SIZE 4106
#define PAYLOAD_OFFSET 8
#define MAGIC0 'Z'
#define MAGIC1 'B'
#define MAGIC2 'O'
#define MAGIC3 'T'
#define CMD_GET_STATE 0x01

#define CMD_GET_STATE   0x01
#define CMD_START_APP   0x02
#define CMD_START_FLASH 0x03
#define CMD_FLASH       0x04
#define CMD_REBOOT      0x05
#define CMD_END_FLASH   0x06
    
#define ZOS_BOOT_STATE_IDLE      0
#define ZOS_BOOT_STATE_FAILSAFE  1
#define ZOS_BOOT_STATE_FLASHING  2

// Reset pin, named LED in the java code PF1 -> port F (index 5): 5*32 + 1 = 161 LED(161),
// found in gpio_manager_script.sh
#define RESET_PIN "/sys/devices/platform/leds/leds/nrst:usr/brightness"
// But it also could be GPIO6 PC01 according to the java code
//#define RESET_PIN "/sys/devices/platform/leds/leds/gpio6:usr/brightness"

// Boot found in gpio_manager_script.sh
#define BOOT_PIN "/sys/devices/platform/leds/leds/boot:usr/brightness"
// But it could also be GPIOS PC03 according to the java code
//#define BOOT_PIN "/sys/devices/platform/leds/leds/gpios:usr/brightness"

void set_reset_pin(int value)
{
    // Use label "mcunrst:led1:usr" for NRST (PF01)
    FILE *f = fopen(RESET_PIN, "w");
    if (f) {
        fprintf(f, "%d\n", value ? 1 : 0);
        fclose(f);
    }
}

void set_boot_pin(int value)
{
    // Use label "mcuboot:led1:usr" for BOOT (PC02)
    FILE *f = fopen(BOOT_PIN, "w");
    if (f) {
        fprintf(f, "%d\n", !value ? 1 : 0);
        fclose(f);
    }
}

void reset_mcu()
{
    set_reset_pin(1);
    usleep(1000000); // 1000ms
    set_reset_pin(0);
}

void enter_programming_mode()
{
    // Configure GPIOs for bootloader mode:
    set_boot_pin(1); // Programming mode On
    reset_mcu();
}


void exit_programming_mode()
{
    // Configure GPIOs for bootloader mode:
    set_boot_pin(0); // Programming mode Off
    reset_mcu();
}

static const uint16_t crcTable[256] = {
    0, 4489, 8978, 12955, 17956, 22445, 25910, 29887, 35912, 40385, 44890, 48851, 51820, 56293, 59774, 63735,
    2329, 6288, 10763, 15234, 20285, 24244, 27695, 32166, 34129, 38104, 42563, 47050, 50037, 54012, 57447, 61934,
    4658, 955, 12576, 8361, 21526, 17823, 30468, 26253, 40570, 36851, 48488, 44257, 55390, 51671, 64332, 60101,
    6955, 2722, 14393, 10672, 23823, 19590, 32285, 28564, 38755, 34538, 46193, 42488, 53575, 49358, 62037, 58332,
    9316, 13805, 1910, 5887, 25152, 29641, 16722, 20699, 43052, 47525, 35646, 39607, 60936, 65409, 52506, 56467,
    11645, 15604, 3695, 8166, 27481, 31440, 18507, 22978, 41269, 45244, 33319, 37806, 59153, 63128, 50179, 54666,
    13910, 10207, 5444, 1229, 28786, 25083, 21344, 17129, 47646, 43927, 39180, 34949, 64570, 60851, 57128, 52897,
    16207, 11974, 7261, 3540, 31083, 26850, 23161, 19440, 45831, 41614, 36885, 33180, 62755, 58538, 54833, 51128,
    18632, 22849, 27610, 31315, 3820, 8037, 11774, 15479, 50304, 54537, 59282, 63003, 33444, 37677, 41398, 45119,
    16849, 20568, 25283, 29514, 2037, 5756, 9447, 13678, 52633, 56336, 61067, 65282, 35773, 39476, 43183, 47398,
    23290, 19315, 31208, 26721, 7390, 3415, 16332, 11845, 54962, 51003, 62880, 58409, 37014, 33055, 45956, 41485,
    21475, 17002, 28913, 24952, 5575, 1102, 14037, 10076, 57259, 52770, 64697, 60720, 39311, 34822, 47773, 43796,
    27820, 32037, 20414, 24119, 10888, 15105, 2458, 6163, 57572, 61805, 50166, 53887, 42688, 46921, 34258, 37979,
    26037, 29756, 18087, 22318, 9105, 12824, 131, 4362, 59901, 63604, 51951, 56166, 45017, 48720, 36043, 40258,
    32414, 28439, 23948, 19461, 14522, 10547, 7080, 2593, 62166, 58207, 53700, 49229, 46322, 42363, 38880, 34409,
    30599, 26126, 21653, 17692, 12707, 8234, 4785, 824, 64463, 59974, 55517, 51540, 48619, 44130, 40697, 36720
};

// 0, 4489, 8978, 12955, 17956, 22445, 25910, 29887, 35912, 40385, 44890, 48851, 51820, 56293, 59774, 63735, 2329, 6288, 10763, 15234, 20285, 24244, 27695, 32166, 34129, 38104, 42563, 47050, 50037, 54012, 57447, 61934, 4658, 955, 12576, 8361, 21526, 17823, 30468, 26253, 40570, 36851, 48488, 44257, 55390, 51671, 64332, 60101, 6955, 2722, 14393, 10672, 23823, 19590, 32285, 28564, 38755, 34538, 46193, 42488, 53575, 49358, 62037, 58332, 9316, 13805, 1910, 5887, 25152, 29641, 16722, 20699, 43052, 47525, 35646, 39607, 60936, 65409, 52506, 56467, 11645, 15604, 3695, 8166, 27481, 31440, 18507, 22978, 41269, 45244, 33319, 37806, 59153, 63128, 50179, 54666, 13910, 10207, 5444, 1229, 28786, 25083, 21344, 17129, 47646, 43927, 39180, 34949, 64570, 60851, 57128, 52897, 16207, 11974, 7261, 3540, 31083, 26850, 23161, 19440, 45831, 41614, 36885, 33180, 62755, 58538, 54833, 51128, 18632, 22849, 27610, 31315, 3820, 8037, 11774, 15479, 50304, 54537, 59282, 63003, 33444, 37677, 41398, 45119, 16849, 20568, 25283, 29514, 2037, 5756, 9447, 13678, 52633, 56336, 61067, 65282, 35773, 39476, 43183, 47398, 23290, 19315, 31208, 26721, 7390, 3415, 16332, 11845, 54962, 51003, 62880, 58409, 37014, 33055, 45956, 41485, 21475, 17002, 28913, 24952, 5575, 1102, 14037, 10076, 57259, 52770, 64697, 60720, 39311, 34822, 47773, 43796, 27820, 32037, 20414, 24119, 10888, 15105, 2458, 6163, 57572, 61805, 50166, 53887, 42688, 46921, 34258, 37979, 26037, 29756, 18087, 22318, 9105, 12824, 131, 4362, 59901, 63604, 51951, 56166, 45017, 48720, 36043, 40258, 32414, 28439, 23948, 19461, 14522, 10547, 7080, 2593, 62166, 58207, 53700, 49229, 46322, 42363, 38880, 34409, 30599, 26126, 21653, 17692, 12707, 8234, 4785, 824, 64463, 59974, 55517, 51540, 48619, 44130, 40697, 36720

static int calculateCRC16(int crc, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        crc = (crcTable[((crc >> 8) ^ data[i]) & 0xFF] ^ (crc << 8)) & 0xFFFF;
    }
    return crc;
}

static speed_t baud_to_speed(int baud)
{
    switch (baud) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default: return B115200; /* fallback */
    }
}

static int open_serial(const char *dev, int baud)
{
    int fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tio;
    if (tcgetattr(fd, &tio) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfmakeraw(&tio);
    speed_t sp = baud_to_speed(baud);
    cfsetispeed(&tio, sp);
    cfsetospeed(&tio, sp);
    tio.c_cflag |= CLOCAL | CREAD;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CRTSCTS; // no hw flow

    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

static int write_all(int fd, const void *buf, size_t len, int timeout_ms)
{
    const uint8_t *p = buf;
    size_t remaining = len;
    while (remaining) {
        fd_set wfds;
        struct timeval tv;
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        int r = select(fd + 1, NULL, &wfds, NULL, &tv);
        if (r <= 0) return -1;
        ssize_t w = write(fd, p, remaining);
        if (w < 0) return -1;
        p += w;
        remaining -= w;
    }
    return 0;
}

static int read_exact(int fd, void *buf, size_t len, int timeout_ms)
{
    uint8_t *p = buf;
    size_t got = 0;

    while (got < len) {
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        int r = select(fd + 1, &rfds, NULL, NULL, &tv);
        if (r < 0) return -1;
        if (r == 0) {
            // timeout
            return -2;
        }
        ssize_t rd = read(fd, p + got, len - got);
        if (rd < 0) return -1;
        if (rd == 0) {
            // no more data
            return -2;
        }
        for (size_t i = 0; i < rd; i++) {
            printf("RX: 0x%02X ('%c')\n", p[got + i], (p[got + i] >= 32 && p[got + i] <= 126) ? p[got + i] : '.');
        }
        got += rd;
        // continue until len
    }
    return 0;
}

void clear_message(uint8_t *pframe)
{
    memset(pframe, 0, sizeof(uint8_t) * FRAME_SIZE);
    pframe[0] = MAGIC0;
    pframe[1] = MAGIC1;
    pframe[2] = MAGIC2;
    pframe[3] = MAGIC3;
}

void prepare_frame(uint8_t *pframe)
{
    int crc = calculateCRC16(0xFFFF, pframe, SEND_BUFFER_BYTE_INDEX_CRC);
    pframe[SEND_BUFFER_BYTE_INDEX_CRC] = crc & 0xFF;
    pframe[SEND_BUFFER_BYTE_INDEX_CRC + 1] = (crc & 0xFF00) >> 8;
    printf("Frame last two bytes (CRC): 0x%02X 0x%02X\n", pframe[FRAME_SIZE - 2], pframe[FRAME_SIZE - 1]);
}

void setCommandType(uint8_t *pframe, uint8_t cmd)
{
    pframe[4] = cmd;
}

int bootloader_v2_command(int fd, uint8_t cmd, const uint8_t *payload, size_t payload_len, uint8_t *response, int timeout_ms)
{
    if (payload_len > 4096)
        return -1;

    uint8_t frame[FRAME_SIZE];
    clear_message(frame);
    setCommandType(frame, cmd);
    prepare_frame(frame);

    //frame[5] = 0x00; // is_response = 0 (command)
    //frame[6] = payload_len & 0xFF;
    //frame[7] = (payload_len >> 8) & 0xFF;
    //if (payload != NULL && payload_len > 0)
    //    memcpy(&frame[PAYLOAD_OFFSET], payload, payload_len);



    if (write_all(fd, frame, FRAME_SIZE, timeout_ms) != 0)
        return -2;

    if (!response)
        return 0;

    int r = read_exact(fd, response, FRAME_SIZE, timeout_ms);
    if (r == -2) {
        printf("read timeout\n");
        return -6;
    } else if (r < 0) {
        perror("read");
        return -7;
    }
    if (r != 0)
        return -3;

    if (response[0] != MAGIC0 || response[1] != MAGIC1 ||
        response[2] != MAGIC2 || response[3] != MAGIC3)
        return -4;

    uint16_t resp_crc = response[4104] | (response[4105] << 8);
    uint16_t calc = calculateCRC16(0xFFFF, response, SEND_BUFFER_BYTE_INDEX_CRC);
    if (resp_crc != calc)
    {
        printf("Invalid response crc.\n");
        return -5;
    }
    return 0;
}

typedef enum {
    BOOTLOADER_UNKNOWN = 0,
    BOOTLOADER_V1,
    BOOTLOADER_V2
} BootloaderVersion;

BootloaderVersion bootloaderVersionDetectWaitForGetFastMethod(int fd)
{
    // Enter programming mode (bootloader mode)
    enter_programming_mode();

    // Clear input buffer
    tcflush(fd, TCIFLUSH);

    // Sequence to detect V1: "GET 512"
    const uint8_t v1_seq[7] = {71, 69, 84, 32, 53, 49, 50};
    /* Sequence in hex: 0x47, 0x45, 0x54, 0x20, 0x35, 0x31, 0x32 */
    // V1 sequence as ASCII: 'G','E','T',' ','5','1','2'
    // V1 sequence as string: "GET 512"
    BootloaderVersion bootloaderVersion = BOOTLOADER_UNKNOWN;

    for (int attempt = 0; attempt < 3 && bootloaderVersion == BOOTLOADER_UNKNOWN; attempt++)
    {
        reset_mcu();
        int matched = 0;
        uint8_t ch;
        unsigned long start = (unsigned long)time(NULL);
        while (((unsigned long)time(NULL) - start) < 12) { // 12 seconds timeout
            usleep(100000); // 100ms
            while (read(fd, &ch, 1) == 1) {
                printf("%c", (ch >= 32 && ch <= 126) ? ch : '.');
                if (ch == v1_seq[matched]) {
                    printf("- Matched %d\n", matched);
                    matched++;
                    if (matched == 7) {
                        bootloaderVersion = BOOTLOADER_V1;
                        break;
                    }
                } else {
                    matched = 0;
                }
            }
            if (bootloaderVersion == BOOTLOADER_V1)
                break;
        }
        /*
        if (bootloaderVersion == BOOTLOADER_UNKNOWN) {
            reset_mcu();
            usleep(100000); // 100ms
            // Try V2 detection: send GET_STATE command and expect valid response
            uint8_t resp[FRAME_SIZE];
            int r = bootloader_v2_command(fd, CMD_GET_STATE, NULL, 0, resp, 6000);
            printf("V2 detection bootloader_v2_command returned: %d\n", r);
            if (r == 0) {
                bootloaderVersion = BOOTLOADER_V2;
            }
        }*/
    }
    if (bootloaderVersion == BOOTLOADER_V1) {
        printf("Detected bootloader version: V1\n");
    } else if (bootloaderVersion == BOOTLOADER_V2) {
        printf("Detected bootloader version: V2\n");
    } else {
        printf("Bootloader version: UNKNOWN\n");
    }
    // Exit programming mode
    exit_programming_mode();

    return bootloaderVersion;
}

/**
 * Usage: sudo ./vendor_programing_tool "filename" ttyport
 */
int main(int argc, char **argv)
{
    const char *dev = "/dev/ttyS2";
    int baud = 115200;
    char filename[1024] = {0};

    if (argc >= 2)
    {
        strncpy(filename, argv[1], sizeof(filename) - 1);
    }
    if (argc >= 3)
    {
        dev = argv[2];
    }
    printf("Filename: %s\n", filename);
    printf("Device: %s\n", dev);
    printf("Baud: %d\n", baud);

    int serial_port = open_serial(dev, baud);
    if (serial_port < 0)
    {
        return 2;
    }

    set_reset_pin(1);
    set_boot_pin(0);

    bootloaderVersionDetectWaitForGetFastMethod(serial_port);
    exit(0);
/*
    uint8_t frame[FRAME_SIZE];
    memset(frame, 0, sizeof(frame));
    frame[0] = MAGIC0; frame[1] = MAGIC1; frame[2] = MAGIC2; frame[3] = MAGIC3;
    frame[4] = CMD_GET_STATE;   // command id
    frame[5] = 0x00;            // is_response = 0 (command)
    frame[6] = 0x00; frame[7] = 0x00; // data size (little endian)
    // payload left zero

    uint16_t crc = crc16_ccitt(frame, PAYLOAD_OFFSET + 4096 + 0); // bytes 0..4103
    // write CRC little endian at offset 4104
    frame[4104] = crc & 0xFF;
    frame[4105] = (crc >> 8) & 0xFF;

    if (write_all(fd, frame, FRAME_SIZE, 2000) != 0) {
        fprintf(stderr, "write timeout or error\n");
        close(fd);
        return 3;
    }

    // wait for response frame
    uint8_t resp[FRAME_SIZE];
    int r = read_exact(fd, resp, FRAME_SIZE, 6000); // 6s
    if (r == -2) {
        fprintf(stderr, "read timeout\n");
        close(fd);
        return 4;
    } else if (r != 0) {
        perror("read");
        close(fd);
        return 5;
    }

    // validate magic
    if (resp[0] != MAGIC0 || resp[1] != MAGIC1 || resp[2] != MAGIC2 || resp[3] != MAGIC3) {
        fprintf(stderr, "invalid magic in response\n");
        close(fd);
        return 6;
    }

    // validate CRC
    uint16_t resp_crc = resp[4104] | (resp[4105] << 8);
    uint16_t calc = crc16_ccitt(resp, 4104);
    if (resp_crc != calc) {
        fprintf(stderr, "CRC mismatch: resp=0x%04X calc=0x%04X\n", resp_crc, calc);
        // continue, maybe different CRC variant used
    } else {
        printf("CRC OK\n");
    }

    printf("response cmd=%#02x is_response=%u\n", resp[4], resp[5]);
    // dump first 32 bytes of payload
    printf("payload (first 32 bytes):\n");
    for (int i = 0; i < 32; i++) {
        printf("%02X ", resp[PAYLOAD_OFFSET + i]);
        if ((i & 15) == 15) printf("\n");
    }
    printf("\n");

    // interpret likely state value at payload[0] (vendor-dependent)
    uint8_t state = resp[PAYLOAD_OFFSET + 0];
    printf("bootloader reported state: 0x%02X (%u)\n", state, state);

    close(fd);
    */
    return 0;
}
