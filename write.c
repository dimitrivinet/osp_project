#define FUSE_USE_VERSION 26

#include <unistd.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <err.h>

char file_content[256];
static const char *write_path = "/write";
// static const char *write_disk_path = "/write_disk";

static int write_getattr(const char *path, struct stat *stbuf)
{
    // memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid();     // The owner of the file/directory is the user who mounted the filesystem
    stbuf->st_gid = getgid();     // The group of the file/directory is the same as the group of the user who mounted the filesystem
    stbuf->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
    stbuf->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now

    int res = 0;

    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, write_path) == 0)
    {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = 1024;
    }
    // else if (strcmp(path, write_disk_path) == 0)
    // {
    //     stbuf->st_mode = S_IFREG | 0644;
    //     stbuf->st_nlink = 1;
    //     stbuf->st_size = 1024;
    // }
    else
        res = -ENOENT;

    return res;
}

static int write_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, write_path + 1, NULL, 0);
    // filler(buf, write_disk_path + 1, NULL, 0);

    return 0;
}

static int write_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, write_path) != 0)
        return -ENOENT;

    return 0;
}

static int write_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void)fi;
    if (strcmp(path, write_path) != 0)
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

static int write_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    int res = 0;
    if (strcmp(write_path, path) != 0)
    {
        res = -ENOENT;
    }
    else
    {
        strncpy(file_content, buffer, 256);


        char *popen_buf = malloc(1024);
        char *wd = malloc(1024);
        char *disk_file = "/write_disk";

        FILE *fp1 = popen("pwd", "r");
        fgets(popen_buf, 1024, fp1);
        strcpy(wd, popen_buf); 
        wd[strcspn(wd, "\n")] = 0;
        
	    pclose(fp1);
        free(popen_buf);

        strcat(wd, disk_file);
        printf("%s", wd);

        FILE *fp2 = fopen(wd, "w");
        // printf("%i", fd);
        // if (fd == -1)
        // {
        //     return -errno; 
        // }

        int a = fprintf(fp2, buffer);       
        if (a == -1)
        {
            return -errno;
        }

        fclose(fp2);
        res = size;
    }

    return res;
}

static int write_truncate(const char* path, off_t size)
{
    int res;    
    res = truncate(path, size);
    if (res == -1)
        return -errno;
    return 0;
}

static struct fuse_operations write_oper = {
    .getattr = write_getattr,
    .readdir = write_readdir,
    .open = write_open,
    .read = write_read,
    .write = write_write,
    .truncate = write_truncate,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &write_oper, NULL);
}
