#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "cpu.h"
#include "mem.h"
#include "net.h"
#include "http.h"

#define REFRESH_PERIOD 5
#define INTERFACE "wlo1"
#define METRIX_SIZE 1024

/* Global variables live in process address, can be accessed by all threads */
volatile sig_atomic_t stop = 0;
char metrics_text[METRIX_SIZE];
pthread_mutex_t metrics_lock = PTHREAD_MUTEX_INITIALIZER;
extern int server_fd_global;

void handle_sigint(int sig) {
  (void) sig;
  stop = 1;
  shutdown(server_fd_global, SHUT_RDWR);
}

void *http_thread_func(void *arg) {
  (void) arg;
  start_http_server(8080);
  return NULL;
}

void print_local_ip() {
  struct ifaddrs *ifaddr, *ifa;
  char ip[INET_ADDRSTRLEN];

  if (getifaddrs(&ifaddr) == -1) {
      perror("getifaddrs");
      return;
  }

  printf("Available network interfaces:\n");
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL) continue;
    if (ifa->ifa_addr->sa_family == AF_INET) { // IPv4 only
      struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
      inet_ntop(AF_INET, &(sa->sin_addr), ip, INET_ADDRSTRLEN);
      // skip loopback
      if (strcmp(ip, "127.0.0.1") != 0) {
          printf(" -> You can access SysProbe at http://%s:8080\n", ip);
      }
    }
  }

  freeifaddrs(ifaddr);
}

int main() {
  double cpu = get_cpu_usage();
  MemInfo mem;
  NetStats net;
  time_t now; 
  char *ts;
  pthread_t http_thread;

  if (signal(SIGINT, handle_sigint) == SIG_ERR) {
    perror("signal");
    return 1;
  }

  pthread_create(&http_thread, NULL, http_thread_func, NULL);

  printf("SysProbe started. Collecting metrics every %d seconds...\n",
    REFRESH_PERIOD);

  print_local_ip();

  while (!stop) {
    get_mem_info(&mem);
    get_net_stats(&net, INTERFACE);

    double mem_used_pct = (double)mem.used_kb / mem.total_kb * 100.0;

    now = time(NULL);
    ts = ctime(&now);
    ts[strcspn(ts, "\n")] = '\0';

    pthread_mutex_lock(&metrics_lock);
    snprintf(metrics_text, sizeof(metrics_text),
     "[%s] CPU: %.2f%% | MEM: %.2f%% (%llu/%llu MB) | NET: rx=%llu KB tx=%llu KB\n",
     ts,
     cpu,
     mem_used_pct,
     mem.used_kb / 1024,
     mem.total_kb / 1024,
     net.rx_bytes / 1024,
     net.tx_bytes / 1024);
    pthread_mutex_unlock(&metrics_lock);

    sleep(REFRESH_PERIOD);
  }

  printf("\nStopping SysProbe...\n");
  pthread_join(http_thread, NULL);
  printf("Exited cleanly.\n");
  return 0;

  return 0;
}
