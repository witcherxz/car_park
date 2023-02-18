#include "CarPark.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "PQueue.h"
#include "RandList.h"

#define CS_TIME             .2
#define VALET_SLEEP_TIME    1
Car **park;
pthread_t *in_valet_threads;
pthread_t *out_valet_threads;
pthread_t monitor_thread;
pthread_t gui_thread;

sem_t parking_empty;
sem_t parking_full;
sem_t queue_empty;
sem_t queue_full;
pthread_mutex_t parking_lock;
pthread_mutex_t queue_lock;

// input parameters
int psize = PARK_SIZE;
int inval = IN_VALETS;
int outval = OUT_VALETS;
int qsize = QUEUE_SIZE;
double expnum = EXP_CARS;

// stat
int oc = 0;
int nc = 0;
int pk = 0;
int rf = 0;
int nm = 0;  // TODO: implement this stat
int sqw = 0; // TODO: implement this stat
int spt = 0; // TODO: implement this stat
int ut = 0;  // TODO: implement this stat


double get_random(double max)
{
    return (double)rand() / RAND_MAX * max;
}
void add_car_to_park(Car *car)
{
    int slot = RLget();
    car->sno = slot;
    park[slot] = car;
}

void remove_car_from_park(Car *car)
{
    RLadd(car->sno);
    park[car->sno] = NULL;
}
void thread_sleep(float t)
{
    usleep(get_random(t) * 1e6);
}
void *in_valet_runner(void *arg)
{

    int id = (intptr_t)arg;
    while (1)
    {
        sem_wait(&queue_full);
        Car *car = Qpeek();
        setViCar(id, car);
        car->vid = id;
        setViState(id, WAIT);
        sem_wait(&parking_empty);
        pthread_mutex_lock(&parking_lock);
        // pthread_mutex_lock(&queue_lock);
        setViState(id, FETCH);
        thread_sleep(VALET_SLEEP_TIME);
        Qserve();
        thread_sleep(CS_TIME);
        // pthread_mutex_unlock(&queue_lock);
        sem_post(&queue_empty);
        nm++;
        int value;
        // Add car to parking lot
        setViState(id, MOVE);
        thread_sleep(VALET_SLEEP_TIME);
        car->ptm = time(NULL);
        add_car_to_park(car);
        PQenqueue(car);
        oc++;
        thread_sleep(VALET_SLEEP_TIME);
        thread_sleep(CS_TIME);
        pthread_mutex_unlock(&parking_lock);
        sem_post(&parking_full);
        nm--;
        setViState(id, READY);
    }
}

void *out_valet_runner(void *arg)
{
    int id = (intptr_t)arg;
    while (1)
    {
        sem_wait(&parking_full);
        pthread_mutex_lock(&parking_lock);
        Car *car = PQpeek();
        if (time(NULL) - car->ptm >= car->ltm)
        {
            setVoCar(id, car);
            setVoState(id, WAIT);
            // pthread_mutex_lock(&parking_lock);
            thread_sleep(VALET_SLEEP_TIME);
            PQserve();
            remove_car_from_park(car);
            setVoState(id, MOVE);
            oc--;
            thread_sleep(VALET_SLEEP_TIME);
            thread_sleep(CS_TIME);
            free(car);
            pthread_mutex_unlock(&parking_lock);
            sem_post(&parking_empty);
            setVoState(id, READY);
        }else{
            sem_post(&parking_full);
            pthread_mutex_unlock(&parking_lock);
        }
    }
}
//FIX: SEG fault caused by queue
void *monitor_runner(void *arg)
{
    // TODO : check monitor implementation
    while (1)
    {
        // sleep(1);
        show();
        Qdisp();
        // print separator line
        // printf("+------+");
        // for (int i = 0; i < psize; i++)
        // {
        //     printf("----+");
        // }
        // printf("\n");

        // // print slots
        // printf("| Slot |");
        // for (int i = 0; i < psize; i++)
        // {
        //     printf(" %2d |", i + 1);
        // }
        // printf("\n");

        // // print parking lot
        // printf("| Park |");
        // // pthread_mutex_lock(&parking_lock);
        // for (int i = 0; i < psize; i++)
        // {
        //     Car * car = park[i];
        //     if (car)
        //     {
        //         printf(" %2d |", park[i]->cid);
        //     }
        //     else
        //     {
        //         printf("    |");
        //     }
        // }
        // // pthread_mutex_unlock(&parking_lock);
        // printf("\n");
    }
}

