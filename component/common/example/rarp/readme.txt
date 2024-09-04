RARP EXAMPLE

Description:
In this example, it will send rarp request for 3 times. And the rarp reply callback will print the received reply IP address.

Configuration:
[platform_opts.h]
    #define CONFIG_EXAMPLE_RARP    1
	
Execution:
You need to connect to WiFi manually by AT command. After connected the rarp request will be sent.

If you are using VirtualBox, you may want to change the network adapter settings:
Attached to: [Bridged Adapter]

You may want to update your ubuntu firewall configuration:
sudo apt install ufw
sudo ufw allow 2222/tcp
sudo ufw allow 2323/tcp
sudo ufw allow from AmebaZ2_IP_Address to any port 219
sudo ufw allow from Server_IP_Address to any port 219

RARP server:
For Ubuntu, you can try to setup the RARP server as doing this:
apt-get install rarpd
touch /etc/ethers
Add	Ameba's MAC address such as "00:e0:4c:87:00:09	client"		in /etc/ethers
Add	Ameba's IP  address such as "192.168.0.100			client"		in /etc/hosts
sudo rarpd -A
Note:
IPv4 uses rarp while IPv6 uses neighbor discovery (ND) packets to find out the hardware MAC address.

[Supported List]
	Supported :
	    Ameba-1, Ameba-z, Ameba-pro, Ameba-z2