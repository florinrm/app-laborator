build:
	gcc static_schedule.c -o static_schedule -fopenmp
	gcc dynamic_schedule.c -o dynamic_schedule -fopenmp
	gcc auto_schedule.c -o auto_schedule -fopenmp
	gcc guided_schedule.c -o guided_schedule -fopenmp
	gcc nowait.c -o nowait -fopenmp
clean:
	rm static_schedule dynamic_schedule auto_schedule nowait guided_schedule