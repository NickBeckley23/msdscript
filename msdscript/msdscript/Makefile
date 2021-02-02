INCS = cmdline.h catch.h Expr.h

OBJS = main.o cmdline.o Expr.o

CXX = c++
CXXFLAGS = --std=c++14 -O2

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

.PHONY: test
test: msdscript
	./msdscript --test

main.o: main.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c main.cpp

cmdline.o: cmdline.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c cmdline.cpp

Expr.o: Expr.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c Expr.cpp
