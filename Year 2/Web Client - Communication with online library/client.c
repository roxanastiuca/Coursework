/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Asks the user to complete a prompt, such as: prompt=user_given_value.
 */
void read_prompt(char *prompt, char str[MAX_PROMPT]) {
	printf("%s", prompt);
	fgets(str, MAX_PROMPT, stdin);
	remove_whitespace(str);
}


/**
 * Add a new entry in body_data array:
 - for a value of type String, the new entry has the form: "key":"value";
 - for a value of type Number, the new entry has the form: "key":value.
 */
void add_body_data(
	char **body_data, int *body_data_count, char *key, char *value, int isInt
	) {

	body_data[*body_data_count] = 
			(char *) calloc(MAX_COMMAND + MAX_PROMPT, sizeof(char));
	prog_error(body_data[*body_data_count] == NULL, "Memory error.");

	if(isInt) {
		sprintf(body_data[*body_data_count], "\"%s\":%s", key, value);
	} else {
		sprintf(body_data[*body_data_count], "\"%s\":\"%s\"", key, value);
	}

	(*body_data_count)++;
}

/**
 * Shows all books information after extracting them from the server
 response.
 */
void show_books_info(char *books) {
	if(books == NULL || strlen(books) == 0) {
		printf("You currently don't have any books in your library.\n");
		return;
	}

	printf("You currently have the following books in your library:\n");

	/* Split the string into each book's information, knowing that
	books string has the form: {book1_info},{book2_info},... */
	char *book = strtok(books, "}");
	int cnt = 0;

	while(book) {
		char buf[BUFLEN];
		strcpy(buf, book);
		strcat(buf, "}");

		/* Extract id and title fields from the book's informations. */
		int id = extract_payload_int_field(buf, "id");
		char *title = extract_payload_field(buf, "title");

		printf("- Book %d: id: %d, title: %s\n", ++cnt, id, title);

		free(title);
		book = strtok(NULL, "}");
	}

	free(books);
}

/**
 * Shows a book's information in a user-friendly format after
 extracting it from the server response.
 */
void show_book_info(char *book) {
	char *title = extract_payload_field(book, "title");
	char *author = extract_payload_field(book, "author");
	char *genre = extract_payload_field(book, "genre");
	char *publisher = extract_payload_field(book, "publisher");

	printf("You received the following information about the book:\n");
	printf("- Title: %s\n", title);
	printf("- Author: %s\n", author);
	printf("- Genre: %s\n", genre);
	printf("- Page count: %d\n", extract_payload_int_field(book, "page_count"));
	printf("- Publisher: %s\n", publisher);

	free(title);
	free(author);
	free(genre);
	free(publisher);
}

/**
 * Sends a message to server and waits for a response. In case the server
 closed the connection, reopens it and tries again.
 */
char *send_and_receive(int *sockfd, char *message) {
	send_to_server(*sockfd, message);
	char *response = receive_from_server(*sockfd);

	if(response == NULL || strlen(response) == 0) {
		/* Server closed connection. Try to reopen it and send the message
		again. */
		if(response)
			free(response);
		
		*sockfd = open_connection(HOST_IP, HOST_PORT, AF_INET, SOCK_STREAM, 0);
		send_to_server(*sockfd, message);
		response = receive_from_server(*sockfd);
	}

	return response;
}

/**
 * Function corresponds to the "register" command.
 */
int register_command(int *sockfd) {
	char username[MAX_PROMPT];
	char password[MAX_PROMPT];

	/* Ask user for username and password in order to add those to the
	message's body. */
	read_prompt("username=", username);
	read_prompt("password=", password);

	char **body_data = (char **) calloc(2, sizeof(char *));
	int body_data_count = 0;

	add_body_data(body_data, &body_data_count, "username", username, 0);
	add_body_data(body_data, &body_data_count, "password", password, 0);

    char *message = compute_post_request(
    	HOST_NAME, "/api/v1/tema/auth/register", NULL, "application/json",
        body_data, 2, NULL, 0);

    char *response = send_and_receive(sockfd, message);

    if(response == NULL || strlen(response) == 0) {
    	return CONNECTION_LOST;
    }

    /* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
    	printf("Successfully registered new account for %s.\n", username);
    } else {
    	/* Extract error message from the response. */
    	char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
    }

    free(body_data[0]);
    free(body_data[1]);
    free(body_data);
    free(message);
    free(response);

    return 0;
}

