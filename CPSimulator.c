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

#define CS_TIME .2
#define VALET_SLEEP_TIME 1
Car **park;
pthread_t *in_valet_threads;
pthread_t *out_valet_threads;
pthread_t monitor_thread;

sem_t parking_empty;
sem_t parking_full;
sem_t queue_empty;
sem_t queue_full;
pthread_mutex_t parking_lock;

// input parameters
int psize = PARK_SIZE;      // Park capacity
int inval = IN_VALETS;      // Number of in-valet
int outval = OUT_VALETS;    // Number of out-valet
int qsize = QUEUE_SIZE;     // Arrival queue capacity
double expnum = EXP_CARS;   // Expected number of arrivals, used in the Poisson random number

// stat
int oc = 0;                 // Current number of occupied slots in the parking space
int nc = 0;                 // Running total number of cars created during the simulation
int pk = 0;                 // Running total number of cars allowed to park
int rf = 0;                 // Running total number of cars not allowed to park
int nm = 0;                 // The number of cars currently acquired by in-valets
time_t sqw = 0;             // Running sum of car-waiting times in the arrival queue
time_t spt = 0;             // Running sum of car-parking durations
time_t sbt = 0;             // Simulator begin time
time_t sst = 0;             // Simulator stop time
double ut = 0;              // Current car-park space utilization


/*
 * get_random:
 * A helper function to generate random number between 0 and [max]
 *  - max            the maximum value to be generated
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/09
 */
double get_random(double max)
{
    return (double)rand() / RAND_MAX * max;
}
/*
 * thread_sleep:
 * A helper function to pause the caller thread for a random period (up to [t] seconds)
 *  - t            maximum time to pause (Unit second)
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/09
 */
