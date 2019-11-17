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

clean:
	rm -f *.tab.[hc] *.yy.[hc] *.o *~ lambda *.stackdump tags parser.output