/**
 * Function corresponds to the "login" command.
 */
int login_command(int *sockfd, char **cookies, int *cookies_count) {
	char username[MAX_PROMPT];
	char password[MAX_PROMPT];

	/* Ask user for username and password in order to add those to the
	message's body. */
	read_prompt("username=", username);
	read_prompt("password=", password);

	char **body_data = (char **) calloc(2, sizeof(char *));
	int body_data_count = 0;

	add_body_data(body_data, &body_data_count, "username", username, 0);
	add_body_data(body_data, &body_data_count, "password", password, 0);

	char *message = compute_post_request(
		HOST_NAME, "/api/v1/tema/auth/login", NULL, "application/json",
		body_data, 2, NULL, 0);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
    	return CONNECTION_LOST;
    }

    // printf("///////////////");
    // printf("%s", response);
    // printf("///////////////");

    /* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
    	printf("Successfully logged-in. Welcome back, %s!\n", username);
		cookies[*cookies_count] = extract_cookie(response);
		(*cookies_count)++;  
    } else {
    	/* Extract error message from the response. */
    	char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
    }

    free(body_data[0]);
    free(body_data[1]);
    free(body_data);
    free(message);
    free(response);

	return 0;
}

/**
 * Function corresponds to the "enter_library" command.
 */
int enter_library_command(
	int *sockfd, char **cookies, int *cookies_count, char **token
	) {

	char *message = compute_get_request(
		HOST_NAME, "/api/v1/tema/library/access", NULL, NULL,
		cookies, *cookies_count);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
		return 1;
	}

	/* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
		printf("Successfully entered library.\n");
		*token = extract_payload_field(response, "token");
	} else {
		/* Extract error message from the response. */
		char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
	}

	free(message);
    free(response);

	return 0;
}

/**
 * Function corresponds to the "logout" command.
 */
int logout_command(int *sockfd, char **cookies, int *cookies_count) {
	char *message = compute_get_request(
		HOST_NAME, "/api/v1/tema/auth/logout", NULL, NULL,
		cookies, *cookies_count);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
		return 1;
	}

	/* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
		printf("Successfully logged-out.\n");
		/* Delete old cookie. */
		free(cookies[*cookies_count - 1]);
		(*cookies_count)--;
	} else {
		/* Extract error message from the response. */
		char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
	}

	free(message);
    free(response);

	return 0;	
}

/**
 * Function corresponds to the "add_book" command.
 */
int add_book_command(int *sockfd, char *token) {
	char title[MAX_PROMPT], author[MAX_PROMPT], genre[MAX_PROMPT];
	char publisher[MAX_PROMPT], page_count[MAX_PROMPT];

	/* Ask user for title, author, genre, publisher and page count in
	order to add those to the message's body. */
	read_prompt("title=", title);
	read_prompt("author=", author);
	read_prompt("genre=", genre);
	read_prompt("publisher=", publisher);
	read_prompt("page_count=", page_count);

	char **body_data = (char **) calloc(5, sizeof(char *));
	int body_data_count = 0;

	add_body_data(body_data, &body_data_count, "title", title, 0);
	add_body_data(body_data, &body_data_count, "author", author, 0);
	add_body_data(body_data, &body_data_count, "genre", genre, 0);
	add_body_data(body_data, &body_data_count, "page_count", page_count, 1);
	add_body_data(body_data, &body_data_count, "publisher", publisher, 0);

	char *message = compute_post_request(
		HOST_NAME, "/api/v1/tema/library/books", token, "application/json",
		body_data, body_data_count, NULL, 0);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
    	return 1;
    }

    /* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
    	printf("Successfully added book %s.\n", title);
    } else {
    	/* Extract error message from the response. */
    	char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
    }

    for(int i = 0; i < body_data_count; i++)
    	free(body_data[i]);
    free(body_data);
    free(message);
    free(response);

	return 0;
}

/**
 * Function corresponds to the "get_books" command.
 */
