#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>

void string_shift_right(int arg_index, char* input, const char* original, int cur_socket) {
	size_t input_length = strlen(input);
	char shifted_input[input_length];
	memset(shifted_input, 0, sizeof(shifted_input));

	//for (size_t char_index = 0; char_index < (input_length); ++char_index)
	// do not shift /0 terminator
	for (size_t char_index = 0; char_index < (input_length ); ++char_index) {
		//size_t new_position = (char_index + 2) / input_length;
		// use: % , do not shift /0
		size_t new_position = (char_index + 2) % input_length;
		shifted_input[new_position] = input[char_index];
	}
	
	//Set \0 terminator
	shifted_input[input_length] = '\0';

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		input[char_index] = toupper(input[char_index]);
	}

	if (send(cur_socket, shifted_input, input_length, 0) < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
	printf("original argv[%d]: %s\nuppercased: %s\nshifted: %s\n", arg_index, original, input, shifted_input);
}

int main(int argc, const char** argv) {
	char* shared_strings[argc];
	int parent_sockets[argc];
	//for (int arg_index = 0; arg_index < argc; ++arg_index)
	// ++ on right
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		//size_t arg_length = strlen(argv[arg_index]);
		// +1 for /0
		size_t arg_length = strlen(argv[arg_index]) + 1;
		//shared_strings[arg_index] = malloc(arg_length * sizeof(char));
		// better: sizeof(*shared_strings[arg_index])
		shared_strings[arg_index] = malloc(arg_length * sizeof(*shared_strings[arg_index]));
		if (shared_strings[arg_index] == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		strcpy(shared_strings[arg_index], argv[arg_index]);
	}

	//for (int arg_index = 0; arg_index <= argc; arg_index++)
	// argc - 1 => agrv available
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		int sockets[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
            perror("socketpair");
            exit(EXIT_FAILURE);
        }
		
		pid_t pid = fork();
		//if (pid == -1)
		// better: if (pid < 0) 
		if (pid < 0) {
			perror("Could not create fork");
			exit(EXIT_FAILURE);
		}
		//if ...
		// better: else if
		else if (pid == 0) {
			// Child process: close parent's end and use child's socket
			close(sockets[0]);
            string_shift_right(arg_index, shared_strings[arg_index], argv[arg_index], sockets[1]);
			close(sockets[1]);
            exit(EXIT_SUCCESS);
		}
		else {
			// Parent process: close child's end and store parent's socket
            close(sockets[1]);
            parent_sockets[arg_index] = sockets[0];
		}
	}

	//for (int arg_index = 0; arg_index <= argc; arg_index++)
	// argc - 1 => agrv available
	for (int arg_index = 0; arg_index < argc; arg_index++) {
        char buffer[128];
        ssize_t received = recv(parent_sockets[arg_index], buffer, sizeof(buffer)/sizeof(char), 0);
        if (received < 0) {
            perror("recv");
        } else {
            printf("==> Parent received from argv[%d]: %s\n", arg_index, buffer);
        }
        close(parent_sockets[arg_index]);
		free(shared_strings[arg_index]);
        wait(NULL);
    }

	printf("Done.");
	return EXIT_SUCCESS;
}
