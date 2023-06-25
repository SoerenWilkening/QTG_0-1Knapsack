GSLPATH = 
GMPPATH = 

ifeq ($(OS),Windows_NT)
	GSLFLAG = -I$(GSLPATH)\\include -L$(GSLPATH)\\lib
	GMPFLAG = -I$(GMPPATH)\\include -L$(GMPPATH)\\lib
	RM = del
	PATH_SEP = \\

else
	GSLFLAG =
	GMPFLAG =
	RM = rm
	PATH_SEP = /

endif

compile:
	@echo "creating .o files"
	@gcc		-c							\
				src$(PATH_SEP)syslinks.c	\
				src$(PATH_SEP)knapsack.c	\
				src$(PATH_SEP)simulate.c	\
				src$(PATH_SEP)stategen.c	\
				src$(PATH_SEP)combowrp.c	\
				src$(PATH_SEP)combo.c		\
				src$(PATH_SEP)qtgcount.c
	@echo "creating cmbcount executable"
	@gcc		cmbcount.c	\
				syslinks.o	\
				knapsack.o	\
				combowrp.o	\
				combo.o		\
				$(GSLFLAG)	\
				$(GMPFLAG)	\
				-lgsl		\
				-lgslcblas	\
				-lgmp		\
				-O1			\
				-g			\
				-o cmbcount
	@echo "creating main executable"
	@gcc		main.c		\
				syslinks.o	\
				knapsack.o	\
				simulate.o	\
				stategen.o	\
				combowrp.o	\
				combo.o		\
				qtgcount.o	\
				$(GSLFLAG)	\
				$(GMPFLAG)	\
				-lgsl		\
				-lgslcblas	\
				-lgmp		\
				-O1			\
				-g			\
				-o main

	@echo "deleting non used files"
	@$(RM) qtgcount.o simulate.o stategen.o combowrp.o combo.o knapsack.o syslinks.o