P3.out : Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o
	g++ -g -o P3.out Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o

Project2.o : Project2.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project2.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h
	g++ -g -c CodeGenerator.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c SyntacticalAnalyzer.cpp

clean :
	rm [SP]*.o P3.out *.gch

submit : Project2.cpp LexicalAnalyzer.h CodeGenerator.h SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile README.txt
	rm -rf Team21P3
	mkdir Team21P3
	cp Project2.cpp Team21P3
	cp SyntacticalAnalyzer.h Team21P3
	cp SyntacticalAnalyzer.cpp Team21P3
	cp CodeGenerator.h Team21P3
	cp CodeGenerator.cpp Team21P3
	cp makefile Team21P3
	cp README.txt Team21P3
	tar cfvz Team21P3.tgz Team21P3
	cp Team21P3.tgz ~tiawatts/cs460drop
