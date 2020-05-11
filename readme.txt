Lift Sim By Moritz Bergemann
Last Updated: 11/05/2020

General Information:
- Both lift_sim_A and lift_sim_B implement the lift simulation functionality, with the former using
    threads and the latter using processes. Both function effectively identically from a user's perspective.

Compiling the Prorgam:
- Both programs can be compiled from the root directory
- The following instructions apply when running on a Linux operating system with GCC and make installed, 
    from the bash terminal while inside the assignment's main directory ("19759948_OS_Assignment")
- To compile both lift_sim_A and B, simply enter 'make' into the terminal
- To compile lift_sim_A or B individually, enter 'make lift_sim_<A or B>'
- To remove all compiled files, enter 'make clean'
- To compile both programs in debug mode (which allows debugger use and prints additional runtime
     information to the terminal), enter 'make DEBUG=1'

Running the Program:
- Run the program on Linux using './lift_sim_<A or B> <m> <n>' on the executable created during 
    compilation, where:
    - m is the buffer size (the amount of lift requests that can be loaded into memory without
        being processed at the same time) as an integer (must be at least 1)
    - n is the time taken for a lift to perform an operation in seconds as an integer,
        i.e. how long it takes a lift to move from one floor to another (must be at least 1)
- Run without any parameters after the executable name for additional information.
- The file 'sim_input.txt' is required within the folder with the executable to run the program.
    It must contain a set of 'requests' (1 on each line, recommended 50-100) for the lifts to 
    complete. Each request consists of the 2 floor numbers separated by a space, which must be 
    between 1 and the max number of floors (default 20).