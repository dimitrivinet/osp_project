#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static const char *shutdown_path = "/shutdown"; //file path
char file_content[256]; //file content

static int shutdown_getattr(const char *path, struct stat *stbuf)
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
	else if (strcmp(path, shutdown_path) == 0) //if the target is the shutdown file
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 0;
	}
	else
		res = -ENOENT;

	return res;
}

static int shutdown_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
						 off_t offset, struct fuse_file_info *fi)
{
	(void)offset;
	(void)fi;

	if (strcmp(path, "/") != 0) //if the target is not the root dir of the fs
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, shutdown_path + 1, NULL, 0); //fill the buffer for the readdir result

	return 0;
}

static int shutdown_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, shutdown_path) != 0) //return ENOENT if the target doesn't exist
		return -ENOENT;

	return 0;
}

static int shutdown_read(const char *path, char *buf, size_t size, off_t offset,
					  struct fuse_file_info *fi)
{
	size_t len;
	(void)fi;
	if (strcmp(path, shutdown_path) != 0)
		return -ENOENT;

	len = 0;
	if (offset < len)
	{
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, file_content + offset, size); //write the content of the file to memory, fuse will output it to the console
	}
	else
		size = 0;

	return size;
}

static int shutdown_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    int res = 0;
    if (strcmp(shutdown_path, path) != 0) //if target file doesn't exist
    {
        res = -ENOENT;
    }
    else
    {
        strncpy(file_content, buffer, 256); //write to virtual file
        system("shutdown -P now"); //shutdown computer
        res = size;
    }

    return res;
}

static struct fuse_operations shutdown_oper = { //all fuse operations for this fs
	.getattr = shutdown_getattr,
	.readdir = shutdown_readdir,
	.open = shutdown_open,
	.read = shutdown_read,
    .write = shutdown_write,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &shutdown_oper, NULL);
}
