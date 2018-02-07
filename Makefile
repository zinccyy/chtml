PROJECTNAME = chtml
CC = gcc
INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin
SRCFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(wildcard $(SRCDIR)/*.o)
LIBNAME = lib/lib$(PROJECTNAME).a
BINARY  = bin/$(PROJECTNAME)

bin: $(SRCFILES) main.c
	$(CC) -Wall -I $(INCDIR) $(SRCFILES) main.c -g -o $(BINARY)
	
obj: $(SRCFILES) main.c
	$(foreach var,$(SRCFILES), $(CC) -I $(INCDIR) -c $(var) -o obj/$(notdir $(basename .c $(var))).o;)
	
lib: $(OBJDIR)
	$(AR) rcs $(LIBNAME) $(OBJDIR)/*
	
clean:
	rm obj/*
