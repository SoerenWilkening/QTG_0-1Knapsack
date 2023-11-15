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
				src$(PATH_SEP)expknapwrp.c	\
                src$(PATH_SEP)expknap.c		\
				src$(PATH_SEP)qtgcount.c
	@echo "creating cmbcount executable"
	@gcc		cmbcount.c	\
				syslinks.o	\
				knapsack.o	\
				combowrp.o	\
				combo.o		\
				expknapwrp.o	\
				expknap.o		\
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
				expknapwrp.o	\
				expknap.o		\
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
	@$(RM) qtgcount.o simulate.o stategen.o combowrp.o combo.o expknapwrp.o expknap.o knapsack.o syslinks.o

generate:
	$(eval n := $(shell cat generator_input.txt | head -n 1))
	$(eval Z := $(shell cat generator_input.txt | sed -n '2p'))
	$(eval g := $(shell cat generator_input.txt | sed -n '3p'))
	$(eval f := $(shell cat generator_input.txt | sed -n '4p'))
	$(eval eps := $(shell cat generator_input.txt | sed -n '5p'))
	$(eval s := $(shell cat generator_input.txt | tail -n 1))
	@echo "Check if instance already exists"

	$(eval file := instances/problemInstances/n_)
	$(eval file := $(addprefix $(file), $(n)))
	$(eval file := $(addprefix $(file), _c_))
	$(eval file := $(addprefix $(file), $(Z)))
	$(eval file := $(addprefix $(file), _g_))
	$(eval file := $(addprefix $(file), $(g)))
	$(eval file := $(addprefix $(file), _f_))
	$(eval file := $(addprefix $(file), $(f)))
	$(eval file := $(addprefix $(file), _eps_))
	$(eval file := $(addprefix $(file), $(eps)))
	$(eval file := $(addprefix $(file), _s_))
	$(eval file := $(addprefix $(file), $(s)))

	@if [ -f $(addprefix $(file), /test.in) ]; then \
		echo "instance already exists"; \
    else \
    	echo "instance will be generated"; \
		g++ -g -std=c++11 -O2 src/generator.cpp -o generatorExecutable; \
		mkdir -p $(file); \
		touch $(addprefix $(file), /test.in); \
		./generatorExecutable < generator_input.txt > $(addprefix $(file), /test.in); \
    fi