bool validate_args()
{
    // Validate arguments
    if (psize % 4 != 0 || psize < 12 || psize > 40)
    {
        printf("Error: psize must be an integer multiple of 4 in the range [12 - 40].\n");
        return false;
    }
    if (inval < 1 || inval > 6)
    {
        printf("Error: inval must be an integer in the range [1 - 6].\n");
        return false;
    }
    if (outval < 1 || outval > 6)
    {
        printf("Error: outval must be an integer in the range [1 - 6].\n");
        return false;
    }
    if (qsize < 3 || qsize > 8)
    {
        printf("Error: qsize must be an integer in the range [3 - 8].\n");
        return false;
    }
    if (expnum < 0.01 || expnum > 1.50)
    {
        printf("Error: expnum must be a real number in the range [0.01 - 1.50].\n");
        return false;
    }
}
void sig_handler(int signum)
{
    // cancel threads
    for (int i = 0; i < inval; i++)
    {
        pthread_cancel(in_valet_threads[i]);
    }
    for (int i = 0; i < outval; i++)
    {
        pthread_cancel(out_valet_threads[i]);
    }
    pthread_cancel(monitor_thread);
    pthread_cancel(gui_thread);
    // free allocated memory
    PQfree();
    Qfree();
    RLfree();
    free(park);
    free(in_valet_threads);
    free(out_valet_threads);
    // shutdown the graphical system
    finish();
    // TODO: print stat
    exit(0); // exit
}
bool init_sim(int argc, char const *argv[])
{
    int args[4] = {psize, inval, outval, qsize};

    if (argc > 6)
    {
        printf("Too many arguments provided.\n Usage : $ ./carpark [psize inval outval qsize expnum].\n");
        return false;
    }
    int nargs;
    if (argc > 5)
    {
        nargs = 5;
        expnum = atof(argv[5]);
    }
    else
    {
        nargs = argc - 1;
    }

    for (size_t i = 0; i < nargs; i++)
    {
        args[i] = atoi(argv[i + 1]);
    }
    psize = args[0];
    inval = args[1];
    outval = args[2];
    qsize = args[3];
    if (!validate_args())
        return false;

    pthread_mutex_init(&parking_lock, NULL);
    pthread_mutex_init(&queue_lock, NULL);
    
    sem_init(&parking_full, 0, 0);
    sem_init(&parking_empty, 0, psize);
    sem_init(&queue_full, 0, 0);
    sem_init(&queue_empty, 0, qsize);
    Qinit(qsize);  // init Queue
    PQinit(psize); // init PQueue
    RLinit(psize); // init Random List
    park = (Car **)malloc(psize * sizeof(Car *));
    G2DInit(park, psize, inval, outval, parking_lock);
    in_valet_threads = (pthread_t *)malloc(inval * sizeof(pthread_t));
    out_valet_threads = (pthread_t *)malloc(inval * sizeof(pthread_t));
    // init in-valets threads
    // pthread_create(&gui_thread, NULL, gui_runner, NULL);
    pthread_create(&monitor_thread, NULL, monitor_runner, NULL);
    for (size_t i = 0; i < inval; i++)
    {
        pthread_t thread;
        in_valet_threads[i] = thread;
        pthread_create(&thread, NULL, in_valet_runner, (void *)i);
    }
    // init out-valets threads
    for (size_t i = 0; i < outval; i++)
    {
        pthread_t thread;
        out_valet_threads[i] = thread;
        pthread_create(&thread, NULL, out_valet_runner, (void *)i);
    }

}

void register_new_car()
{
    if (!QisFull())
    {
        Car *car = (Car *)malloc(sizeof(Car));
        CarInit(car);
        // printf("Created Car : {cid: %d, vid: %d, sno: %d}\n", car->cid, car->vid, car->sno);
        sem_wait(&queue_empty);
        // pthread_mutex_lock(&parking_lock);
        // pthread_mutex_lock(&queue_lock);
        Qenqueue(car);
        thread_sleep(CS_TIME);
        // pthread_mutex_unlock(&queue_lock);
        // pthread_mutex_unlock(&parking_lock);
        sem_post(&queue_full);
        nc++;
        pk++;
    }
    else
    {
        rf++;
    }
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, sig_handler); // Register signal handler

    if (!init_sim(argc, argv))
        return 1;

    while (1)
    {
        // if(PQisFull()) can_in_valet = false;
        int n_cars = newCars(expnum);
        for (size_t i = 0; i < n_cars; i++)
        {
            register_new_car();
        }
        int size;
        PQiterator(&size);
        // if(!QisFull()) printf("N cars : %d\n", n_cars);
        sleep(1);
        // setViCar(car.vid, &car);
    }

    return 0;
}
