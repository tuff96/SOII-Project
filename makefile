#obj=
tema1:
	gcc -Wall -o count count.c
.PHONY:clean
clean:
	-rm count
.PHONY:run
run:
	./count
