all: src/parser

clean:
	rm -f src/*.pyc
	rm -f src/parser src/gen.c
	rm -f src/parser
	rm -f doc/grammar/*.tex
	if [ -e doc/grammar -a ! "$(ls -A doc/grammar )" ]; then rmdir doc/grammar; fi
	rm -f doc/report*.aux doc/report*.toc doc/report*.log doc/report*.pdf doc/report*.dvi doc/report*.out
	rm -f tests/*.tree tests/*.lst tests/*.tbl

src/parser: src/parser.c src/machines.c src/types.c src/symtab.c src/item.c
	gcc -Wall -Wextra -g -std=c99 src/parser.c src/machines.c src/types.c src/symtab.c src/item.c -o src/parser
	chmod +x src/parser

src/gen.c: src/parser.c.py src/firstfollow.py src/massage.py src/rules.py src/table.py
	python src/parser.c.py > src/gen.c

doc: doc/report.pdf doc/report2.pdf doc/report3.pdf

doc/report.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst src/parser.c src/massage.py src/firstfollow.py src/table.py
	cd doc; pdflatex report.tex; cd ..;
	cd doc; pdflatex report.tex; cd ..;

doc/report2.pdf: doc/report.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex tests/minimal.pas tests/minimal.tree tests/blank.pas tests/blank.lst src/parser.c src/massage.py src/firstfollow.py src/table.py
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;
	cd doc; cp report.tex report2.tex; pdflatex "\def\includePython{}\input{report2.tex}"; rm report2.tex; cd ..;

doc/report3.pdf: doc/report3.tex doc/grammar/initial.tex doc/grammar/recursion.tex doc/grammar/factoring.tex src/parser.c tests/shenoi.pas tests/shenoi.tbl tests/minimal.pas tests/minimal.tree
	cd doc; pdflatex report3.tex; cd ..;
	cd doc; pdflatex report3.tex; cd ..;

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
	! src/parser tests/blank.pas 2> /dev/null
tests/shenoi.tree: tests/shenoi.pas src/parser
	src/parser tests/shenoi.pas
tests/shenoi.tbl: tests/shenoi.pas src/parser
	src/parser tests/shenoi.pas
tests/minimal.lst: tests/minimal.pas src/parser
	src/parser tests/minimal.pas
tests/blank.lst: tests/blank.pas src/parser
	! src/parser tests/blank.pas 2> /dev/null
tests/shenoi.lst: tests/shenoi.pas src/parser
	src/parser tests/shenoi.pas
tests: tests/minimal.tree tests/blank.tree tests/shenoi.tree tests/minimal.lst tests/blank.lst tests/shenoi.lst
