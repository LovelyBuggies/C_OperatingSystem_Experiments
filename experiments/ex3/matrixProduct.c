/**
 * Matrix Production
 *
 * Created by Nino Lau on 2018.5.7. Copyright © Nino 2018.5.
 */
#include <iostream>
#include <stdlib.h>
#include <pthread.h>

using namespace std;


int M, K, N;
// the size of matrix
int A[50][50];
int B[50][50];
int C[50][50];

// structure for passing data to threads
struct node
{
    int i, j;
};

// calculate value in thread which represents C[row][col]
void *calculate(void *data) {
    struct node *temp = (struct node*)data;
    int i = temp->i;
    int j = temp->j;
    for (int k = 0; k < K; k++) {
        C[i][j] += A[i][k] * B[k][j];
    }
    pthread_exit(NULL);
}

int main() {

    // initial arguments
    cout << "Please enter three numbers(M/K/N) that are less than 50:" << endl;
    cin >> M >> K >> N;

    // initial matrix A
    cout << "Please enter the first matrix(M*K):" << endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            cin >> A[i][j];
        }
    }

    // initial matrix B
    cout << "Please enter the second matrix(K*N):" << endl;
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            cin >> B[i][j];
        }
    }

    // initial matrix C
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }

    pthread_t tid[M * N];
    pthread_attr_t attr;
    // get the default attributes
    pthread_attr_init(&attr);

    // create M*N pthreads
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            struct node *temp = (struct node*)malloc(sizeof(struct node));
            temp->i = i;
            temp->j = j;
            pthread_create(&tid[i * N + j], &attr, calculate, (void*)temp);
        }
    }

    // join upon each thread
    for (int i = 0; i < M * N; i++) {
        pthread_join(tid[i], NULL);
    }

    // output the result
    cout << "The result(M*N) is:" << endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << C[i][j] << " ";
            if (j == N - 1) cout << endl;
        }
    }
    return 0;
}