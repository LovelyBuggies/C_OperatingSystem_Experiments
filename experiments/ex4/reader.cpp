#include <sys/types.h>  
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h> 
#include <cstring>
#include <cstdlib> 
#include <iostream>
using namespace std;

int data = 0;
int read_count = 0;
sem_t writer, nmutex;


struct command
{
    int pid;
    char type;
    int startTime;
    int lastTime;
};


void write() {
    int rd = rand() % RAND_MAX;
    cout << "Write data " << rd << endl;
    data = rd;
}

void read() {
    cout << "Read data " << data << endl;
}


void *Writer(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Writer(the " << c->pid << " pthread) is applying to write." << endl;
        sem_wait(&writer);

        cout << "Writer(the " << c->pid << " pthread) begins to write." << endl;
        write();

        sleep(c->lastTime);
        cout << "Writer(the " << c->pid << " pthread) stops writing." << endl;
        sem_post(&writer);
        sleep(5);
        pthread_exit(0);
    }
}


void *reader(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Reader(the " << c->pid << " pthread) is applying to read." << endl;
        sem_wait(&nmutex);

        read_count++;
        if (read_count == 1) 
            sem_wait(&writer);
        sem_post(&nmutex);

        cout << "Reader(the " << c->pid << " pthread) begins to read." << endl;
        read();

        sleep(c->lastTime);
        cout << "Reader(the " << c->pid << " pthread) stops reading." << endl;
        sem_wait(&nmutex);

        read_count--;
        if (read_count == 0) 
            sem_post(&writer);
        sem_post(&nmutex);
        sleep(5);
        pthread_exit(0);
    }
}


int main(int argc, char const *argv[])
{
  int number_person = atoi(argv[1]);
  sem_init(&writer, 0, 1);
  sem_init(&nmutex, 0, 1);
  struct command information[number_person];
  pthread_t pid[number_person];
  for (int i = 0; i < number_person; i++) {
    cin >> information[i].pid >> information[i].type 
            >> information[i].startTime >> information[i].lastTime;
  }
  for (int i = 0; i < number_person; i++) {
    if (information[i].type == 'R') {
        cout << "Create a reader pthread, it's the " << information[i].pid << " pthread." << endl;
        pthread_create(&pid[i], NULL, reader, &information[i]);
    }

    if (information[i].type == 'W') {
        pthread_create(&pid[i], NULL, Writer, &information[i]);
        cout << "Create a writer pthread, it's the " << information[i].pid << " pthread." << endl;
    }
  }

  for (int i = 0; i < number_person; i++) {
    pthread_join(pid[i], NULL);
  }
  sem_destroy(&writer);
  sem_destroy(&nmutex);
  return 0;
}