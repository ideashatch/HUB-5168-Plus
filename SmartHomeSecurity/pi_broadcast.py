# 該程式負責接收廣播，若收到會回傳樹莓派的IP位址
# 以供其他設備能正常連線至樹莓派
# pip install netifaces
import socket
import netifaces as ni

broadcast_ip = '255.255.255.255'
broadcast_port = 12345

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
server_socket.bind(('', broadcast_port))

wlan0_ip = ni.ifaddresses('wlan0')[ni.AF_INET][0]['addr']

print("等待客戶端...")
while True:
    data, address = server_socket.recvfrom(1024)
    print("接收到来自 {} 的廣播：{}".format(address, data.decode()))

    if data.decode() == "RASPBERRY_PI_IP_REQUEST":
        response_data = "RASPBERRY_PI_IP:" + wlan0_ip
        for i in range(0, 3):
            server_socket.sendto(response_data.encode(), address)

        print("回復客戶端：{}".format(response_data)+" 3次")
