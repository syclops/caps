PAPER = paper
LATEXMK = latexmk
FLAGS = -pdf -bibtex
CLEAN_FLAGS = -c -bibtex
CLEAN_FULL_FLAGS = -C -bibtex

.PHONY = all paper clean clean-full

all: paper

paper:
	$(LATEXMK) $(FLAGS) $(PAPER)

clean:
	$(LATEXMK) $(CLEAN_FLAGS) $(PAPER)

clean-full:
	$(LATEXMK) $(CLEAN_FULL_FLAGS) $(PAPER)
