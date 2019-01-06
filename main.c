/*
 * main.c
 *
 *  Created on: 02 gen 2019
 *      Author: andrea
 */
#include <stdint.h>
#include <syslog.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pru_mylinuxdrone.h>


#define MPU6050_DEVICE_NAME             "/dev/iio:device1"
#define MPU6050_BUFF_SIZE 12
unsigned char mpu6050Data[sizeof(PrbMessageType)] = {0};
PrbMessageType* mpu6050DataStruct = (PrbMessageType*)mpu6050Data;

struct  timeval now; // wall clock times
struct  timeval later;
uint32_t usec;

FILE *fd;
unsigned char readBuff[MPU6050_BUFF_SIZE] = {0};

void openMpu6050()
{
        syslog(LOG_INFO, "open device mpu6050 ...");

        /* Open the rpmsg_pru character device file */
        fd = fopen(MPU6050_DEVICE_NAME, "r");

        if (fd  == NULL)
        {
            syslog(LOG_ERR, "Failed to open [%s] ",
                   MPU6050_DEVICE_NAME);
        }
}
void closeMpu6050() {
    fclose(fd);
}
void readMpu6050() {
    fread(mpu6050Data, sizeof(PrbMessageType), 1, fd);
}

int main(void)
{
    syslog(LOG_INFO, "PrbMessageType size=[%d]", sizeof(PrbMessageType));

    gettimeofday(&now, NULL); // wall clock time when CPU time first read
    gettimeofday(&later, NULL); // wall clock time when CPU time has ticked
    openMpu6050();
    for(int i = 0; i < 100000; i++) {
        readMpu6050();
        gettimeofday(&later, NULL); // wall clock time when CPU time has ticked
        usec = (((unsigned long long)later.tv_sec) * 1000000ULL + later.tv_usec) - (((unsigned long long)now.tv_sec) * 1000000ULL + now.tv_usec);
        if(usec > 1200) {
            syslog(LOG_INFO, "usec[%d], a[%d,%d,%d], g[%d,%d,%d]", usec,
                   mpu6050DataStruct->mpu_accel_gyro.ax,
                   mpu6050DataStruct->mpu_accel_gyro.ay,
                   mpu6050DataStruct->mpu_accel_gyro.az,
                   mpu6050DataStruct->mpu_accel_gyro.gx,
                   mpu6050DataStruct->mpu_accel_gyro.gy,
                   mpu6050DataStruct->mpu_accel_gyro.gz
                               );
        }
        gettimeofday(&now, NULL); // wall clock time when CPU time first read

    }
    closeMpu6050();
}
