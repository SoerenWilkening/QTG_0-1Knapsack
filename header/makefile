compile:
	@echo "compile c files"
	@gcc -c stategen.c combowrp.c combo.c knapsack.c
	@echo "create main executable"
	@gcc 			gain.c \
					stategen.o \
					combo_wrap.o \
					combo.o \
					knapsack.o -o gain -lgmp
	@echo "delete non used files"
	@rm stategen.o combo_wrap.o combo.o knapsack.o