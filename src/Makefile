CC := g++
IDIR :=
LDIR :=
LIBS := -lm -lboost_program_options
CFLAGS := -g -O3 -march=native -Wall \
          $(addprefix -I,$(IDIR)) $(addprefix -L,$(LDIR))

.PHONY: all clean
.PRECIOUS: %.cpp %.o

all: test_graph_config.out opt-hc.out

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

%.out: %.o gg_util.o graph_config.o edge_swap.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *.out
