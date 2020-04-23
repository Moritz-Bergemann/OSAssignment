Lift Sim By Moritz Bergemann
Last Updated: 23/04/2020

General Information:
- The folders 'PartA' and 'PartB' contain the source code for implementations of the 
    lift simulator using threads and processes respectively. Both programs function identically
    at the interface-level.

How To Compile/Run:
- Invoking the 'make' command from the command line from either the 'PartA' or 'PartB' folder
    will compile the respective program into an executable called 'lift_sim_A' or 'lift_sim_B'
    depending on the folder.
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