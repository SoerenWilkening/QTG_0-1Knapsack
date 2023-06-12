compile:
	@echo "compile c files"
	@clang -c header/syslinks.c header/knapsack.c header/simulate.c header/stategen.c header/combowrp.c header/combo.c
	@echo "create main executable"
	@clang 			main.c \
					syslinks.o \
					knapsack.o \
					simulate.o \
					stategen.o \
					combowrp.o \
					combo.o -o main -lgsl -lgslcblas -lgmp
	@echo "delete non used files"
	@rm simulate.o stategen.o combowrp.o combo.o knapsack.o syslinks.o