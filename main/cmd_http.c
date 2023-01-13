#include <stdio.h>
#include <string.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "cmd_http.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void rest_get(const char *url)
{
    esp_http_client_config_t config_get = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void post_rest_function(const char *url, const char *data)
{
    esp_http_client_config_t config_post = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char  *post_data = data;
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}


static struct {
    struct arg_str *http_get;
    struct arg_str *http_adress;
    struct arg_str *http_data;
    struct arg_end *end;
} http_args;

static int http_connect(int argc, char **argv)
{
    char get_request[]="GET";
    char post_request[]="POST";
    int nerrors = arg_parse(argc, argv, (void **) &http_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, http_args.end, argv[0]);
        return 1;
    }
    if(strcmp(http_args.http_get->sval[0], get_request) == 0)
    {
        rest_get(http_args.http_adress->sval[0]);
        //printf("\nRequest is GET\n");
    }
    if(strcmp(http_args.http_get->sval[0], post_request) == 0)
    {
        post_rest_function(http_args.http_adress->sval[0], http_args.http_data->sval[0]);
        //printf("\nRequest is POST\n");
    }
    //printf("\nRequest : %s\n",  http_args.http_get->sval[0]);
    //printf("\nURL : %s\n",  http_args.http_adress->sval[0]);
    //printf("\ndata : %s\n",   http_args.http_data->sval[0]);
    //ESP_LOGI(__func__, "Connected");
    return 0;
}

void http_request(void)
{
    http_args.http_get = arg_str1(NULL, NULL, "request", "GET or POST");
    http_args.http_adress = arg_str1(NULL, NULL, "URL", "http adress");
    http_args.http_data = arg_str0(NULL, NULL, "http data", "request BODY");
    http_args.end = arg_end(2);

    const esp_console_cmd_t http_cmd = {
        .command = "http",
        .help = "http request",
        .hint = NULL,
        .func = &http_connect,
        .argtable = &http_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&http_cmd) );
}