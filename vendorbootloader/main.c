// Minimal Bootloader V1 client.
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
#include <ctype.h>
#include <sys/stat.h>


#define END_SIGNAL -1000

// Reset pin, LED PF1
// found in gpio_manager_script.sh
#define RESET_PIN "/sys/devices/platform/leds/leds/nrst:usr/brightness"

// Boot found in gpio_manager_script.sh
#define BOOT_PIN "/sys/devices/platform/leds/leds/boot:usr/brightness"

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

// helper: drain any pending input
static void drain_input(int fd) {
    uint8_t tmp[256];
    ssize_t r;
    while ((r = read(fd, tmp, sizeof(tmp))) > 0) { /* discard */ }
    (void)r;
}

// helper: write all bytes to fd
static int write_all_fd(int fd, const uint8_t *buf, size_t len, int timeout_ms) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t w = write(fd, buf + sent, len - sent);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += w;
    }
    tcdrain(fd); // ensure kernel pushes out (optional)
    return 0;
}
/*
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
}*/

// parse and wait for "GET <N>\n" or "END" within timeout_ms (ms).
// returns: >0 number requested, END_SIGNAL for END, -1 timeout, -2 parse error, -3 other error
static int get_bytes_request(int fd, int timeout_ms)
{
    char buf[1024];
    size_t pos = 0;
    long start = (long)time(NULL) * 1000L;
    int timed_out = 0;

    while (!timed_out) {
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        int r = select(fd + 1, &rfds, NULL, NULL, &tv);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -3;
        }
        if (r == 0) {
            return -1; // timeout
        }
        ssize_t rd = read(fd, buf + pos, sizeof(buf) - 1 - pos);
        if (rd <= 0) {
            if (rd == 0) return -1;
            if (errno == EINTR) continue;
            return -3;
        }
        // accept only printable/digits/space/CR/LF
        size_t got = 0;
        for (ssize_t i = 0; i < rd; ++i) {
            unsigned char c = (unsigned char)buf[pos + got];
            buf[pos + got] = c; // already read
            got++;
        }
        pos += rd;
        if (pos >= sizeof(buf) - 1) {
            pos = sizeof(buf) - 2;
        }
        buf[pos] = '\0';

        // check for "END " or "ENDE\n" or "END" token
        if (strstr(buf, "END ") || strstr(buf, "ENDE\n") || strstr(buf, "END\n")) {
            return END_SIGNAL;
        }
        // find "GET "
        char *g = strstr(buf, "GET ");
        if (g) {
            // parse digits after "GET "
            char *p = g + 4;
            // skip optional spaces
            while (*p == ' ') p++;
            if (!isdigit((unsigned char)*p)) return -2;
            long val = 0;
            while (isdigit((unsigned char)*p)) {
                val = val * 10 + (*p - '0');
                p++;
            }
            // require terminating newline for safety
            if (*p == '\n' || *p == '\r' || *p == '\0') {
                return (int)val;
            } else {
                // continue reading until newline appears
            }
        }
        // small sleep to let more bytes arrive (avoid busy loop)
        struct timespec ts = {0, 10 * 1000 * 1000}; // 10ms
        nanosleep(&ts, NULL);

        // check overall timeout (safer)
        long now = (long)time(NULL) * 1000L;
        if (now - start > timeout_ms) timed_out = 1;
    }
    return -1;
}

// program_mcu: returns status codes: 0 OK, 2 open/file error, 5 timeout, 100 done
int program_mcu(const char *filename, int serial_dev)
{
    struct stat st;
    if (stat(filename, &st) != 0) return 4; // file not found
    off_t filesize = st.st_size;
    FILE *f = fopen(filename, "rb");
    if (!f) return 4;

    // drain any old bytes
    drain_input(serial_dev);

    // enter programming mode and reset MCU
    enter_programming_mode();

    off_t file_counter = 0;
    int timeout_retries = 0;
    int seen_any_send = 0;

    while (1) {
        // Gets the number of bytes to be sent to the bootloader
        int req = get_bytes_request(serial_dev, 15000); // 15s timeout
        if (req == -1) {
            // timeout
            if (++timeout_retries > 3) {
                fclose(f);
                close(serial_dev);
                exit_programming_mode();
                return 5; // TIMEOUT
            }
            // retry: reset once, then keep trying
            if (!seen_any_send)
            {
                reset_mcu();
            }
            continue;
        } else if (req == END_SIGNAL) {
            // finished
            fclose(f);
            close(serial_dev);
            exit_programming_mode();
            return 100; // DONE
        } else if (req <= 0) {
            // protocol error
            fclose(f);
            exit_programming_mode();
            close(serial_dev);
            return 2; // IO/protocol error
        } else {
            // req > 0
            if (file_counter >= filesize) {
                // no more bytes to send, reset retry counter and continue
                timeout_retries = 0;
                continue;
            }
            // Send the next bytes
            size_t to_send = (size_t)req;
            if ((off_t)to_send > (filesize - file_counter)) {
                to_send = (size_t)(filesize - file_counter);
            }
            uint8_t *buf = malloc(to_send);
            if (!buf) {
                fclose(f);
                close(serial_dev);
                exit_programming_mode();
                return 2;
            }
            size_t readed = fread(buf, 1, to_send, f);
            if (readed != to_send) {
                // unexpected read length
                free(buf);
                fclose(f);
                close(serial_dev);
                exit_programming_mode();
                return 2;
            }
            // Send the data to the bootloader
            if (write_all_fd(serial_dev, buf, to_send, 2000) != 0) {
                free(buf);
                fclose(f);
                close(serial_dev);
                exit_programming_mode();
                return 2;
            }
            free(buf);
            file_counter += to_send;
            seen_any_send = 1;
            timeout_retries = 0;
            // optionally report progress: (int)round(100.0 * file_counter / filesize)
        }
    }
    // unreachable
    return 2;
}

typedef enum {
    BOOTLOADER_UNKNOWN = 0,
    BOOTLOADER_V1
} BootloaderVersion;

BootloaderVersion bootloaderVersionDetect(int fd)
{
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
    }
    if (bootloaderVersion == BOOTLOADER_V1) {
        printf("Detected bootloader version: V1\n");
    } else {
        printf("Bootloader version: UNKNOWN\n");
    }
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

    //bootloaderVersionDetectWaitForGetFastMethod(serial_port);
    int ret = program_mcu(filename, serial_port);
    switch (ret) {
        case 0:
            printf("Programming OK\n");
            break;
        case 2:
            printf("IO or protocol error\n");
            break;
        case 4:
            printf("File not found\n");
            break;
        case 5:
            printf("Timeout\n");
            break;
        case 100:
            printf("Programming DONE\n");
            break;
        default:
            printf("Unknown return value: %d\n", ret);
            break;
    }
    close(serial_port);
    return 0;
}
