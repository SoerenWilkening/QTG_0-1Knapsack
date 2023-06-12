compile:
	@echo "compile c files"
	@clang -c syslinks.c knapsack.c simulate.c stategen.c combowrp.c combo.c
	@echo "create main executable"
	@clang 			gain.c \
					syslinks.c \
					knapsack.o \
					simulate.o \
					stategen.o \
					combowrp.o \
					combo.o -o gain -lgsl -lgslcblas -lgmp
	@echo "delete non used files"
	@rm simulate.o stategen.o combowrp.o combo.o knapsack.o syslinks.o