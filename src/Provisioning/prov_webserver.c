// esp_err_t get_handler(httpd_req_t *req)
// {
//     /* Send a simple response */
//     const char[] resp = "URI GET Response";
//     httpd_resp_send(req, resp, strlen(resp));
//     return ESP_OK;
// }


// /* URI handler structure for GET /uri */
// httpd_uri_t uri_get = {
//     .uri      = "/uri",
//     .method   = HTTP_GET,
//     .handler  = get_handler,
//     .user_ctx = NULL
// };

// /* Function for starting the webserver */
// httpd_handle_t start_webserver(void)
// {
//     /* Generate default configuration */
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();

//     /* Empty handle to esp_http_server */
//     httpd_handle_t server = NULL;

//     /* Start the httpd server */
//     if (httpd_start(&server, &config) == ESP_OK) {
//         /* Register URI handlers */
//         httpd_register_uri_handler(server, &uri_get);
//         httpd_register_uri_handler(server, &uri_post);
//     }
//     /* If server failed to start, handle will be NULL */
//     return server;
// }

// /* Function for stopping the webserver */
// void stop_webserver(httpd_handle_t server)
// {
//     if (server) {
//         /* Stop the httpd server */
//         httpd_stop(server);
//     }
// }