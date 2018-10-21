all: repl

repl: repl.c
	gcc -o repl repl.c -Wall

clean:
	rm repl
