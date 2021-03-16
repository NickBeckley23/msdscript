INCS = cmdline.h catch.h Expr.h Parse.h Val.h pointer.h

INCS2 = ../test_msdscript/test_msdscript/exec.hpp

OBJS = main.o cmdline.o Expr.o Parse.o Val.o

OBJS2 = ../test_msdscript/test_msdscript/main.o ../test_msdscript/test_msdscript/exec.o

CXX = c++
CXXFLAGS = --std=c++14 -Wall -O2

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

test_msdscript: $(OBJS2)
	$(CXX) $(CXXFLAGS) -o ../test_msdscript/test_msdscript/test_msdscript $(OBJS2)

.PHONY: test
test: msdscript
	./msdscript --test

main.o: main.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c main.cpp

main.o: ../test_msdscript/test_msdscript/main.cpp $(INCS2)
	$(CXX) $(CXXFLAGS) -c main.cpp

exec.o: ../test_msdscript/test_msdscript/exec.cpp $(INCS2)
	$(CXX) $(CXXFLAGS) -c exec.cpp

cmdline.o: cmdline.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c cmdline.cpp

Expr.o: Expr.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c Expr.cpp

Parse.o: Parse.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c Parse.cpp

Val.o: Val.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c Val.cpp
