P2.out : Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o
	g++ -g -o P2.out Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o

Project2.o : Project2.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project2.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c SyntacticalAnalyzer.cpp

clean : 
	rm [SP]*.o P2.out *.gch

submit : Project2.cpp LexicalAnalyzer.h SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile README.txt
	rm -rf TeamNameP2
	mkdir TeamNameP2
	cp Project2.cpp TeamNameP2
	cp SyntacticalAnalyzer.h TeamNameP2
	cp SyntacticalAnalyzer.cpp TeamNameP2
	cp makefile TeamNameP2
	cp README.txt TeamNameP2
	tar cfvz TeamNameP2.tgz TeamNameP2
	cp TeamNameP2.tgz ~tiawatts/cs460drop
