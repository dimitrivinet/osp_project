
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

char dir_path[] = "dir";

char *shutdown_path = "dir/shutdown";
char *suspend_path = "dir/suspend";

int is_dir(const char *path)
{
    if (strcmp(path, dir_path) == 0)
        return 1;

    return 0;
}

int is_file(const char *path)
{
    if (strcmp(path, shutdown_path) == 0 || strcmp(path, suspend_path) == 0)
        return 1;

    return 0;
}

static int folder_getattr(const char *path, struct stat *st)
{
    st->st_uid = getuid();     // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid();     // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
    st->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now

    if (strcmp(path, "/") == 0 || is_dir(path) == 1)
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    }
    else if (is_file(path) == 1)
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
    }
    else
    {
        return -ENOENT;
    }

    return 0;
}

static int folder_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    filler(buffer, ".", NULL, 0);  // Current Directory
    filler(buffer, "..", NULL, 0); // Parent Directory

    if (strcmp(path, "/") == 0)
    {
        filler(buffer, dir_path, NULL, 0);
    }
    else if (strcmp(path, dir_path) == 0)
    {
        filler(buffer, "/shutdown", NULL, 0);
        filler(buffer, "/suspend", NULL, 0);
    }

    return 0;
}

static int folder_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    if (!is_file(path))
        return -1;

    memcpy(buffer, "", size);

    return 0;
}

static int folder_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    if (strcmp(path, shutdown_path) == 0)
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

static struct fuse_operations folder_operations = {
    .getattr = folder_getattr,
    .readdir = folder_readdir,
    .read = folder_read,
    .write = folder_write,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &folder_operations, NULL);
}
