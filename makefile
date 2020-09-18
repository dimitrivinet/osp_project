%: hello to_console shutdown write

%: 
	gcc -Wall $@.c `pkg-config fuse --cflags --libs` -o $@_e
	-fusermount -u $@_d
	-rm -rf $@_d
	mkdir $@_d
	./$@_e $@_d

clean: 
	-fusermount -u *_d
	rm -rf *_d
	rm -f *_e