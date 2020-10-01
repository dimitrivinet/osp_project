#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static const char *update_path = "/update"; //file path

static int update_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime = time( NULL );  

	if (strcmp(path, "/") == 0) //if the target is the root directory of the fs
	{
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, update_path) == 0) //if the target is the update file
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 0;
	}
	else
		res = -ENOENT;

	return res;
}

static int update_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
						  off_t offset, struct fuse_file_info *fi)
{
	(void)offset;
	(void)fi;

	if (strcmp(path, "/") != 0) //if the target is not the root dir of the fs
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, update_path + 1, NULL, 0); //fill the buffer for the readdir result

	return 0;
}

static int update_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, update_path) != 0) //return ENOENT if the target doesn't exist
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY) //this file is readonly
		return -EACCES;

	return 0;
}

static int update_read(const char *path, char *buf, size_t size, off_t offset,
					   struct fuse_file_info *fi)
{
	(void)fi;
	if (strcmp(path, update_path) != 0) //if the target is not the update file
		return -ENOENT;

	char buffer[1024]; //initialize buffers
	FILE *fp = popen("sudo apt update", "r"); //create stream with command result

	while (fgets(buffer, 1024, fp) != NULL) //copy each line from the stream into the buffer and print it to the console
	{
		printf("%s", buffer);
	}

	pclose(fp); //close the file

	return 0;
}

static struct fuse_operations update_oper = { //all fuse operations for this fs
	.getattr = update_getattr,
	.readdir = update_readdir,
	.open = update_open,
	.read = update_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &update_oper, NULL);
}