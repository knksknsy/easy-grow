/*
 * easy_http_server.c
 *
 *  Created on: 22 Apr 2019
 *      Author: m.bilge
 */

#include <FreeRTOS.h>

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <lwip/api.h>

#include <easy_wifi_manager.h>

#include <easy_http_server.h>
#include <esp_http_server.h>




/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
//#define EXAMPLE_WIFI_SSID "MartinRouterKing"
//#define EXAMPLE_WIFI_PASS "VgEp#x+c2d7MnVzK,;_="

static const char *TAG="HTTP_SERVER";


void httpd_task(void *pvParameters)
{
    ESP_LOGI(TAG, "HTTPD Task Started");

    struct netconn *client = NULL;
    struct netconn *nc = netconn_new(NETCONN_TCP);
    if (nc == NULL) {
        printf("Failed to allocate socket.\n");
        vTaskDelete(NULL);
    }
    netconn_bind(nc, IP_ADDR_ANY, 80);
    netconn_listen(nc);

    char buf[512];
    const char *webpage = {
        "HTTP/1.1 200 OK\r\n"
        "Content-type: text/html\r\n\r\n"
        "<html><head><title>Easy Grow Server</title>"
        "<style> div.main {"
        "font-family: Arial;"
        "padding: 0.01em 16px;"
        "box-shadow: 2px 2px 1px 1px #d2d2d2;"
        "background-color: #f1f1f1;}"
        "</style></head>"
        "<body><div class='main'>"
        "<h3>HTTP Server</h3>"
    	"<meter max= 1.0 min= 0.0 value= 0.5 high= .75 low= .25 optimum= 0.5 ></meter>"
        "<p>URL: %s</p>"
    	"<p>Current Moisture: feucht</p>"
    	"<p>Watertanklevel: voll</p>"
        "<p>Sun hours: viele</p>"
        "<p>Uptime: %d seconds</p>"
        "<p>Free heap: %d bytes</p>"
        "<button onclick=\"location.href='/higher'\" type='button'>"
        "Moisture Higher</button></p>"
        "<button onclick=\"location.href='/lower'\" type='button'>"
        "Moisture Lower</button></p>"
        "</div></body></html>"
    };

    while (1) {
        err_t err = netconn_accept(nc, &client);
        if (err == ERR_OK) {
            struct netbuf *nb;
            if ((err = netconn_recv(client, &nb)) == ERR_OK) {
                void *data;
                u16_t len;
                netbuf_data(nb, &data, &len);
                /* check for a GET request */
                if (!strncmp(data, "GET ", 4)) {
                    char uri[16];
                    const int max_uri_len = 16;
                    char *sp1, *sp2;
                    /* extract URI */
                    sp1 = data + 4;
                    sp2 = memchr(sp1, ' ', max_uri_len);
                    int len = sp2 - sp1;
                    memcpy(uri, sp1, len);
                    uri[len] = '\0';
                    printf("uri: %s\n", uri);

                    if (!strncmp(uri, "/higher", max_uri_len)){
                        //TODO
                    	//setMoistureHigher(true);
                    }
                    else if (!strncmp(uri, "/lower", max_uri_len)){
                    	//TODO
                    	//setMoistureHigher(false);
                    }
                    snprintf(buf, sizeof(buf), webpage,
                            uri,
                            xTaskGetTickCount() * portTICK_PERIOD_MS / 1000,
                            (int) heap_caps_get_free_size(MALLOC_CAP_8BIT));
                    netconn_write(client, buf, strlen(buf), NETCONN_COPY);
                }
            }
            netbuf_delete(nb);
        }
        printf("Closing connection\n");
        netconn_close(client);
        netconn_delete(client);
    }
}

void init_server(){
	xTaskCreate(&httpd_task, "http_server", 8000, NULL, 2, NULL);
}



void start_http(const website_interface *website)
{
    //static httpd_handle_t server = NULL;
    nvs_flash_init();
    init_server();
}


