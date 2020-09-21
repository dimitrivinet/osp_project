
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

static const char *dir_path = "/dir";
static const char *shutdown_path = "/dir/shutdown";
static const char *suspend_path = "/dir/suspend";

static int folder_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0 || strcmp(path, dir_path) == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, shutdown_path) == 0 || strcmp(path, suspend_path) == 0)
    {
        stbuf->st_mode = S_IFREG | 0444;
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
    filler(buf, "..", NULL, 0);

    if (strcmp(path, "/") == 0)
    {
        filler(buf, dir_path + 1, NULL, 0);
    }
    else if (strcmp(path, "/dir") == 0)
    {
        int len = strlen(dir_path) + 1;
        filler(buf, shutdown_path + len, NULL, 0);
        filler(buf, suspend_path + len, NULL, 0);
    }
    else 
        return -ENOENT;

    return 0;
}

static int folder_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, dir_path) != 0 || strcmp(path, suspend_path) != 0 || strcmp(path, shutdown_path) != 0)
    {
        return -ENOENT;
    }

    return 0;
}

static int folder_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi)
{
    (void)fi;
    if (strcmp(path, dir_path + 1) == 0)
    {
        system("shutdown -P now");
    }
    else if (strcmp(path, suspend_path) == 0)
    {
        system("systemctl suspend");
    }
    else
    {
        return -ENOENT;
    }
    
    

    return 0;
}

static struct fuse_operations folder_oper = {
    .getattr = folder_getattr,
    .readdir = folder_readdir,
    .open = folder_open,
    .read = folder_read,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &folder_oper, NULL);
}
