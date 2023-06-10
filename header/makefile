compile:
	@echo "compile c files"
	@clang -c stategen.c combowrp.c combo.c knapsack.c
	@echo "create main executable"
	@clang 			gain.c \
					stategen.o \
					combowrp.o \
					combo.o \
					knapsack.o -o gain -lgmp
	@echo "delete non used files"
	@rm stategen.o combowrp.o combo.o knapsack.o