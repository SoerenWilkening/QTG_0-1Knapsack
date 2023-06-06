gain: gain.o combo_wrap.o knapsack.o combo.o
	clang++ -Wall -g -o gain combo.o gain.o combo_wrap.o knapsack.o

combo.o: combo.c
	clang -Wall -g -c combo.c

gain.o: gain.cpp
	clang++ -Wall -g -c gain.cpp

combo_wrap.o: combo_wrap.cpp
	clang++ -Wall -g -c combo_wrap.cpp

knapsack.o: knapsack.c
	clang -Wall -g -c knapsack.c