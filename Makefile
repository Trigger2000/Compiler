.PHONY: all clear

all:
	g++ -c parse_tree.cpp
	bison -d --output=b.tab.cpp b.y
	flex --outfile=lex.yy.cpp l.lex
	g++ -fshort-enums parse_tree.o b.tab.cpp lex.yy.cpp
	#g++ -fshort-enums start.cpp -o start

clear:
	rm -r a.out b.tab.cpp b.tab.hpp lex.yy.cpp parse_tree.o #start
