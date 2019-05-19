CFLAGS= -std=c99
INCDIR= ./Inc
SRCDIR= ./Src
OBJDIR= ./Obj
OBJS= $(OBJDIR)/main.o $(OBJDIR)/display.o $(OBJDIR)/board.o $(OBJDIR)/input_methods.o $(OBJDIR)/moves.o

all: init main_chess incver

main_chess: $(OBJS)
	@gcc $(OBJS) -o main_chess

$(OBJDIR)/main.o: $(INCDIR)/display.h $(SRCDIR)/main.c
	@gcc $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/main.c -o $@

$(OBJDIR)/input_methods.o: $(INCDIR)/input_methods.h $(SRCDIR)/input_methods.c
	@gcc $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/input_methods.c -o $@

$(OBJDIR)/moves.o: $(INCDIR)/moves.h $(SRCDIR)/moves.c
	@gcc $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/moves.c -o $@

$(OBJDIR)/display.o: $(INCDIR)/display.h $(SRCDIR)/display.c
	@gcc $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/display.c -o $@

$(OBJDIR)/board.o: $(INCDIR)/board.h $(SRCDIR)/board.c
	@gcc $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/board.c -o $@

init:
	@mkdir -p Obj

incver:
	@Version/incver.sh

rebuild_all: clean all

clean:
	@rm -f $(OBJS) main_chess
	
