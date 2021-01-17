/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Computes a GET request in the right format.
 * Function completed during Lab 10.
 */
char *compute_get_request(
    char *host, char *url, char *token, char *query_params,
    char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL, request params (if any) and protocol type */
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    /* Add host. */
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Add authorization token. */
    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add headers and/or cookies, according to the protocol format. */
    if (cookies != NULL) {
       memset(line, 0, LINELEN);
        strcpy(line, "Cookie: ");

        for(int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if(i != cookies_count - 1) {
                strcat(line, "; ");
            }
        }

       compute_message(message, line);
    }
    /* Add final new line. */
    compute_message(message, "");

    free(line);
    return message;
}

/**
 * Computes a DELETE request in the right format.
 * Similar to a GET request.
 */
char *compute_delete_request(
    char *host, char *url, char *token, char *query_params,
    char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL, request params (if any) and protocol type */
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    /* Add the host. */
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Add authorization token. */
    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add headers and/or cookies, according to the protocol format. */
    if (cookies != NULL) {
       memset(line, 0, LINELEN);
        strcpy(line, "Cookie: ");

        for(int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if(i != cookies_count - 1) {
                strcat(line, "; ");
            }
        }

       compute_message(message, line);
    }
    /* Add final new line. */
    compute_message(message, "");

    free(line);
    return message;
}

/**
 * Computes a POST request in the right format.
 * Function completed during Lab 10.
 */
char *compute_post_request(
    char *host, char *url, char *token, char* content_type, char **body_data,
    int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL and protocol type. */
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    /* Add the host. */
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Add authorization token. */
    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add necessary headers (Content-Type and Content-Length are mandatory) */
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    strcat(body_data_buffer, "{");

    for(int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);

        if(i != body_data_fields_count - 1) {
            strcat(body_data_buffer, ",");
        }
    }

    strcat(body_data_buffer, "}");

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    /* Add cookies. */
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        strcpy(line, "Cookie: ");

        for(int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if(i != cookies_count - 1) {
                strcat(line, "; ");
            }
        }

       compute_message(message, line);
    }

    /* Add new line at end of header. */
    compute_message(message, "");

    /* Add the actual payload data */
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}

