#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include "../Fusion/Fusion.h"

#define SAMPLE_PERIOD (0.1f) // replace this with actual sample period
#define MAX 100
#define PORT 5000
#define SA struct sockaddr

void receive_and_process(int sockfd) {
    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);
    int count;
    float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
    char buff[MAX];

    for (;;) {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("%s", buff);
        sscanf(buff, "{%d %f %f %f %f %f %f}\n",
               &count, &acc_x, &acc_y, &acc_z, &gyro_x, &gyro_y, &gyro_z);

        const FusionVector gyroscope = {gyro_x, gyro_y, gyro_z}; // replace this with actual gyroscope data in degrees/s
        const FusionVector accelerometer = {acc_x, acc_y, acc_z}; // replace this with actual accelerometer data in g
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
    }
}
 
int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket failed to create\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.0.3");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("Connect failed\n");
        exit(0);
    }
    receive_and_process(sockfd);
    close(sockfd);
    return 0;
}


// int main() {
//     FusionAhrs ahrs;
//     FusionAhrsInitialise(&ahrs);

//     while (true) { // this loop should repeat each time new gyroscope data is available
//         const FusionVector gyroscope = {0.0f, 0.0f, 0.0f}; // replace this with actual gyroscope data in degrees/s
//         const FusionVector accelerometer = {0.0f, 0.0f, 1.0f}; // replace this with actual accelerometer data in g

//         FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);

//         const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

//         printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
//     }
// }
