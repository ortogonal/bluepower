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
    struct pollfd fdset[2];
    int nfds = 2;
    int gpio_fd, timeout, rc;
    char *buf[MAX_BUF];
    unsigned int gpio = 5;
    int len;

    timeout = POLL_TIMEOUT;

    gpio_fd = gpioOpen(gpio);

    while (1) {
        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = STDIN_FILENO;
        fdset[0].events = POLLIN;

        fdset[1].fd = gpio_fd;
        fdset[1].events = POLLPRI;

        rc = poll(fdset, nfds, timeout);

        if (rc < 0) {
            printf("\npoll() failed!\n");
            return;
        }

        if (rc == 0) {
            //printf(".");
            emit cadence(0.f);
        }

        if (fdset[1].revents & POLLPRI) {
            lseek(fdset[1].fd, 0, SEEK_SET);
            len = read(fdset[1].fd, buf, MAX_BUF);
            //printf("\npoll() GPIO %d interrupt occurred\n", gpio);

            if (m_int.isValid() & m_int.elapsed() > 0) {
                int elapsed = m_int.elapsed();
                float c = (1000.f / (float)elapsed) * 60.f;
                qDebug() << "Cadence:" << c;
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
