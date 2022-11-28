# Assumptions / Instructions

## Q1
1. Code can be compiled using command - g++ q1.cpp, may need to add "lpthread" flag while compiling.
2. Input to be given in format - `./a.out <no. of hydrogen atoms>  <no. of oxygen atoms> <no. of sites> <threshold>.`
	Example - `./a.out 12 6 3 2`
3. For output, we are printing each reaction detail.
4. Output produced will be in format:
	```
	Reaction number:a
	Reaction completed at Site:x
	Current Chip Energy: b Remaining Hydrogen: c Remaining Oxygen: d
	```

	where, _Current Chip Energy_ refers to total energy being generated at the momenet when this reaction was undergoing.
	_Remaining Hydrogen_ and _oxygen_ signifies the number of atoms remaining for hydrogen and oxygen after this reaction gets completed.


<br>
<hr>
<br>


## Q2
 - The code has two functions `geekArrives` and `nongeekArrives` that do the job of adding their corresponding entity on the bridge for a trip, if this results in filling up the four slots they validate the arrangement and  the trip is scheduled. The bridge is a class that holds the information of the entities on the bridge and also validates arrangements.
 - The output is simply the list of people for whom the bridge trip was scheuled alog with the count of remaining geeks and nongeeks and singers.


<br>
<hr>
<br>

## Q3
 - Command to execute:
	```
	./a.out <no_of_students> <no_of_courses>
	```

__Assumptions__:
 - No. of students is a multiple of 5(as quota is 1:1:2:1) and no. of courses is a multiple of 4(as it is equally distributed in 4 knowledge spectrums).
 - No. of courses should be atleast eight as the preference list of any student is of size 8.

__Code explanation__:
- The code basically iterates over a 3d vector with 3 dimensions namely knowledge spectrum,courses in it and no of students enrolled in that course.
- A mutex lock must be acquired by any student thread before it is to be considered for allocation.
- If all the courses with constraints given in question can be allocated then that student(thread) is allocated the courses and no of seats of the student's branch
				in that course is deducted and allocation details are printed to file allocation.txt using fout of ofstream.
- If it cannot be allocated then we print to stdout that student's id saying he/she could not be allocated.
- After thread executes completely,it releases the lock which can be acquired by other threads for their execution.

__Output__:
- All the allocated students detail is written in `allocation.txt` file and all the students not allocated are printed in `stdout`.
- The no. of actual students allocated to each course of any knowledge spectrum is printed in `allocation.txt` after all the allocated students detail has been printed.


