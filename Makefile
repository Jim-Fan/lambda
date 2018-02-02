.PHONY: example

lambda: parser.tab.c lexer.yy.c lambda.h lambda.c main.c
	gcc -std=c99 -g -o $@ lexer.yy.c parser.tab.c lambda.c main.c

lexer.yy.c: lexer.l
	flex -o $@ $<

parser.tab.c: parser.y
	bison -t -r all -d -Weverything $< -o $@

tags:
	ctags *.[lyhc]

edit:
	vim -c 'vsplit|b2' lexer.l parser.y

test: lambda
	rm -f tests/*.txt; \
	for LAM in tests/*.lam; do \
		cat "$$LAM" | ./lambda 2> tests/`basename $$LAM .lam`.txt; \
	done

example:
	rm -f example/*.txt; \
	for LAM in example/*.lam; do \
		cat "$$LAM" | ./lambda 2>/dev/null 1>example/`basename $$LAM .lam`.txt; \
	done

clean:
	rm -f *.tab.[hc] *.yy.[hc] *.o *~ lambda *.exe *.stackdump tags parser.output
