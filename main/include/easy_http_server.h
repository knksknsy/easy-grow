/*
 * easy_http_server.h
 *
 *  Created on: 22 Apr 2019
 *      Author: m.bilge
 */

#ifndef MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

#define MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

typedef enum {
    EASY_AP_MODE,     /**< AccessPoint Website */
    EASY_GROW_MODE          /**< Easy Grow Webseite */
} website_interface;

void start_http(const website_interface *website);
void init_server();


#endif /* MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_ */

