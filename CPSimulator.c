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

#define CS_TIME .2              // Critical section maximum pause time 
#define VALET_SLEEP_TIME 1      // Valet maximum pause time 

Car **park;                     //  Park list to represent parking lot for the GUI
pthread_t *in_valet_threads;    // List of in valet threads
pthread_t *out_valet_threads;   // List of out valet threads
pthread_t monitor_thread;       // Monitor thread

sem_t park_empty;               // park empty semaphore implement bound protection
sem_t park_full;                // park full semaphore implement bound protection
sem_t queue_empty;              // queue empty semaphore implement bound protection
sem_t queue_full;               // queue full semaphore implement bound protection
pthread_mutex_t park_lock;      // park lock to implement mutual exclusion for park data (park, PQueue, RandList)

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
 * Version 1.00 2023/02/10
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
 * Version 1.00 2023/02/10
 */
void thread_sleep(float t)
{
    usleep(get_random(t) * 1e6);
}

/*
 * add_car_to_park:
 * A helper function to add car to the park list
 *  - car            pointer to a car
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
void add_car_to_park(Car *car)
{
    int slot = RLget();
    car->sno = slot;
    park[slot] = car;
}

/*
 * remove_car_to_park:
 * A helper function to remove car from the park list
 *  - car            pointer to a car
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/10
 */
void remove_car_from_park(Car *car)
{
    RLadd(car->sno);
    park[car->sno] = NULL;
}

/*
 * acquire_car:
 * A helper function to acquire a car by in-valet with id [id]
 *  - id             id of the in-valet to be acquired to
 *  - car            pointer to a car
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
void acquire_car(int id, Car *car)
{
    setViCar(id, car);
    car->vid = id;
    nm++;
}

/*
 * register_car:
 * A helper function to register and record a car
 *  - car            pointer to a car
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
void register_car(Car *car)
{
    car->ptm = time(NULL);
    oc++;
    pk++;                                   // record allowed car
}

/*
 * in_valet_runner:
 * in-valet thread runner this function represent the behavior of a valet, when handling arriving cars;
 * This function implement synchronization mechanism (one mutex) park_lock to Implement mutual exclusion
 * for the  shared data  (PQueue, RandList, park)  and  (4 counting semaphore) to  assures  bounded wait
 * (park_full, park_empty) for the park and (queue_full, queue_empty) for the arrival queue.
 *  - arg            uniq id for the in-valet
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
void *in_valet_runner(void *arg)
{
    int id = (intptr_t)arg;
    while (1)
    {
        sem_wait(&queue_full);              // wait for the queue if empty
        setViState(id, FETCH);              // change valet state to fetch
        Car *car = Qserve();                // serve a car
        sqw +=time(NULL) - car->atm;        // record waiting time in the queue
        sem_post(&queue_empty);             // signal queue count empty 
        acquire_car(id, car);               // acquire the car for the current valet
        thread_sleep(VALET_SLEEP_TIME);     

        setViState(id, WAIT);               // set the valet state to wait
        sem_wait(&park_empty);              // wait for the park if full
        pthread_mutex_lock(&park_lock);
        thread_sleep(CS_TIME);
        thread_sleep(VALET_SLEEP_TIME);     
        // add car to park
        setViState(id, MOVE);               // change valet state to move
        PQenqueue(car);
        add_car_to_park(car);
        register_car(car);
        thread_sleep(VALET_SLEEP_TIME);
        pthread_mutex_unlock(&park_lock);
        sem_post(&park_full);               // signal park full to allow out-valet
        nm--;   
        setViState(id, READY);
    }
}

/*
 * deregister_car:
 * A helper function record the leaving car
 *  - car            pointer to a car
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/10
 */
void deregister_car(Car *car)
{
    spt += time(NULL) - car->ptm;
    oc--;
}

/*
 * out_valet_runner:
 * out-valet thread runner this function represent the behavior of a valet when handling leaving cars;
 * This function implement synchronization mechanism (one mutex) park_lock to Implement mutual exclusion
 * for the  shared data  (PQueue, RandList, park)  and  (4 counting semaphore) to  assures  bounded wait
 * (park_full, park_empty) for the park and (queue_full, queue_empty) for the arrival queue.
 *  - arg            uniq id for the out-valet
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/10
 */
void *out_valet_runner(void *arg)
{
    int id = (intptr_t)arg;
    while (1)
    {
        sem_wait(&park_full);                   // wait if park is empty
        pthread_mutex_lock(&park_lock);
        Car *car = PQpeek();                    // get the closer car to leave
        if (time(NULL) - car->ptm >= car->ltm)  // check if the time of leave arrived
        {
            setVoCar(id, car);                  // set the car to the valet
            setVoState(id, FETCH);
            thread_sleep(CS_TIME);
            thread_sleep(VALET_SLEEP_TIME);
            // remove car from park
            PQserve();
            remove_car_from_park(car);
            deregister_car(car);
            setVoState(id, MOVE);
            thread_sleep(VALET_SLEEP_TIME);
            free(car);                          // free the allocated mommy for the removed car
            pthread_mutex_unlock(&park_lock);
            sem_post(&park_empty);              // signal park empty to allow in-valets
            setVoState(id, READY);              // change state to ready
        }
        else
        {                                       // if leaving time does not arrive
            pthread_mutex_unlock(&park_lock);   // release lock
            sem_post(&park_full);               // signal back park full
        }
    }
}

