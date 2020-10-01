#define FUSE_USE_VERSION 26

#include <unistd.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <err.h>

char file_content[256]; //file content
static const char *write_path = "/write"; //file paths

char *wd; //variable to get file directory path

static int write_getattr(const char *path, struct stat *stbuf)
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
    else if (strcmp(path, write_path) == 0) //if the target is the write file
    {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = 1024;
    }
    else
        res = -ENOENT;

    return res;
}

static int write_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    if (strcmp(path, "/") != 0) //if the target is not the root dir of the fs
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, write_path + 1, NULL, 0); //fill the buffer for the readdir result

    return 0;
}

static int write_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, write_path) != 0) //return ENOENT if the target doesn't exist
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
        memcpy(buffer, file_content + offset, size); //write the content of the file to memory, fuse will output it to the console
    }
    else
        size = 0;

    return size;
}

static int write_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info)
{
    int res = 0;
    if (strcmp(write_path, path) != 0) //if target file doesn't exist
    {
        res = -ENOENT;
    }
    else
    {
        strncpy(file_content, buffer, 256); //write to virtual file        
        // printf("%s", wd);

        FILE *fp2 = fopen(wd, "w"); //write to disk file

        fprintf(fp2, buffer);

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

static struct fuse_operations write_oper = { //all fuse operations for this fs
    .getattr = write_getattr,
    .readdir = write_readdir,
    .open = write_open,
    .read = write_read,
    .write = write_write,
    .truncate = write_truncate,
};

int main(int argc, char *argv[])
{
    char *popen_buf = malloc(1024); //variable creation and memory allocation
    wd = malloc(1024);
    FILE *fp1 = popen("pwd", "r"); //get current working directory
    fgets(popen_buf, 1024, fp1);
    strcpy(wd, popen_buf); 
    wd[strcspn(wd, "\n")] = 0; //copy command result into str and remove \n
    
    pclose(fp1);
    free(popen_buf);

    char *disk_file = "/write_disk";
    strcat(wd, disk_file); //get a path to a file on disk which will be created/modified when virtual write is written to

    return fuse_main(argc, argv, &write_oper, NULL);
}
