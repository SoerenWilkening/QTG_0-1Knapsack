compile:
	@echo "creating .o files"
	@clang -c	src/syslinks.c 	\
				src/knapsack.c 	\
				src/simulate.c 	\
				src/stategen.c 	\
				src/combowrp.c 	\
				src/combo.c 	\
				src/qtgcount.c
	@echo "create cmbcount executable"
	@clang  		cmbcount.c 	\
					syslinks.o 	\
					knapsack.o 	\
					combowrp.o 	\
					combo.o    	\
					-o cmbcount	\
					-lgsl      	\
					-lgslcblas 	\
					-lgmp      	\
					-O1 		\
					-g
	@echo "create main executable"
	@clang  		main.c 		\
					syslinks.o 	\
					knapsack.o 	\
					simulate.o 	\
					stategen.o 	\
					combowrp.o 	\
					combo.o    	\
					qtgcount.o 	\
					-o main    	\
					-lgsl      	\
					-lgslcblas 	\
					-lgmp      	\
					-O1 		\
					-g

	@echo "delete non used files"
	@rm qtgcount.o simulate.o stategen.o combowrp.o combo.o knapsack.o syslinks.o