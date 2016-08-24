all: circbuf

circbuf: circbuf.c
	gcc circbuf.c -o circbuf

clean:
	rm -f circbuf