int get_books_command(int *sockfd, char *token) {
	char *message = compute_get_request(
		HOST_NAME, "/api/v1/tema/library/books", token, NULL,
		NULL, 0);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
		return 1;
	}

	/* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
		show_books_info(extract_multi_payload(response));
	} else {
		/* Extract error message from the response. */
		char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
	}

	free(message);
    free(response);

	return 0;
}

/**
 * Function corresponds to the "get_book" command.
 */
int get_book_command(int *sockfd, char *token) {
	char id[MAX_PROMPT];

	/* Ask user for book id in order to add that to the message's
	url. */
	read_prompt("id=", id);

	char url[MAX_PROMPT];
	sprintf(url, "/api/v1/tema/library/books/%s", id);

	char *message = compute_get_request(
		HOST_NAME, url, token, NULL, NULL, 0);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
		return 1;
	}

	/* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
		show_book_info(response);
	} else {
		/* Extract error message from the response. */
		char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
	}

	free(message);
    free(response);

	return 0;
}

/**
 * Function corresponds to the "delete_book" command.
 */
int delete_book_command(int *sockfd, char *token) {
	char id[MAX_PROMPT];

	/* Ask user for book id in order to add that to the message's
	url. */
	read_prompt("id=", id);

	char url[MAX_PROMPT];
	sprintf(url, "/api/v1/tema/library/books/%s", id);

	char *message = compute_delete_request(
		HOST_NAME, url, token, NULL, NULL, 0);

	char *response = send_and_receive(sockfd, message);

	if(response == NULL || strlen(response) == 0) {
		return 1;
	}

	/* Check message status code for success or errors. */
    int status_code = extract_status_code(response);

    if(status_code / 100 == 2) {
    	/* Status code 2XX stands for success. */
		printf("Successfully deleted book with id %s.\n", id);
	} else {
		/* Extract error message from the response. */
		char *error = extract_payload_field(response, "error");
    	printf("%s\n", error);
    	free(error);
	}

	free(message);
	free(response);

	return 0;
}

/**
 * Main function of the program which reads commands and calls the respective
 functions for those commands.
 */
int main() {
	int sockfd = open_connection(HOST_IP, HOST_PORT, AF_INET, SOCK_STREAM, 0);

	int continue_while = 1;

	char **cookies = (char **) calloc(MAX_COOKIES, sizeof(char *));
	int cookies_count = 0;

	char *token = NULL;

	while(continue_while) {
		char command[MAX_COMMAND];
		fgets(command, MAX_COMMAND, stdin);
		remove_whitespace(command);

		int ret = 0;

		if(strcmp(command, "register") == 0) {
			ret = register_command(&sockfd);
		} else if(strcmp(command, "login") == 0) {
			if(cookies_count > 0) {
				printf("Someone else is already logged-in!\n");
				continue;
			}
			ret = login_command(&sockfd, cookies, &cookies_count);
		} else if(strcmp(command, "enter_library") == 0) {
			ret = enter_library_command(&sockfd, cookies, &cookies_count, &token);
		} else if(strcmp(command, "add_book") == 0) {
			ret = add_book_command(&sockfd, token);
		} else if(strcmp(command, "get_books") == 0) {
			ret = get_books_command(&sockfd, token);
		} else if(strcmp(command, "get_book") == 0) {
			ret = get_book_command(&sockfd, token);
		} else if(strcmp(command, "delete_book") == 0) {
			ret = delete_book_command(&sockfd, token);
		} else if(strcmp(command, "logout") == 0) {
			ret = logout_command(&sockfd, cookies, &cookies_count);
			free(token);
			token = NULL; /* Forget about old authorization token. */
		} else if(strncmp(command, "exit", 4) == 0) {
			/* Stop program execution. */
			continue_while = 0;
		} else {
			printf("Unknown command.\n");
		}

		if(ret == CONNECTION_LOST) {
			printf("Server closed connection.\n");
			continue_while = 0;
		}
	}

	/* Free all memory used. */
	while(cookies_count != 0) {
		cookies_count--;
		free(cookies[cookies_count]);
	}
	free(cookies);

	if(token)
		free(token);

	/* Close socket connection. */
	close_connection(sockfd);

	return 0;
}