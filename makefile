compile:
	@echo "compile c files"
	@clang -c	header/syslinks.c \
				header/knapsack.c \
				header/simulate.c \
				header/stategen.c \
				header/combowrp.c \
				header/combo.c 	  \
				header/qtgcount.c
	@echo "create main executable"
	@clang  		main.c \
					syslinks.o \
					knapsack.o \
					simulate.o \
					stategen.o \
					combowrp.o \
					combo.o    \
					qtgcount.o \
					-o main    \
					-lgsl      \
					-lgslcblas \
					-lgmp      \
					-framework CoreFoundation \
					-framework IOKit \
					-O1 \
					-g 

	@echo "delete non used files"
	@rm qtgcount.o simulate.o stategen.o combowrp.o combo.o knapsack.o syslinks.o