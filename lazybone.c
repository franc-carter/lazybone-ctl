
/*
   Copyright {2014} {Franc Carter}

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

void usage()
{
    fprintf(stdout, "Usage: lazybone <address> <off|on>\n");
    fprintf(stdout, "       <address> - the bluetooth address of the lazybone\n");
    fprintf(stdout, "       off       - turn the device off . . .\n");
    fprintf(stdout, "       on        - turn the device on  . . .\n");
    fprintf(stdout, "       status    - print the on/off status to stdout\n");
    exit(1);
}

void fail(char *msg, int fd, int ret)
{
    close(fd);
    perror(msg);
    exit(ret);
}

int main(int argc, char **argv)
{
    struct sockaddr_rc baddr = { 0 };
    int bsock, status;
    unsigned char onoff;
    char dest[18] = "XX:XX:XX:XX:XX:XX";

    if (argc != 3)
        usage();

    strncpy(dest, argv[1], 18);
    if (!strncmp(argv[2], "off", 3))
        strncpy(argv[2], "o", 2);
    else if (!strncmp(argv[2], "on", 2))
        strncpy(argv[2], "e", 2);
    else if (!strncmp(argv[2], "status", 6))
        strncpy(argv[2], "[", 2);
    else
        usage();

    bsock            = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    baddr.rc_channel = (uint8_t)1;
    baddr.rc_family  = AF_BLUETOOTH;
    str2ba(dest, &baddr.rc_bdaddr);

    status = connect(bsock, (struct sockaddr *)&baddr, sizeof(baddr));
    if (status < 0)
        fail("Bluetooth - connect() failed: ", bsock, status);

    status = write(bsock, argv[2], 1);
    if (status < 0)
        fail("Bluetooth - write() failed: ", bsock, status);
    if (argv[2][0] == '[') {
        status = read(bsock, &onoff, 1);
        if (status != 1)
            fail("Bluetooth - read() failed: ", bsock, status);
        if (onoff == 1)
            fprintf(stdout, "on\n");
        else
            fprintf(stdout, "off\n");
    }

    return 0;
}

