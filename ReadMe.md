# Car-park Simulator
<p align="center">
  <img src="https://github.com/witcherxz/car_park/blob/main/CP_Example.jpg"  width="35%" height="35%">
  </p>

  This simulator is intended to visualize graphically how multiple threads
  and thread pools interact together by communicating through shared data structures
  to correctly solve common OS problems. 

The Graphical Display:
  The screen shot image shows a graphical window showing four areas:
  
    - The parking space:
        Has the entered capacity of parking slots, with division lines and numbered
        labels. This space will change the scale according to the given size.
         * Parked cars will show "V#-C#" above them and "THH:MM:SS" beneath them,
           where, V# is in-valet-id, C# is Car-id, identifying the car and in-valet
           who parked it, and the time it was parked in hour:minute:second format.

    - Queue of arriving cars:
        In the simulated street north of the park. It can hold upto 8 waiting cars,
        but will be limited by your input parameter. Each car will be displayed with
        its car ID labeled on top of the car. Current queue size or "Queue Full" are
        also indicated as appropriate.

    - Valets rooms:
        The rectangles north of the street. The one on the left is for in-valets,
        and the one on the right is for out-valets. They are labeled with the number
        of valets in each category, and they contain a number of small color-filled
        circles, each one's color representing the valet's state with his/her ID
        displayed within the circle.
         * The left rectangle also displays the legend used for valet states:
            - Green color:   Valet is ready.
            - Yellow color:  Valet is fetching a car
            - Red color:     Valet is waiting for something
            - Magenta color: Valet has acquired the car whose ID is displayed beneath
                             his/her circle and is moving it to its destination
         * Single characters appear on the left side of each rectangle to identify the
           line where data related to a particular valet is displayed as follows:
            - "N" : Number of cars the valet have served so far
            - "V" : The valet ID displayed in the valet representing circle
            - "C" : The ID of the car currently acquired and being moved by the valet
            - "S" : The slot number where a leaving car was parked
            - "T" : The duration of time a leaving car stayed parked

        The number of circles in each rectangle is variable according to the input
        parameters, but they are limited by 6 valets of each type.

    - Statistics Area:
        Across the top of the graphical window and in between the two valet rooms will
        be displayed several running statistics of the simulation as indicated by
        their labels. Also, the number of available slots or "Park Full" is indicated
        in this area as appropriate while the simulator is running.
         * A running clock time is displayed on the street border.
