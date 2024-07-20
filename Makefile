SOURCES=main.c

all: main

main:
	gcc -o fetchnip $(SOURCES)

clean:
	rm -rf fetchnip *.o
