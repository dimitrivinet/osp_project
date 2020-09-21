#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static const char *update_path = "/update";

static int update_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	stbuf->st_uid = 0; // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

	if (strcmp(path, "/") == 0)
	{
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, update_path) == 0)
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

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, update_path + 1, NULL, 0);

	return 0;
}

static int update_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, update_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int update_read(const char *path, char *buf, size_t size, off_t offset,
					  struct fuse_file_info *fi)
{
	(void)fi;
	if (strcmp(path, update_path) != 0)
		return -ENOENT;

	char command[500];
    int pid = get_shell_pid();
    snprintf(command, sizeof(command), "sudo apt update >> /proc/%d/fd/0", pid);
    system(command);

	return 0;
}

static struct fuse_operations update_oper = {
	.getattr = update_getattr,
	.readdir = update_readdir,
	.open = update_open,
	.read = update_read,
};

int get_shell_pid()
{
    int pid;
    FILE *fp = popen("pgrep -t pts/0", "r");

    fscanf(fp, "%d", &pid);
    pclose(fp);
    return pid;
}

int main(int argc, char *argv[])
{
    
	return fuse_main(argc, argv, &update_oper, NULL);
}