void thread_sleep(float t)
{
    usleep(get_random(t) * 1e6);
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

void acquire_car(int id, Car *car)
{
    setViCar(id, car);
    car->vid = id;
    nm++;
}
void register_car(Car *car)
{
    car->ptm = time(NULL);
    oc++;
}
void *in_valet_runner(void *arg)
{

    int id = (intptr_t)arg;
    while (1)
    {
        sem_wait(&queue_full);
        setViState(id, WAIT);
        Car *car = Qserve();
        sem_post(&queue_empty);
        acquire_car(id, car);

        time_t begin_waiting = time(NULL);
        sem_wait(&parking_empty);
        time_t end_waiting = time(NULL);
        sqw += end_waiting - begin_waiting;

        setViState(id, FETCH);
        // Add car to parking lot
        pthread_mutex_lock(&parking_lock);
        thread_sleep(CS_TIME);
        setViState(id, MOVE);
        thread_sleep(VALET_SLEEP_TIME);
        PQenqueue(car);
        add_car_to_park(car);
        register_car(car);
        thread_sleep(VALET_SLEEP_TIME);
        pthread_mutex_unlock(&parking_lock);
        sem_post(&parking_full);
        nm--;
        setViState(id, READY);
    }
}
void deregister_car(Car *car)
{
    spt += time(NULL) - car->ptm;
    oc--;
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
            thread_sleep(CS_TIME);
            thread_sleep(VALET_SLEEP_TIME);
            PQserve();
            remove_car_from_park(car);
            deregister_car(car);
            thread_sleep(VALET_SLEEP_TIME);
            setVoState(id, MOVE);
            free(car);
            pthread_mutex_unlock(&parking_lock);
            sem_post(&parking_empty);
            setVoState(id, READY);
        }
        else
        {
            pthread_mutex_unlock(&parking_lock);
            sem_post(&parking_full);
        }
    }
}
void *monitor_runner(void *arg)
{
    while (1)
    {
        ut = ((double)oc / psize) * 100;
        updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
        show();
        // print separator line
        printf("+------+");
        for (int i = 0; i < psize; i++)
        {
            printf("----+");
        }
        printf("\n");

        // print slots
        printf("| Slot |");
        for (int i = 0; i < psize; i++)
        {
            printf(" %2d |", i + 1);
        }
        printf("\n");

        // print parking lot
        printf("| Park |");
        // pthread_mutex_lock(&parking_lock);
        for (int i = 0; i < psize; i++)
        {
            Car *car = park[i];
            if (car)
            {
                printf(" %2d |", park[i]->cid);
            }
            else
            {
                printf("    |");
            }
        }
        // pthread_mutex_unlock(&parking_lock);
        printf("\n");
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
char *format_time(time_t t)
{
    char *ft = malloc(100);
    strftime(ft, 100, "%a %b %d %H:%M:%S %Z %Y", localtime(&t));
    return ft;
}
#define average(x, y) (y != 0) ? (double)x / y : 0;
void print_stat()
{
    double aqw = average(sqw, pk);
    double apt = average(spt, pk);
    printf("\n");
    printf("%-35s %s\n","Simulator started at:" ,format_time(sbt));
    printf(" %-35s%d cars.\n","Park Space Capacity was:", psize);
    printf(" %-35s%d cars.\n", "Allowed queue length was:", qsize);
    printf(" %-35s%d.\n","Number of in valets was:", inval);
    printf(" %-35s%d.\n","Number of out valets was:" ,outval);
    printf(" %-35s%f.\n","Expected arrivals was:" ,expnum);
    printf("%-35s %s\n","Simulator stopped at:" ,format_time(sst));
    printf("\n");
    printf("%-35s %ld seconds\n","CP Simulation was executed for:" ,sst - sbt);
    printf("%-35s %d cars\n","Total number of cars processed:" ,nc);
    printf(" %-35s%d cars\n","Number of cars that parked:" ,pk);
    printf(" %-35s%d cars\n","Number of cars turned away:" ,rf);
    printf(" %-35s%d cars\n","Number of cars in transit:" ,nm);
    printf(" %-35s%d cars\n","Number of cars still queued:" ,Qsize());
    printf(" %-35s%d cars\n","Number of cars still parked:" ,PQsize());
    printf("\n");
    printf(" %-35s%f seconds\n","Average queue waiting time:" ,aqw);
    printf(" %-35s%f seconds\n","Average parking time:" ,apt);
    printf(" %-35s%f \n","Percentage of park utilization:" ,apt);
    printf("\n");
    printf("%s:\t CarPark exits.\n", format_time(time(NULL)));
    printf("\n");
}
void sig_handler(int signum)
{
    printf("\n");
    printf("%s:\t Received shutdown signal ...\n", format_time(time(NULL)));
    printf("%s:\t Car park is shutting down ...\n", format_time(time(NULL)));
    printf("%s:\t The valets are leaving ...\n", format_time(time(NULL)));
    sst = time(NULL);
    // cancel threads
    for (int i = 0; i < inval; i++)
    {
        pthread_cancel(in_valet_threads[i]);
    }
    for (int i = 0; i < outval; i++)
    {
        pthread_cancel(out_valet_threads[i]);
    }
    printf("%s:\t Done. %d valets left.\n", format_time(time(NULL)), inval + outval);
    printf("%s:\t Monitor exiting ...\n", format_time(time(NULL)));
    pthread_cancel(monitor_thread);
    // free allocated memory
    Qfree();
    PQfree();
    RLfree();
    free(park);
    free(in_valet_threads);
    free(out_valet_threads);
    print_stat();
    // shutdown the graphical system
    finish();
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
        return false; // validate user input arguments

    pthread_mutex_init(&parking_lock, NULL);                                // init park lock to protect PQueue, park, and RandList
    sem_init(&parking_full, 0, 0);                                          // init full parking semaphore to 0
    sem_init(&parking_empty, 0, psize);                                     // init empty parking semaphore to size of park
    sem_init(&queue_full, 0, 0);                                            // init full queue semaphore to 0
    sem_init(&queue_empty, 0, qsize);                                       // init empty parking semaphore to size of queue
    Qinit(qsize);                                                           // init Queue
    PQinit(psize);                                                          // init PQueue
    RLinit(psize);                                                          // init Random List
    park = (Car **)malloc(psize * sizeof(Car *));                           // allocate list to represent parking slots
    G2DInit(park, psize, inval, outval, parking_lock);                      // init simulator GUI
    in_valet_threads = (pthread_t *)malloc(inval * sizeof(pthread_t));      // allocate list to store in-valet threads
    out_valet_threads = (pthread_t *)malloc(outval * sizeof(pthread_t));    // allocate list to store out-valet threads

    pthread_create(&monitor_thread, NULL, monitor_runner, NULL);            // init monitor thread
    // init in-valets threads
    for (size_t i = 0; i < inval; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, in_valet_runner, (void *)i);
        in_valet_threads[i] = thread;
    }
    // init out-valets threads
    for (size_t i = 0; i < outval; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, out_valet_runner, (void *)i);
        out_valet_threads[i] = thread;
    }
}
void create_new_car()
{
    if (!QisFull())
    {
        Car *car = (Car *)malloc(sizeof(Car));
        CarInit(car);
        sem_wait(&queue_empty);
        Qenqueue(car);
        thread_sleep(CS_TIME);
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
    sbt = time(NULL);
    if (!init_sim(argc, argv))
        return 1;

    while (1)
    {
        int n_cars = newCars(expnum);
        for (size_t i = 0; i < n_cars; i++)
        {
            create_new_car();
        }
        sleep(1);
    }

    return 0;
}
