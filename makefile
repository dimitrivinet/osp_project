%: test hello shutdown write

%: 
	gcc -Wall $@.c `pkg-config fuse --cflags --libs` -o $@_e
	-fusermount -u $@_d
	-rm -rf $@_d
	mkdir $@_d
	./$@_e $@_d

to_console:
	gcc -Wall $@.c `pkg-config fuse --cflags --libs` -o $@_e
	-fusermount -u $@_d
	-rm -rf $@_d
	mkdir $@_d
	./$@_e $@_d -f

update:
	gcc -Wall update.c `pkg-config fuse --cflags --libs` -o update_e
	-sudo fusermount -u update_d
	-rm -rf update_d
	mkdir update_d
	sudo ./update_e update_d -o allow_other -f

clean_test: 
	-fusermount -u test_d
	rm -rf test_d
	rm -f test_e

clean_hello: 
	-fusermount -u hello_d
	rm -rf hello_d
	rm -f hello_e

clean_to_console: 
	-fusermount -u to_console_d
	rm -rf to_console_d
	rm -f to_console_e

clean_shutdown: 
	-fusermount -u shutdown_d
	rm -rf shutdown_d
	rm -f shutdown_e

clean_write:
	-fusermount -u write_d
	rm -rf write_d
	rm -f write_e

clean_folder:
	-fusermount -u folder_d
	rm -rf folder_d
	rm -f folder_e

clean_update: 
	-sudo fusermount -u update_d
	rm -rf update_d
	rm -f update_e