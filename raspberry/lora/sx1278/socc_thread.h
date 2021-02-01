#ifndef __SOCC_THREAD_H__
#define _SOCC_THREAD_H__

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include "log.h"

#include <base64.h>
#include <arc4.h>

char client_message[2000];
char buffer[1024];

char pass[] = "P@ss6wd#1";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
enum STATE {
    STATE_NONE,
    STATE_START,
    STATE_END
};

void decode_msg(char* in_msg, int in_msg_len, char* out_msg) {
    char decrypt_msg[1024];
    int base64_decoded_msg_len;
    char *base64_decoded_msg = base64_decode(in_msg, in_msg_len, &base64_decoded_msg_len);
    if (base64_decoded_msg) {
        ARC4(base64_decoded_msg, base64_decoded_msg_len, pass, strlen(pass), decrypt_msg);
        printf("\ndecrypt_msg: %s \n", decrypt_msg);
        free(base64_decoded_msg);
    }
}

void * socketThread(void *arg) {
    int msg_len;
    int newSocket = *((int *)arg);
    char out_msg[1024];

    memset(client_message, 0, sizeof(client_message));
    msg_len = recv(newSocket , client_message , 2000 , 0);
    
    if (msg_len > 0) {
        decode_msg(client_message, msg_len, out_msg);
    }
    // printf("client_message: %s \n", client_message);

    // // Send message to the client socket 
    // pthread_mutex_lock(&lock);
    // char *message = malloc(sizeof(client_message) + 20);
    // strcpy(message,"Hello Client : ");
    // strcat(message, client_message);
    // strcat(message,"\n");
    // strcpy(buffer,message);
    // free(message);
    // pthread_mutex_unlock(&lock);
    // sleep(1);
    // send(newSocket,buffer,13,0);
    // printf("Exit socketThread \n");
    close(newSocket);
    pthread_exit(NULL);
}

void *socc_main_thread() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket. 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address struct
    // Address family = Internet 
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order 
    serverAddr.sin_port = htons(7799);
    //Set IP address to localhost 
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //Set all bits of the padding field to 0 
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind the address struct to the socket 
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Listen on the socket, with 40 max connection requests queued 
    if(listen(serverSocket,50) == 0) {
        printf("Listening\n");
    } else {
        printf("Error\n");
    }
    pthread_t tid[60];
    int i = 0;
    while(1) {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
       
        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
        if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 ) {
            printf("Failed to create thread\n");
        }
        if( i >= 50 ) {
            i = 0;
            while( i < 50 ) {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }
}

int test_socc_thread() {
    pthread_t thread;
    pthread_create(&thread, NULL, socc_main_thread, NULL);
    return 0;
}

#endif

