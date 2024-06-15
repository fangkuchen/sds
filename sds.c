#include <stdio.h>
#include <sys/socket.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>


#define BUFFER_SIZE 1024
#define OK_HEADER "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n"
#define NOTFOUND_HEADER "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define NOTALLOWED_HEADER "HTTP/1.0 405 Method Not Allowed\r\nContent-Type:text/html\r\n\r\n"
#define INTERNALERR_HEADER "HTTP/1.0 500 Internal Server Error\r\nContent-Type:text/html\r\n\r\n"

typedef struct {
  char *name;
  char *addr;
  char *unit;
} Sensor;

typedef struct {
  char *memory;
  size_t size;
} MemoryStruct;

void die(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

#include "sensors.h"

size_t writemem(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    fprintf(stderr, "not enough memory (realloc returned NULL)\n");
    return 0;
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void buildsite(char response[], char *name, char *val, char *unit) {
  strcat(response, "<li>");
  strcat(response, name);
  strcat(response, ": ");
  strcat(response, val);
  strcat(response, unit);
  strcat(response, "</li>");
}

int main(int argc, char *argv[]) {
  if (argc < 3 || strcmp(argv[1], "-p") != 0)
      die("usage: mts [-p PORT]");
  char response[BUFFER_SIZE];
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int port = atoi(argv[2]);
  char buffer[BUFFER_SIZE];

  struct sockaddr_in host_addr;
  int host_addrlen = sizeof(host_addr);
  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(port);
  host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  struct sockaddr_in client_addr;
  int client_addrlen = sizeof(client_addr);

  if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) 
    die("could not bind socket");
  if (listen(sockfd, 10) != 0)
    die("could not listen");

  while(1) {
    int clientfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
    if (clientfd < 0) {
      fprintf(stderr, "could not accept connection\n");
      continue;
    }
    getpeername(clientfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);

    memset(buffer, 0, BUFFER_SIZE);
    read(clientfd, buffer, BUFFER_SIZE);
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    memset(response, 0, BUFFER_SIZE);

    if (strcmp("GET", method) != 0) {
      strncat(response, NOTALLOWED_HEADER, BUFFER_SIZE - strlen(NOTALLOWED_HEADER) - 1);
      strncat(response, "<html>method not allowed</html>", BUFFER_SIZE - strlen(response) - 1);
    }
    else if (strcmp("/", uri)) {
      printf("%s\n", uri);
      strncat(response, NOTFOUND_HEADER, BUFFER_SIZE - strlen(NOTFOUND_HEADER) - 1);
      strncat(response, "<html>page not found</html>", BUFFER_SIZE - strlen(response) - 1);
    }
    else {
      strncat(response, OK_HEADER, BUFFER_SIZE - strlen(OK_HEADER) - 1);
      strcat(response, "<html><h1>Sensor data</h1><ul>");
      int sensorsc = sizeof(sensors)/sizeof(sensors[0]);
      int i;
      for (i = 0; i < sensorsc; i++) {
        char *name = sensors[i].name;
        char *unit = sensors[i].unit;
        CURL *curl;
        CURLcode curlresponse;

        MemoryStruct chunk;
        chunk.memory = malloc(1);
        chunk.size = 0;


        curl = curl_easy_init();
        if (curl == NULL) {
          fprintf(stderr, "request failed\n");
          continue;
        }

        curl_easy_setopt(curl, CURLOPT_URL, sensors[i].addr);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writemem);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        curlresponse = curl_easy_perform(curl);
        if (curlresponse != CURLE_OK) {
          fprintf(stderr, "request failed\n");
          continue;
        }

        char *val = chunk.memory;
        buildsite(response, name, val, unit);

        free(chunk.memory);
        curl_easy_cleanup(curl);

      }
      strcat(response, "</ul></html>");
    }
    write(clientfd, response, strlen(response));
    close(clientfd);
  }
  return 0;
}