LATEXMK = latexmk
FLAGS = -pdf -bibtex-cond
CLEAN_FLAGS = -c -bibtex-cond
CLEAN_FULL_FLAGS = -C -bibtex-cond

.PHONY = all clean clean-full notes

all: notes

notes:
	$(LATEXMK) $(FLAGS) notes

clean:
	$(LATEXMK) $(CLEAN_FLAGS) $(PAPER)

clean-full:
	$(LATEXMK) $(CLEAN_FULL_FLAGS) $(PAPER)
