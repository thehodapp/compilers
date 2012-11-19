all: src/parser

clean:
	rm -f src/*.pyc
	rm -f src/parser src/parser.c
	rm -f src/parser
	rm -f doc/grammar/*.tex
	if [ -e doc/grammar -a ! "$(ls -A doc/grammar )" ]; then rmdir doc/grammar; fi
	rm -f doc/report*.aux doc/report*.toc doc/report*.log doc/report*.pdf doc/report*.dvi doc/report*.out
	rm -f tests/*.tree tests/*.lst

p1:
	cd p1
	git pull origin master > /dev/null 2>&1
	cd ..

src/parser: src/parser.c
	gcc -Wall -g -std=c99 src/parser.c p1/src/machines.c p1/src/types.c -o src/parser
	chmod +x src/parser

src/parser.c: src/parser.c.py src/firstfollow.py src/massage.py src/rules.py src/table.py
	python src/parser.c.py > src/parser.c

doc: doc/report.pdf doc/report2.pdf

doc/report.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst src/parser.c src/massage.py src/firstfollow.py src/table.py
	cd doc; pdflatex report.tex; cd ..;
	cd doc; pdflatex report.tex; cd ..;

doc/report2.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst src/parser.c src/massage.py src/firstfollow.py src/table.py
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;

doc/grammar/initial.tex: src/grammarToLatex.py doc/grammar
	python src/grammarToLatex.py --initial doc/grammar/initial.tex
doc/grammar/recursion.tex: src/grammarToLatex.py doc/grammar
	python src/grammarToLatex.py --recursion doc/grammar/recursion.tex
doc/grammar/factoring.tex: src/grammarToLatex.py doc/grammar
	python src/grammarToLatex.py --factoring doc/grammar/factoring.tex
doc/grammar:
	mkdir -p doc/grammar

tests/minimal.tree: tests/minimal.pas src/parser
	src/parser tests/minimal.pas
tests/blank.tree: tests/blank.pas src/parser
	! src/parser tests/blank.pas
tests/minimal.lst: tests/minimal.pas src/parser
	src/parser tests/minimal.pas
tests/blank.lst: tests/blank.pas src/parser
	! src/parser tests/blank.pas
