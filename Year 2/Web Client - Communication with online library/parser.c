/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Remove trailing whitespace from a string.
 */
void remove_whitespace(char *str) {
	int len = strlen(str);
	while(isspace(str[len - 1]))
		len--;

	str[len] = '\0';
}

/**
 * Extract cookie from a HTTP message.
 */
char *extract_cookie(char *message) {
	char *aux = strstr(message, "Set-Cookie");
	if(aux == NULL)		return NULL;
	aux = aux + COOKIE_OFFSET;
	aux = strtok(aux, ";");
	if(aux == NULL)		return NULL;

	/* Copy it into a different string. */
	char *cookie = (char *) calloc(strlen(aux) + 1, sizeof(char));
	prog_error(cookie == NULL, "Memory error - cookie allocation.");
	memcpy(cookie, aux, strlen(aux));

	return cookie;
}

/**
 * Extract payload from a HTTP message.
 */
char *extract_payload(char *message) {
	/* Search for opening bracket. */
	char *aux = strchr(message, '{');
	if(aux == NULL)		return NULL;
	aux = aux + 1;

	/* Search for closing bracket. */
	char *aux2 = strchr(aux, '}');
	if(aux2 == NULL)	return NULL;

	/* Copy it into a different string. */
	char *payload = (char *) calloc(aux2 - aux + 1, sizeof(char));
	prog_error(payload == NULL, "Memory error - payload allocation.");
	memcpy(payload, aux, aux2 - aux);
	payload[aux2 - aux] = '\0';

	return payload;
}

/**
 * Extract array payload from a HTTP message.
 */
char *extract_multi_payload(char *message) {
	/* Search for opening bracket. */
	char *aux = strchr(message, '[');
	if(aux == NULL)		return NULL;
	aux = aux + 1;

	/* Search for closing bracket. */
	char *aux2 = strchr(aux, ']');
	if(aux2 == NULL)	return NULL;

	/* Copy it into a different string. */
	char *payload = (char *) calloc(aux2 - aux + 1, sizeof(char));
	prog_error(payload == NULL, "Memory error - payload allocation.");
	memcpy(payload, aux, aux2 - aux);
	payload[aux2 - aux] = '\0';

	return payload;
}

/**
 * Extract status code from a HTTP message.
 */
int extract_status_code(char *message) {
	char *aux = strchr(message, ' ');
	if(aux == NULL)		return 0;
	aux++;

	int status_code = atoi(aux);
	prog_error(status_code == 0, "Atoi conversion unsuccessfull.");

	return status_code;
}

/**
 * Extract a certain field's value from a HTTP message, where
 value must be of type Number.
 */
int extract_payload_int_field(char *message, char *field) {
	char *payload = extract_payload(message);

	/* Search for the word field and then skip to its value. */
	char *aux = strstr(payload, field);
	aux = aux + 2 + strlen(field);

	int numb = atoi(aux);
	prog_error(numb == 0, "Atoi conversion failed.\n");

	free(payload);
	return numb;
}

/**
 * Extract a certain field's value from a HTTP message, where value
 must be of type String.
 */
char *extract_payload_field(char *message, char *field) {
	char *payload = extract_payload(message);

	/* Search for the word field and then skip to its value. */
	char *aux = strstr(payload, field);
	aux = aux + 3 + strlen(field);

	char *aux2 = strchr(aux, '\"');
	if(aux2 == NULL)	return NULL;

	/* Copy value into a different string and return it. */
	char *value = (char *) calloc(aux2 - aux + 1, sizeof(char));
	prog_error(value == NULL, "Memory error - payload field allocation.");
	memcpy(value, aux, aux2 - aux);
	value[aux2 - aux] = '\0';

	free(payload);
	return value;
}