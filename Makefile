.PHONY: all clear

all:
	g++ -c real.cpp
	bison -d b.y
	flex l.lex
	g++ -fshort-enums real.o b.tab.c lex.yy.c
	g++ -fshort-enums start.cpp -o start

clear:
	rm -r a.out b.tab.c b.tab.h lex.yy.c real.o start
