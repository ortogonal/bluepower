#include "cadenceio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <QDebug>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64
#define POLL_TIMEOUT (10 * 1000) /* 10 seconds */

CadenceIO::CadenceIO(QObject *parent) : QThread(parent)
{
    exportGpio(5);
    m_timer.setInterval(5000);
    m_timer.start();
    m_timer.moveToThread(this);

    connect(&m_timer, &QTimer::timeout, this, [&]() {
        emit cadence(0);
    });
}

void CadenceIO::exportGpio(int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/direction");
        return;
    }

    write(fd, "in", 3);

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-edge");
        return;
    }

    write(fd, "falling", 8);

    return;
}

int CadenceIO::gpioOpen(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("gpio/fd_open");
        return -1;
    }
    return fd;
}

void CadenceIO::run()
{
    struct pollfd fdset[1];
    int nfds = 1;
    int gpio_fd, timeout, rc;
    char *buf[MAX_BUF];
    unsigned int gpio = 5;
    int len;

    timeout = POLL_TIMEOUT;

    gpio_fd = gpioOpen(gpio);

    while (1) {
        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = gpio_fd;
        fdset[0].events = POLLPRI | POLLERR;

        rc = poll(fdset, nfds, timeout);

        if (rc < 0) {
            printf("\npoll() failed!\n");
            return;
        }

        if (rc == 0) {
            //printf(".");
            emit cadence(0.f);
        }

        if (fdset[0].revents & POLLPRI) {
            lseek(fdset[0].fd, 0, SEEK_SET);
            len = read(fdset[0].fd, buf, MAX_BUF);
            //printf("\npoll() GPIO %d interrupt occurred\n", gpio);

            // No one chan havea revulation that is below 250 ms
            if (m_int.isValid() && m_int.elapsed() > 250) {
                int elapsed = m_int.elapsed();
                float c = (1000.f / (float)elapsed) * 60.f;

                // Make sure we don't but out to much power
                // For some reasone the cadence can be a bit to high
                if (c > 175.f) {
                    c = 175.f;
                }

                m_timer.start();

                emit cadence(c);
            }
            m_int.restart();
        }

        if (fdset[0].revents & POLLIN) {
            (void)read(fdset[0].fd, buf, 1);
            printf("\npoll() stdin read 0x%2.2X\n", buf[0]);
        }

        fflush(stdout);
    }
}
