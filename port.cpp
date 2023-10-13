#include <iostream>
#include "curl/curl.h"

int main() {
    // Initialize libcurl
    CURL *curl;
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        std::cerr << "libcurl initialization failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    // Create a libcurl handle
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "libcurl handle creation failed" << std::endl;
        curl_global_cleanup();
        return 1;
    }

    // Set the URL
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.1:52869/Public UPNP C3");

    // Set the HTTP POST method
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    // Set the request headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
    headers = curl_slist_append(headers, "Connection: close");
    headers = curl_slist_append(headers, "SOAPAction: \"urn:schemas-upnp-org:service:WANIPConnection:1#AddPortMapping\"");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the SOAP XML payload
    const char *soap_xml = "<?xml version=\"1.0\"?>"
                          "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">"
                          "   <s:Body>"
                          "       <!-- Your SOAP XML content here -->"
                          "   </s:Body>"
                          "</s:Envelope>";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soap_xml);

    // Perform the HTTP request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "HTTP request failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "HTTP request succeeded" << std::endl;
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}
