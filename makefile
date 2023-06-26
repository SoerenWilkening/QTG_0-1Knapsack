compile:
	@echo "compile c files"
	@clang -c	header/syslinks.c \
				header/knapsack.c \
				header/simulate.c \
				header/stategen.c \
				header/combowrp.c \
				header/combo.c 	  \
				header/qtgcount.c
	@echo "create cmbcount executable"
	@clang  		cmbcount.c \
					syslinks.o \
					knapsack.o \
					combowrp.o \
					combo.o    \
					-o cmbcount\
					-lgsl      \
					-lgslcblas \
					-lgmp      \
					-framework CoreFoundation \
					-framework IOKit \
					-O1 \
					-g
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
		g++ -g -std=c++11 -O2 instance_generator/generator.cpp -o generatorExecutable; \
		mkdir -p $(file); \
		touch $(addprefix $(file), /test.in); \
		./generatorExecutable < generator_input.txt > $(addprefix $(file), /test.in); \
    fi
