
/**
 * Fibonacci Threads
 *
 * Created by Nino Lau on 2018.5.7. Copyright © Nino 2018.5.
 */
#include <iostream>
#include <pthread.h>

using namespace std;


// the size of fibonacci array
int n;

void *fibonacci(void *data) {
    int *a = (int*)data;
    // calculate the fibonacci array
    for (int i = 2; i < n; i++) {
        a[i] = a[i - 1] + a[i - 2];
    }
    pthread_exit(NULL);
}

int main() {
    cout << "Please enter the number n(n>2):" << endl;
    cin >> n;
    while (n <= 2) {
        cout << "The number should be larger than 2." << endl;
        cout << "Please enter the number n(n>2):" << endl;
        cin >> n;
    }

    int a[1000];
    // initialize
    a[0] = 0;
    a[1] = 1;

    pthread_t tid;
    // create a thread to calculate
    pthread_create(&tid, NULL, fibonacci, (void*)a);
    // a thread to be joined upon
    pthread_join(tid, NULL);

    cout << "Fibonacci: " << endl;

    // output the result
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
    return 0;
}