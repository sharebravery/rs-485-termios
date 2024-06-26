#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyACM0"
#define BAUDRATE B115200 // 波特率，与串口模块配置一致

int main()
{
    int fd;
    struct termios options;

    // 打开串口
    fd = open(SERIAL_PORT, O_RDWR);
    if (fd == -1)
    {
        perror("open_port: Unable to open port");
        return 1;
    }

    // 获取当前串口配置
    if (tcgetattr(fd, &options) != 0)
    {
        perror("tcgetattr failed");
        close(fd);
        return 1;
    }

    // 设置波特率
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);

    // 无校验，8位数据位，1位停止位
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // 设置为本地连接，使能接收
    options.c_cflag |= (CLOCAL | CREAD);

    // 设置为原始模式
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // 禁用软件流控制
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // 禁用硬件流控制
    options.c_cflag &= ~CRTSCTS;

    // 设置新的串口设置
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("tcsetattr failed");
        close(fd);
        return 1;
    }

    // 循环接收数据
    while (1)
    {
        char read_buffer[256];
        int bytes_read = read(fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read < 0)
        {
            perror("read failed");
            close(fd);
            return 1;
        }
        read_buffer[bytes_read] = '\0';
        printf("Received: %s\n", read_buffer);
    }

    // 关闭串口
    close(fd);

    return 0;
}
