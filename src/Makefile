IDIR=../include
ODIR=obj
CDIR=backend

CC=gcc
CFLAGS = -Wall -ansi -g -Ofast `pkg-config --cflags glib-2.0` -I$(IDIR)

DEPS=$(IDIR)/$(wildcard *.h)
SOURCES=$(CDIR)/$(wildcard *.c)
SOURCES_OBJ=$(patsubst %.c,$(ODIR)/%.o,$(SOURCES))

LIBS=`pkg-config --libs glib-2.0`

print-% : ; @echo $* = $($*)

$(ODIR)/%.o : $(CDIR)/%.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

program: $(SOURCES_OBJ)
	$(foreach dir,$(wildcard *.l),$(shell flex $(dir)))
	mv lex.*.c $(CDIR) 2> /dev/null
	$(CC) $(CFLAGS) $(wildcard $(ODIR)/*.o) -o program $(LIBS)
	rm -rf $(CDIR)/lex.*.c

clean:
	rm -rf $(ODIR)/*.o
	rm -rf program