/*
 * monitor_runner:
 * monitor thread runner this function periodically print, update and graphically display 
 * the current status of the car park.
 *  - arg            null
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/10
 */
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
        // pthread_mutex_lock(&park_lock);
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
        // pthread_mutex_unlock(&park_lock);
        printf("\n");
    }
}

/*
 * format_time:
 * helper function to format a given time [t], it return a pointer of char to the formatted time
 *  - t            the time to be formatted
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/10
 */
char *format_time(time_t t)
{
    char *ft = malloc(100);
    strftime(ft, 100, "%a %b %d %H:%M:%S %Z %Y", localtime(&t));
    return ft;
}

#define safe_divide(x, y) (y != 0) ? x / y : 0; // macro to divide two number x/y and return 0 if divisor [y] is zero

/*
 * print_stat:
 * helper function to print the exit report
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/11
 */
void print_stat()
{
    double aqw = safe_divide((double)sqw, pk);
    double apt = safe_divide((double)spt, (pk - PQsize()));
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
    printf(" %-35s%f \n","Percentage of park utilization:" ,ut);
    printf("\n");
    printf("%s:\t CarPark exits.\n", format_time(time(NULL)));
    printf("\n");
}

/*
 * sig_handler:
 * ctrl-c signal handler this function cancel all created threads, free all allocated memory, close the GUI
 * and print the exit status report
 *  - signum            signal number
 * 
 * Author Turki Safar Alzahrani
 * Version 1.00 2023/02/11
 */
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

/*
 * validate_args:
 * helper function to validate user entered values where there need to be :
 *  psize  : multiple of 4 and between [12, 40]
 *  inval  : between [1, 6]  
 *  outval : between [1, 6]  
 *  qsize  : between [3, 8] 
 *  expnum : between [0.01, 1.5] 
 * 
 * if one of them is breaking the rule print an error and return false
 * otherwise return true
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
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
    return true;
}

/*
 * init_sim:
 * helper function to validate user entered values where there need to be :
 *  psize  : multiple of 4 and between [12, 40]
 *  inval  : between [1, 6]  
 *  outval : between [1, 6]  
 *  qsize  : between [3, 8] 
 *  expnum : between [0.01, 1.5] 
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/10
 */
bool init_sim(int argc, char const *argv[])
{
    int args[4] = {psize, inval, outval, qsize};                        // list to store int user input, initialized to default value

    if (argc > 6)                                                       // show error message to user if the number of input is more than 5
    {
        printf("Too many arguments provided.\n Usage : $ ./carpark [psize inval outval qsize expnum].\n");
        return false;
    }
    int nargs;
    if (argc == 6) expnum = atof(argv[5]);                                  // if user input all args store the expnum
    nargs = argc - 1;                                                       // get number user inputs

    for (size_t i = 0; i < nargs; i++) args[i] = atoi(argv[i + 1]);         // loop to take user input and replace with defaults
    psize = args[0];
    inval = args[1];
    outval = args[2];
    qsize = args[3];
    if (!validate_args()) return false;                                     // validate user input arguments

    pthread_mutex_init(&park_lock, NULL);                                   // init park lock to protect PQueue, park, and RandList
    sem_init(&park_full, 0, 0);                                             // init full parking semaphore to 0
    sem_init(&park_empty, 0, psize);                                        // init empty parking semaphore to size of park
    sem_init(&queue_full, 0, 0);                                            // init full queue semaphore to 0
    sem_init(&queue_empty, 0, qsize);                                       // init empty parking semaphore to size of queue
    Qinit(qsize);                                                           // init Queue
    PQinit(psize);                                                          // init PQueue
    RLinit(psize);                                                          // init Random List
    park = (Car **)malloc(psize * sizeof(Car *));                           // allocate list to represent parking slots
    G2DInit(park, psize, inval, outval, park_lock);                         // init simulator GUI
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
    return true;
}

/*
 * create_new_car:
 * A helper function to create new car and add it to the arrival queue
 *  - car            pointer to a car
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/09
 */
void create_new_car()
{
    Car *car = (Car *)malloc(sizeof(Car));      // allocate memory for a new car
    CarInit(car);                               // init car
    nc++;                                       // record the creation of new car
    if (!QisFull())
    {
        sem_wait(&queue_empty);                 
        Qenqueue(car);                          // insert the created car to the arrival queue
        sem_post(&queue_full);                  // signal queue full semaphore to allow in-valets
    }
    else
    {
        free(car);                              // free allocated memory for car
        rf++;                                   // record rejected car
    }
}

/*
 * main:
 * The main thread this function initialize the simulator, register a signal handler to hand ctrl-c signal
 * then enter an endless loop where it randomly create incoming cars with Poisson distribution set.
 *  - argc            Number of entered arguments
 *  - argv            List of entered arguments
 * 
 * Author Abdullah Khamis Albijali
 * Version 1.00 2023/02/09
 */
int main(int argc, char const *argv[])
{
    sbt = time(NULL);                       // record Simulator start time
    signal(SIGINT, sig_handler);            // Register SIGINT signal handler
    signal(SIGTERM, sig_handler);            // Register SIGTERM signal handler
    if (!init_sim(argc, argv))  return 1;   // initialize simulator
    // enter endless loop
    while (1)
    {
        int n_cars = newCars(expnum);       // the random number of incoming cars with Poisson distribution set
        for (size_t i = 0; i < n_cars; i++) 
        {
            create_new_car();               // create new car
        }
        sleep(1);
    }

    return 0;
}
