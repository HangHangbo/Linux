test:test.c libadd.so
	gcc $^ -o $@

libadd.so:add.c
	gcc add.c -shared -fPIC -o libadd.so
