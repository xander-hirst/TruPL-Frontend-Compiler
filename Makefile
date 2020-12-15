token.o:	token.h token.cpp
	clang++ -c -g -Wall --pedantic token.cpp

keywordtoken.o:	keywordtoken.h keywordtoken.cpp token.h
	clang++ -c -g -Wall --pedantic keywordtoken.cpp

punctoken.o:	punctoken.h punctoken.cpp token.h
	clang++ -c -g -Wall --pedantic punctoken.cpp

reloptoken.o:	reloptoken.h reloptoken.cpp token.h
	clang++ -c -g -Wall --pedantic reloptoken.cpp

addoptoken.o:	addoptoken.h addoptoken.cpp token.h
	clang++ -c -g -Wall --pedantic addoptoken.cpp

muloptoken.o:	muloptoken.h muloptoken.cpp token.h
	clang++ -c -g -Wall --pedantic muloptoken.cpp

idtoken.o:	idtoken.h idtoken.cpp token.h
	clang++ -c -g -Wall --pedantic idtoken.cpp

numtoken.o:	numtoken.h numtoken.cpp token.h
	clang++ -c -g -Wall --pedantic numtoken.cpp

eoftoken.o:	eoftoken.h eoftoken.cpp token.h
	clang++ -c -g -Wall --pedantic eoftoken.cpp

buffer.o:	buffer.h buffer.cpp
	clang++ -c -g -Wall --pedantic buffer.cpp

scanner.o:	scanner.h scanner.cpp buffer.h token.h keywordtoken.h \
		punctoken.h reloptoken.h addoptoken.h muloptoken.h \
		idtoken.h numtoken.h eoftoken.h
	clang++ -c -g -Wall --pedantic scanner.cpp


symbol_table.o:	symbol_table.h symbol_table.cpp
	clang++ -c -g -Wall --pedantic symbol_table.cpp

parser.o:	parser.h parser.cpp scanner.h buffer.h \
		token.h keywordtoken.h punctoken.h \
		reloptoken.h addoptoken.h muloptoken.h \
		idtoken.h numtoken.h eoftoken.h \
		symbol_table.h
	clang++ -c -g -Wall --pedantic parser.cpp

truc.o:		truc.cpp scanner.h token.h keywordtoken.h \
		punctoken.h reloptoken.h addoptoken.h muloptoken.h \
		idtoken.h numtoken.h eoftoken.h parser.h symbol_table.h
	clang++ -c -g -Wall --pedantic truc.cpp

truc: 	 	truc.o parser.o scanner.o buffer.o token.o keywordtoken.o \
		punctoken.o reloptoken.o addoptoken.o \
		muloptoken.o idtoken.o numtoken.o eoftoken.o symbol_table.o
	clang++ -o truc -g -Wall --pedantic scanner.o \
		buffer.o eoftoken.o numtoken.o \
		idtoken.o muloptoken.o \
		addoptoken.o reloptoken.o punctoken.o keywordtoken.o \
		token.o parser.o truc.o symbol_table.o

clean:	
	rm -f *.o test_scanner

all:	truc
