#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void DiscoverSSDPDevices(vector<string>& devices) {
  // Create a socket
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == INVALID_SOCKET) {
    cerr << "Error creating socket" << endl;
    exit(1);
  }

  // Bind the socket to the multicast address
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1900);
  addr.sin_addr.s_addr = inet_addr("239.255.255.250");

  if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    cerr << "Error binding socket" << endl;
    exit(1);
  }

  // Send an SSDP discovery message
  string msg = "M-SEARCH * HTTP/1.1\r\n"
              "HOST: 239.255.255.250:1900\r\n"
              "ST: ssdp:all\r\n\r\n";

  sendto(sock, msg.c_str(), msg.length(), 0, (SOCKADDR*)&addr, sizeof(addr));

  // Receive SSDP responses
  char buf[1024];
  int r = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
  while (r > 0) {
    // Parse the SSDP response
    string response(buf, r);
    string location = response.substr(response.find("Location:") + 10);
    location = location.substr(0, location.find("\r\n"));

    // Add the device IP address to the list of discovered devices
    devices.push_back(location);

    // Receive the next SSDP response
    r = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
  }

  // Close the socket
  closesocket(sock);
}

void ForwardPort(const char* device_ip, int internal_port, int external_port) {
  // Create a socket
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == INVALID_SOCKET) {
    cerr << "Error creating socket" << endl;
    exit(1);
  }

  // Bind the socket to the multicast address
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1900);
  addr.sin_addr.s_addr = inet_addr("239.255.255.250");

  if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    cerr << "Error binding socket" << endl;
    exit(1);
  }

  // Send an SSDP UPnP port mapping add request
  string msg = "POST 192.168.11.1/WANIPConnection/PortMapping/Add HTTP/1.1\r\n"
              "HOST: 192.168.11.1\r\n"
              "CONTENT-TYPE: text/xml\r\n"
              "CONTENT-LENGTH: 512\r\n\r\n"
              "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
              "<AddPortMapping>\n"
              " <NewRemoteHost></NewRemoteHost>\n"
              " <NewExternalPort>" + to_string(external_port) + "</NewExternalPort>\n"
              " <NewProtocol>UDP</NewProtocol>\n"
              " <NewInternalPort>" + to_string(internal_port) + "</NewInternalPort>\n"
              " <NewInternalClient>" + "192.168.11.1" + "</NewInternalClient>\n"
              " <NewPortMappingDescription>Port mapping for " + to_string(internal_port) + "</NewPortMappingDescription>\n"
              " <NewLeaseDuration>0</NewLeaseDuration>\n";
}
