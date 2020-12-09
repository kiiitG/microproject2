#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <ctime>
#include <chrono>

const int MIN = -50;
const int MAX = 50;
const int N = 100;
const int READER_COUNT = 3;
const int WRITER_COUNT = 10;

int dataBase[N];

std::chrono::steady_clock::time_point start;
sem_t reader_sem;
sem_t writer_sem;

void* Writer(void* param) {
    int id = *((int*)param);
    int data, index;
    std::chrono::steady_clock::time_point time;
    std::chrono::duration<double> diff;
    while (clock() < 10000) {
        int reader_count;
        sem_getvalue(&reader_sem, &reader_count);
        if (reader_count == 0) {
            sem_wait(&writer_sem);
            data = rand() % (MAX - MIN + 1) + MIN;
            time = std::chrono::steady_clock::now();
            diff = time - start;

            printf("[%fs Writer %d] computes value = %d\n", diff.count(), id, data);
            
            index = rand() % N;
            dataBase[index] = data;
            time = std::chrono::steady_clock::now();
            diff = time - start;

            printf("[%fs Writer %d] writes computed value = %d to cell %d\n", diff.count(), id, data, index);
            
            sem_post(&writer_sem);
            sleep(3);
        }
    }
    return nullptr;
}

void* Reader(void* param) {
    int id = *((int*)param);
    int data, index;
    std::chrono::steady_clock::time_point time;
    std::chrono::duration<double> diff;
    while (clock() < 10000) {
        sem_post(&reader_sem);
        index = rand() % N;
        time = std::chrono::steady_clock::now();
        diff = time - start;

        printf("[%fs Reader %d] computes index = %d\n", diff.count(), id, index);
        
        data = dataBase[index];
        time = std::chrono::steady_clock::now();
        diff = time - start;

        printf("[%fs Reader %d] reads value = %d from cell %d\n", diff.count(), id, data, index);
        
        sem_wait(&reader_sem);
        sleep(10);
    }
    return nullptr;
}

int main() {
    srand(clock());
    start = std::chrono::steady_clock::now();

    sem_init(&reader_sem, 0, 0);
    sem_init(&writer_sem, 0, 1);

    int i;

    pthread_t wThread[WRITER_COUNT];
    int writer[WRITER_COUNT];
    for (i = 0; i < WRITER_COUNT; i++) {
        writer[i] = i + 1;
        pthread_create(&wThread[i], nullptr, Writer, (void*)(writer + i));
    }

    pthread_t rThread[READER_COUNT];
    int reader[READER_COUNT];
    for (i = 0; i < READER_COUNT; i++) {
        reader[i] = i + 1;
        pthread_create(&rThread[i], nullptr, Reader, (void*)(reader + i));
    }

    int mId = 0;
    Writer((void*)&mId);

    return 0;
}
