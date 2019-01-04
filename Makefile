.PHONY: test example tags

lambda: parser.tab.c lexer.yy.c lambda.h lambda.c binding.h binding.c main.c
	gcc -std=c99 -g -Iinclude -Llib -DDMALLOC \
		lexer.yy.c parser.tab.c lambda.c binding.c main.c -ldmalloc -o $@

lexer.yy.c: lexer.l
	flex -o $@ $<

parser.tab.c: parser.y
	bison -t -r all -d -Weverything $< -o $@

tags:
	ctags *.[lyhc]

edit:
	vim -c 'vsplit|b2' lexer.l parser.y

test: lambda
	rm -f test/*.txt; \
	for LAM in test/*.lam; do \
		cat "$$LAM" | ./lambda -t 2> test/`basename $$LAM .lam`.txt; \
	done

example:
	rm -f example/*.txt; \
	for LAM in example/*.lam; do \
		cat "$$LAM" | ./lambda 2>/dev/null 1>example/`basename $$LAM .lam`.txt; \
	done

clean:
	rm -f *.tab.[hc] *.yy.[hc] *.o *~ lambda *.exe *.stackdump tags parser.output
