#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyACM0"
#define BAUDRATE B115200 // 波特率，与串口模块配置一致
#define DELAY_US 1000000 // 发送间隔，单位微秒

int main()
{
    int fd;
    struct termios options;

    // 打开串口
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
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

    // 循环发送数据
    char write_buffer[256];
    while (1)
    {
        // 获取用户输入
        printf("Enter message to send (max 255 characters): ");
        fgets(write_buffer, sizeof(write_buffer), stdin);

        // 移除换行符
        write_buffer[strcspn(write_buffer, "\n")] = 0;

        // 发送数据
        int bytes_written = write(fd, write_buffer, strlen(write_buffer));
        if (bytes_written < 0)
        {
            perror("write failed");
            close(fd);
            return 1;
        }
        printf("Sent: %s\n", write_buffer);

        usleep(DELAY_US); // 发送间隔
    }

    // 关闭串口
    close(fd);

    return 0;
}
