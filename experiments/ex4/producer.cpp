#include <sys/types.h>  
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h> 
#include <cstring>
#include <cstdlib> 
#include <iostream>
#include <fstream>
#define BUFFER_SIZE 10  
using namespace std;

int total_producer = 0, total_consumer = 0;
int buffer[BUFFER_SIZE];
int nextP = 0, nextC = 0;
sem_t empty, full, nmutex;

struct command
{
    int pid;
    char type;
    int startTime;
    int lastTime;
    int num;
};

void *producer(void *param) {
    struct command* data = (struct command*)param;

    while (true) {
        sem_wait(&empty);
        sleep(data->startTime);
        sem_wait(&nmutex);

        buffer[nextP] = data->num;
        cout << "Producer No." << data->pid 
            << " produces " << "product No." << data->num << endl;
        nextP = (nextP + 1) % BUFFER_SIZE;

        sleep(data->lastTime);
        sem_post(&nmutex);
        sem_post(&full);
        pthread_exit(0);
    }
}

void *consumer(void *param) {
    struct command* data = (struct command*)param;

    while (true) {
        sem_wait(&full);
        sleep(data->startTime);
        sem_wait(&nmutex);

        cout << "Consumer No." << data->pid 
            << " consumes " << "product No." << buffer[nextC] << endl;
        buffer[nextC] = 0;
        nextC = (nextC + 1) % BUFFER_SIZE;

        sleep(data->lastTime);
        sem_post(&nmutex);
        sem_post(&empty);
        pthread_exit(0);
    }
}

int main(int argc, char const *argv[])
{
    int total = atoi(argv[1]);
    struct command information[total];
    pthread_t Pid[total];

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&nmutex, 0, 1);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
    ifstream in;
    in.open("./data.txt");
    if (!in.is_open()) {
        exit(1);
    }

    char buffer[256];
    int j = 0;
    char ch;
    while (!in.eof()) {
        in.read(&ch, 1);
        if (ch != ' ' && ch != '\n' && ch != '\r') 
            buffer[j++] = ch;
    }
    j = 0;

    for (int i = 0; i < total; i++) {
        information[i].pid = buffer[j++] - '0';
        information[i].type = buffer[j++];
        information[i].startTime = buffer[j++] - '0';
        information[i].lastTime = buffer[j++] - '0';
        if (information[i].type == 'P') 
            information[i].num = buffer[j++] - '0';
    }
    for (int i = 0; i < total; i++) {
        if (information[i].type == 'P') {
            total_producer++;
            cout << "Create a producer thread No." << information[i].pid << endl;
            pthread_create(&Pid[i], NULL, producer, &information[i]);
        }

        else if (information[i].type == 'C') {
            total_consumer++;
            pthread_create(&Pid[i], NULL, consumer, &information[i]);
            cout << "Create a consumer thread No." << information[i].pid << endl;
        }
    }

    for (int i = 0; i < total; i++) {
        pthread_join(Pid[i], NULL);
    }
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&nmutex);
    return 0;
}
