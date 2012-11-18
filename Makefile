all: parser

clean:
	rm -f parser parser.c
	rm -f parser
	rm -f doc/grammar/*.tex
	rm -f doc/report*.aux doc/report*.toc doc/report*.log doc/report*.pdf doc/report*.dvi doc/report*.out
	rm -f tests/*.tree tests/*.lst

p1:
	cd p1
	git pull origin master > /dev/null 2>&1
	cd ..

parser: parser.c
	gcc -Wall -g -std=c99 parser.c p1/src/machines.c p1/src/types.c -o parser
	chmod +x parser

parser.c: parser.c.py firstfollow.py massage.py rules.py
	python parser.c.py > parser.c

doc: doc/report.pdf doc/report2.pdf

doc/report.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst parser.c massage.py firstfollow.py table.py
	cd doc; pdflatex report.tex; cd ..;
	cd doc; pdflatex report.tex; cd ..;

doc/report2.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst parser.c massage.py firstfollow.py table.py
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;

doc/grammar/initial.tex: grammarToLatex.py
	python grammarToLatex.py --initial doc/grammar/initial.tex
doc/grammar/recursion.tex: grammarToLatex.py
	python grammarToLatex.py --recursion doc/grammar/recursion.tex
doc/grammar/factoring.tex: grammarToLatex.py
	python grammarToLatex.py --factoring doc/grammar/factoring.tex

tests/minimal.tree: tests/minimal.pas ./parser
	./parser tests/minimal.pas
tests/blank.tree: tests/blank.pas ./parser
	! ./parser tests/blank.pas
tests/minimal.lst: tests/minimal.pas ./parser
	./parser tests/minimal.pas
tests/blank.lst: tests/blank.pas ./parser
	! ./parser tests/blank.pas
