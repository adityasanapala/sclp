CC     = g++
LEX    = flex
YACC   = bison
CFLAGS = -Wall -Wno-unused-function -Wno-sign-compare -std=c++11

TARGET = sclp
OBJS   = lex.yy.o parser.tab.o main.o tac.o rtl.o #spim.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

parser.tab.c parser.tab.h y.output: parser.y
	$(YACC) -d -v -o parser.tab.c parser.y && mv parser.output y.output

lex.yy.c: scanner.l parser.tab.h
	$(LEX) -o lex.yy.c scanner.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c ast.hpp symtab.hpp
	$(CC) $(CFLAGS) -c parser.tab.c

tac.o: tac.cpp tac.hpp ast.hpp
	$(CC) $(CFLAGS) -c tac.cpp

rtl.o: rtl.cpp rtl.hpp tac.hpp ast.hpp
	$(CC) $(CFLAGS) -c rtl.cpp

#spim.o: spim.cpp spim.hpp tac.hpp ast.hpp
#	$(CC) $(CFLAGS) -c spim.cpp

main.o: main.cpp ast.hpp tac.hpp rtl.hpp #spim.hpp parser.tab.h
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c parser.tab.c parser.tab.h parser.output y.output
	rm -f *.toks *.ast *.tac *.rtl *.spim *.log

.PHONY: all clean