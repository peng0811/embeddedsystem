/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "TCPSocket.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);
#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif
/*
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}
*/
/*
int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("Scan:\n");
    int count = wifi->scan(NULL,0);
    printf("%d networks available.\n", count);
*/
    /* Limit number of network arbitrary to 15 */
/*    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);
    for (int i = 0; i < count; i++)
    {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }

    delete[] ap;
    return count;
}
*/

void http_demo(NetworkInterface *net,char *buf)
{
    TCPSocket socket;
    nsapi_error_t response;

    printf("Sending HTTP request to www.arm.com...\n");

    // Open a socket on the network interface, and create a TCP connection to www.arm.com
    // Show the network address
    SocketAddress a;
    net->get_ip_address (&a);
    printf("IP address: %s n", a.get_ip_address () ? a.get_ip_address () : "None");
    printf("Sending HTTP request to www.arm. n");

    socket.open(net);
    net->gethostbyname ("192.168.1.236", &a);
    a.set_port(8000);
    response = socket.connect (a);
    if(0 != response) {
        printf("Error connecting: %d\n", response);
        socket.close();
        return;
    }
    // Send a simple http request
    //char sbuffer[] = "GET / HTTP/1.1\r\nHost: 192.168.1.236\r\n\r\n";
    //nsapi_size_t size = strlen(sbuffer);
    char sbuffer[10];
    for(int i=0; i<10; i++){
        sbuffer[i] = buf[i];
    }
    nsapi_size_t size = strlen(sbuffer);
    socket.send(sbuffer, size);
    response = 0;
    while(size)
    {
        response = socket.send(sbuffer+response, size);
        if (response < 0) {
            printf("Error sending data: %d\n", response);
            socket.close();
            return;
        } else {
            size -= response;
            // Check if entire message was sent or not
            //printf("sent %d [%.*s]\n", response, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);
        }
    }
    socket.close();
    // Recieve a simple http response and print out the response line
    //char rbuffer[64];
    //response = socket.recv(rbuffer, sizeof rbuffer);
    //if (response < 0) {
    //    printf("Error receiving data: %d\n", response);
    //} else {
    //    printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
    //}

    // Close the socket to return its memory and bring down the network interface

}

void dtostrf(double val, int len, int prec, char* buf, int buf_len) {
    int int_count = 0;
    int index = 0;
    //判斷正負
    if (val < 0) {
        buf[index] = '-';
        index ++;
        val = -val;
    }
    //先數一下整數部分長度
    while ((int) val > 0) {
        val /= 10.0;
        int_count++;
    }
    while (index < len && index < buf_len) {
    	//如果整數部分結束
        if (int_count == 0) {
        	//如果一上來就是0
            if (index == 0) {
                buf[index] = '0';
                index++;
            }
            else {
                buf[index] = '.';
                index++;
                int_count--;
            }
        }
        //int_count此時已經是小數長度的相反數
        else if ( int_count >= -prec) {
            val *= 10.0;
            buf[index] = (int)val + '0';
            val -= (double)((int)val);
            index++;
            int_count--;
        }
        else break;
    }
    //強制加'\0'
    if (index == buf_len)  buf[index - 1] = '\0';
    else buf[index] = '\0';
}



int main()
{
    int count = 0;
    float sensor_value = 0;
    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};
    int k = 0;
    printf("Start sensor init\n");

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();

    while(1) {

        sensor_value = BSP_TSENSOR_ReadTemp();
        printf("\nTEMPERATURE = %.2f degC\n", sensor_value);

        char buf[10] = "aefeag";
   	    dtostrf(sensor_value, 10, 2, buf, 10);

        sensor_value = BSP_HSENSOR_ReadHumidity();
        printf("HUMIDITY    = %.2f %%\n", sensor_value);
        
        sensor_value = BSP_PSENSOR_ReadPressure();
        printf("PRESSURE is = %.2f mBar\n", sensor_value);

        ThisThread::sleep_for(1s);

        BSP_MAGNETO_GetXYZ(pDataXYZ);
        printf("\nMAGNETO_X = %d\n", pDataXYZ[0]);
        printf("MAGNETO_Y = %d\n", pDataXYZ[1]);
        printf("MAGNETO_Z = %d\n", pDataXYZ[2]);
        int num = pDataXYZ[0];
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        printf("\nGYRO_X = %.2f\n", pGyroDataXYZ[0]);
        printf("GYRO_Y = %.2f\n", pGyroDataXYZ[1]);
        printf("GYRO_Z = %.2f\n", pGyroDataXYZ[2]);

        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
        printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
        printf("ACCELERO_Z = %d\n", pDataXYZ[2]);
 
        int i=0,j=0;
        char temp[10],str[10];
        while(num)
        {
            temp[i++]=num%10+'0';  //將數字加字元0就變成相應字元
            num/=10;               //此時的字串為逆序
        }
        temp[i]='\0';
        i=i-1;
        while(i>=0)
            str[j++]=temp[i--];   //將逆序的字串轉為正序
        str[j]='\0';              //字串結束標誌

        ThisThread::sleep_for(1s);

        printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
        int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);

        printf("Success\n\n");
        printf("MAC: %s\n", wifi.get_mac_address());
        printf("IP: %s\n", wifi.get_ip_address());
        printf("Netmask: %s\n", wifi.get_netmask());
        printf("Gateway: %s\n", wifi.get_gateway());
        printf("RSSI: %d\n\n", wifi.get_rssi());

        http_demo(&wifi,buf);

        k++;
        if(k>80){
            break;
        }

    }

    printf("WiFi example\n\n");

    wifi.disconnect();

    printf("\nDone\n");
}
