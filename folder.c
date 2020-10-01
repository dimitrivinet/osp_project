
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

static const char *dir_path = "/dir"; //directory path
static const char *shutdown_path = "/dir/shutdown"; //shutdown file path
static const char *suspend_path = "/dir/suspend"; //suspend file path

static int folder_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime = time( NULL );  

    if (strcmp(path, "/") == 0 || strcmp(path, dir_path) == 0) //if the target is the root directory of the fs
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, shutdown_path) == 0 || strcmp(path, suspend_path) == 0) //if the target is the shutdown or suspend file
    {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0;
    }
    else
        res = -ENOENT;

    return res;
}

static int folder_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0); //basic buffer

    if (strcmp(path, "/") == 0) //if the target is not the root dir of the fs
    {
        filler(buf, dir_path + 1, NULL, 0); //only show the parent directory
    }
    else if (strcmp(path, "/dir") == 0) //if the target is the parent directory
    {
        int len = strlen(dir_path) + 1;
        filler(buf, shutdown_path + len, NULL, 0);
        filler(buf, suspend_path + len, NULL, 0); //show child files
    }
    else
        return -ENOENT;

    return 0;
}

static int folder_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, dir_path) != 0 && strcmp(path, suspend_path) != 0 && strcmp(path, shutdown_path) != 0)
    { //return ENOENT if the target doesn't exist
        return -ENOENT;
    }

    return 0;
}

static int folder_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi)
{
    (void)fi;
    if (strcmp(path, shutdown_path) != 0 && strcmp(path, suspend_path) != 0) //if the target is not a file
        return -ENOENT;

    return 0; //nothing to read since nothind is written
}

static int folder_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    int res = 0;

    if (strcmp(suspend_path, path) == 0) //if the target is the suspend file
    {
        system("systemctl suspend");
        res = size;
    }
    else if (strcmp(shutdown_path, path) == 0) //if the target is the shutdown path
    {
        system("shutdown -P now");
        res = size;
    }
    else
    {
        res = -ENOENT;
    }

    return res;
}

static struct fuse_operations folder_oper = { //all fuse operations for this fs
    .getattr = folder_getattr,
    .readdir = folder_readdir,
    .open = folder_open,
    .read = folder_read,
    .write = folder_write,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &folder_oper, NULL);
}
