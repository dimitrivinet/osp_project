#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

char file_content[256];
static const char *to_console_path = "/to_console";

static int to_console_getattr(const char *path, struct stat *stbuf)
{
    // memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

    int res = 0;

    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, to_console_path) == 0)
    {
		stbuf->st_mode = S_IFREG | 0644;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1024;
	}
    else
        res = -ENOENT;

    return res;
}

static int to_console_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                              off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, to_console_path + 1, NULL, 0);

    return 0;
}

static int to_console_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, to_console_path) != 0)
        return -ENOENT;

    return 0;
}

static int to_console_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void)fi;
    if (strcmp(path, to_console_path) != 0)
        return -ENOENT;

    char *content = file_content;
    len = strlen(content);
    if (offset < len)
    {
        if (offset + size > len)
            size = len - offset;
        memcpy(buffer, file_content + offset, size);
    }
    else
        size = 0;

    return size;
}

static int to_console_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    int res = 0;
    if (strcmp(to_console_path, path) != 0)
    {
        res = -ENOENT;
    }
    else
    {
        strncpy(file_content, buffer, 256);
        printf(buffer);
        res = size;
    }

    return res;
}

static struct fuse_operations to_console_oper = {
    .getattr = to_console_getattr,
    .readdir = to_console_readdir,
    .open = to_console_open,
    .read = to_console_read,
    .write = to_console_write,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &to_console_oper, NULL);
}
