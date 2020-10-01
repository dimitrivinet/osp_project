#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *hello_str = "Hello\n"; //file content
static const char *hello_path = "/hello"; //file path

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));

	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime = time( NULL );  

	if (strcmp(path, "/") == 0) //if the target is the root directory of the fs
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, hello_path) == 0) //if the target is the hello file
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	}
	else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
						 off_t offset, struct fuse_file_info *fi)
{
	(void)offset;
	(void)fi;

	if (strcmp(path, "/") != 0) //if the target is not the root dir of the fs
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0); //fill the buffer for the readdir result

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0) //return ENOENT if the target doesn't exist
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY) //this file is readonly, error when you try to write to it
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
					  struct fuse_file_info *fi)
{
	size_t len;
	(void)fi;
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len)
	{
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size); //write the content of the file to memory, fuse will output it to the console
	}
	else
		size = 0;

	return size;
}

static struct fuse_operations hello_oper = { //all fuse operations for this fs
	.getattr = hello_getattr,
	.readdir = hello_readdir,
	.open = hello_open,
	.read = hello_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
