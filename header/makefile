compile:
	@echo "compile c files"
	@gcc -c combo.c knapsack.c
	@echo "create main executable"
	@g++ -std=c++17 gain.cpp \
					combo.o \
					combo_wrap.cpp \
					knapsack.o -o gain
	@echo "delete non used files"
	@rm combo.o knapsack.o