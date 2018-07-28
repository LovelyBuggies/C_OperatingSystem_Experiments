#include <sys/types.h>  
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h> 
#include <cstring>
#include <cstdlib> 
#include <iostream>
using namespace std;

int data = 0;
int read_count = 0, write_count = 0;
sem_t writeAccess, readAccess, mutexR, mutexW;


struct command
{
    int pid;
    char type;
    int startTime;
    int lastTime;
};


void write() {
    int rd = rand() % RAND_MAX;
    cout << "Write data " << rd << "." << endl;
    data = rd;
}
void read() {
    cout << "Read data " << data << "." << endl;
}


void *writer(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Writer(the " << c->pid << " pthread) is applying to write." << endl;
        sem_wait(&writeAccess);

        write_count++;
        if (write_count == 1) 
            sem_wait(&mutexR);
        sem_post(&writeAccess);

        sem_wait(&mutexW);
        cout << "Writer(the " << c->pid << " pthread) begins to write." << endl;
        write();

        sleep(c->lastTime);
        cout << "Writer(the " << c->pid << " pthread) stops writing." << endl;
        sem_post(&mutexW);

        sem_wait(&writeAccess);
        write_count--;
        if (write_count == 0) 
            sem_post(&mutexR);
        sem_post(&writeAccess);
        sleep(5);
        pthread_exit(0);
    }
}


void *reader(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Reader(the " << c->pid << " pthread) is applying to read." << endl;
        sem_wait(&mutexR);
        sem_wait(&readAccess);

        read_count++;
        if (read_count == 1) 
            sem_wait(&mutexW);
        sem_post(&readAccess);
        sem_post(&mutexR);

        cout << "Reader(the " << c->pid << " pthread) begins to read." << endl;
        read();

        sleep(c->lastTime);
        cout << "Reader(the " << c->pid << " pthread) stops reading." << endl;

        sem_wait(&readAccess);
        read_count--;
        if (read_count == 0) 
            sem_post(&mutexW);
        sem_post(&readAccess);
        sleep(5);
        pthread_exit(0);
    }
}


int main(int argc, char const *argv[])
{
  int number_person = atoi(argv[1]);
  sem_init(&writeAccess, 0, 1);
  sem_init(&readAccess, 0, 1);
  sem_init(&mutexR, 0, 1);
  sem_init(&mutexW, 0, 1);

  struct command information[number_person];
  pthread_t pid[number_person];
  for (int i = 0; i < number_person; i++) {
    cin >> information[i].pid >> information[i].type 
        >> information[i].startTime >> information[i].lastTime;
  }
  for (int i = 0; i < number_person; i++) {
    if (information[i].type == 'R') {
        cout << "Create a reader thread No." << information[i].pid << " pthread." << endl;
        pthread_create(&pid[i], NULL, reader, &information[i]);
    }

    if (information[i].type == 'W') {
        pthread_create(&pid[i], NULL, writer, &information[i]);
        cout << "Create a writer thread No." << information[i].pid << " pthread." << endl;
    }
  }

  for (int i = 0; i < number_person; i++) {
    pthread_join(pid[i], NULL);
  }
  sem_destroy(&writeAccess);
  sem_destroy(&readAccess);
  sem_destroy(&mutexW);
  sem_destroy(&mutexR);
  return 0;
}