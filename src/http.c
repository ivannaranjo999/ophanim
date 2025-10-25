#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include "http.h"

#define METRIX_SIZE 1024
#define UPDATE_RATE 5

extern volatile sig_atomic_t stop;
extern char metrics_text[METRIX_SIZE];
extern pthread_mutex_t metrics_lock;
int server_fd_global;

int start_http_server(int port) {
  int new_socket;
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);
  char buffer[1024] = {0};
  char response[2048] = {0};
  char *body = NULL;

  /* IPv4 TCP */
  if ((server_fd_global = socket(AF_INET, SOCK_STREAM,0)) == 0){
    perror("socket failed");
    return -1;
  }

  if (setsockopt(server_fd_global, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
    &opt, sizeof(opt))) {
    perror("setsockopt");
    close(server_fd_global);
    return -1;
  }

  /* IPv4, all interfaces, given port */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  /* Bind created socket */
  if(bind(server_fd_global, (struct sockaddr *)&address, sizeof(address)) < 0){
    perror("bind failed");
    close (server_fd_global);
    return -1;
  }
  
  /* Passive socket with up to MAX_PENDING_QUEUE pending connections */
  if (listen(server_fd_global, MAX_PENDING_QUEUE) < 0){
    perror("listen");
    close(server_fd_global);
    return -1;
  }

  printf("HTTP server listening on port %d...\n", port);

  while (!stop) {
    /* Accept a connection, blocking call */
    new_socket = accept(server_fd_global, (struct sockaddr *)&address, &addrlen);
    if(new_socket < 0) {
      if(stop){
        /* Socket closed due to shutdown, exit gracefully */
        break;
      } else {
        perror("accept");
        continue;
      }
    }

    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));

    /* Read request from client, blocking call */
    read(new_socket, buffer, sizeof(buffer) - 1);
    /* printf("Received request:\n%s\n",buffer); */

    /* Read from shared variable */
    pthread_mutex_lock(&metrics_lock);
    body = metrics_text;
    pthread_mutex_unlock(&metrics_lock);

    snprintf(response, sizeof(response),
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Connection: close\r\n"
      "Refresh: %d\r\n"
      "\r\n"
      "Current metrics:\n"
      "%s", UPDATE_RATE, body);

    send(new_socket, response, strlen(response),0);
    close(new_socket);
  }

  printf("HTTP closed gracefully\n");
  close(server_fd_global);
  return 0;
}
