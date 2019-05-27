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
#define EXAMPLE_WIFI_SSID "MartinRouterKing"
#define EXAMPLE_WIFI_PASS "VgEp#x+c2d7MnVzK,;_="

static const char *TAG="APP";

/* An HTTP GET handler */
esp_err_t moisture_up_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t moisture_down_get_handler(httpd_req_t *req)
{
	char* buf;
	size_t buf_len;

	/*
	 buf_len = httpd_resp_send(req, "<!DOCTYPE html><html><body>") + 1;
	 if (buf_len > 1) {
	 buf = malloc(buf_len);
	 if (httpd_resp_send(req, "<!DOCTYPE html><html><body>", buf, buf_len) == ESP_OK) {
	 ESP_LOGI(TAG, "Found header => Host: %s", buf);
	 }
	 free(buf);
	 }
	 */
	char* resp_str = "<!DOCTYPE html> <html><body> <p>Hello World</p> </body></html>";
	httpd_resp_send(req, resp_str, strlen(resp_str));

	resp_str ="<p>Hello World</p>";
	httpd_resp_send(req, resp_str, strlen(resp_str));

	httpd_resp_send(req, resp_str, strlen(resp_str));

	resp_str = "</body></html>";
	httpd_resp_send(req, resp_str, strlen(resp_str));

	httpd_resp_send(req, NULL, 0);

	return ESP_OK;
}

httpd_uri_t moistureup = {
    .uri       = "/moistureup",
    .method    = HTTP_GET,
    .handler   = moisture_up_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Moisture was set up."
};

httpd_uri_t moisturedown = {
    .uri       = "/moisturedown",
    .method    = HTTP_GET,
    .handler   = moisture_down_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Moisture was set down."
};




/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* Handler can be unregistered using the uri string */
        ESP_LOGI(TAG, "Unregistering /moistureup and /echo URIs");
        httpd_unregister_uri(req->handle, "/moistureup");
        httpd_unregister_uri(req->handle, "/mousturedown");
    }
    else {
        ESP_LOGI(TAG, "Registering /moistureup and /echo URIs");
        httpd_register_uri_handler(req->handle, &moistureup);
        httpd_register_uri_handler(req->handle, &moisturedown);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &moistureup);
        httpd_register_uri_handler(server, &moisturedown);
        httpd_register_uri_handler(server, &ctrl);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: '%s'",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));


        /* Start the web server */
        if (*server == NULL) {
            //*server = start_webserver();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the web server */
        if (*server) {
           // stop_webserver(*server);
            *server = NULL;
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void *arg)
{
    tcpip_adapter_init();
    //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

}


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
    ESP_LOGI(TAG, "Netconn bound");

    netconn_listen(nc);

    char buf[512];
    ESP_LOGI(TAG, "Buffer allocated.");

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
    ESP_LOGI(TAG, "Webpage loaded");

    /* disable LED */
    //gpio_enable(2, GPIO_OUTPUT);
    //gpio_write(2, true);
    ESP_LOGI(TAG, "Webpage loaded");

    while (1) {
        ESP_LOGI(TAG, "Webpage loaded");

        err_t err = netconn_accept(nc, &client);
        if (err == ERR_OK) {
            struct netbuf *nb;
            ESP_LOGI(TAG, "Webpage loaded");

            if ((err = netconn_recv(client, &nb)) == ERR_OK) {
                void *data;
                u16_t len;
                netbuf_data(nb, &data, &len);
                /* check for a GET request */
                ESP_LOGI(TAG, "Webpage loaded");

                if (!strncmp(data, "GET ", 4)) {
                    ESP_LOGI(TAG, "Webpage loaded");

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

                    if (!strncmp(uri, "/higher", max_uri_len))
                        //gpio_write(2, false);
                    	ESP_LOGI(TAG, "Webpage loaded");
                    else if (!strncmp(uri, "/lower", max_uri_len))
                        ESP_LOGI(TAG, "Webpage loaded");
                        //gpio_write(2, true);
                    ESP_LOGI(TAG, "Webpage loaded");

                    snprintf(buf, sizeof(buf), webpage,
                            uri,
                            xTaskGetTickCount() * portTICK_PERIOD_MS / 1000,
                            (int) heap_caps_get_free_size(MALLOC_CAP_8BIT));
                    netconn_write(client, buf, strlen(buf), NETCONN_COPY);
                }
                ESP_LOGI(TAG, "Webpage loaded");

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
    ESP_LOGI(TAG, "start_http called.");

    //static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    init_server();
    ESP_LOGI(TAG, "nvs_flash_init called.");
}


