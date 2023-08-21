#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/tty.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int s;
    unsigned char number=0;
    int ldisc = N_SLLIN; // N_SLLIN_SLAVE		
    int tty_fd;
    char tty_dev[] = "/dev/ttyLP3"; // Uart 3
    char dev_name[] = "sllin0";
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame_send;
    struct can_frame frame;
    int i;
    int nbytes;
    pid_t pid = -1;

    if ((tty_fd = open (tty_dev, O_WRONLY | O_NOCTTY)) < 0) {
        perror(tty_dev);
        return -1;
	}

    /* set line discipline on given tty */
    if (ioctl (tty_fd, TIOCSETD, &ldisc) < 0) {
        perror("ioctl TIOCSETD");
        return -1;
    }

    printf("line discipline set to %d\n", ldisc);

    /* retrieve the name of the created LIN netdevice */
    if (ioctl (tty_fd, SIOCGIFNAME, ifr.ifr_name) < 0) {
        perror("ioctl SIOCGIFNAME");
        return -1;
    }

    printf("attached tty %s to netdevice %s\n", tty_dev, ifr.ifr_name);

    system("ifconfig sllin0 txqueuelen 1000");

    system("ip link set sllin0 up");

    printf("ip link set %s up\n", ifr.ifr_name);

    /* create socket */
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Create socket failed");
        exit(-1);
    }

    /* set up can interface */
    strcpy(ifr.ifr_name, dev_name);
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
        exit(-1);
	}
    printf("lin port is %s\n",ifr.ifr_name);

    /* assign can device */
    // ioctl(s, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* bind can device */
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind lin device failed\n");
        close(s);
        exit(-2);
    }

    pid = fork();

    if (pid == -1) {
        perror("\n\tfailed to fork!!\n\n");
        return  -1;
    } else if (pid==0) { /* configure receiving can data*/
	    /* keep reading */
	    while(1)
	    {
		    nbytes = read(s, &frame, sizeof(frame));
		    if (nbytes > 0)
		    {
			    printf("read datas:%s ID=%#x data length=%d\n", ifr.ifr_name, frame.can_id, frame.can_dlc);
			    for ( i=0; i < frame.can_dlc; i++)
			    {
				    printf("%#x ", frame.data[i]);
			    }
			    printf("\n");
		    }
		    printf("read LIN data over\n");
	    }
    } else { /* configure sending can data*/
	    while(1) {
		    /* configure can_id and can data length */
		    frame_send.can_id = 0x10;
		    frame_send.can_dlc = 8;
		    printf("%s ID=%#x data length=%d\n", ifr.ifr_name, frame_send.can_id, frame_send.can_dlc);
		    /* prepare data for sending: 0x11,0x22...0x88 */
		    for (i=0; i<8; i++)
		    {
			    frame_send.data[i] = ((i+1)<<4) | (i+1);
			    frame_send.data[7] =number;
			    printf("%#x ", frame_send.data[i]);
		    }
		    printf("success to Sent out\n");
		    /* Sending data */
		    if (write(s, &frame_send, sizeof(frame_send)) < 0)
		    {
			    perror("Send failed");
			    close(s);
			    exit(-4);
		    }
		    sleep(1);
		    number++;
	    }
    }

    close(s);
    return 0;
}
