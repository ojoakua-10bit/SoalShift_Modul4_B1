#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

static const char *dpath = "/home/tidakumum/Documents";

static int anu_getattr(const char *path, struct stat *stbuf) {
  int res;
	char filpth[1000];
	sprintf(filpth,"%s%s",dpath,path);
	res = lstat(filpth, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int anu_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
  char filpth[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dpath;
		sprintf(filpth,"%s",path);
	}
	else sprintf(filpth, "%s%s",dpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(filpth);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int anu_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char filpth[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dpath;
		sprintf(filpth,"%s",path);
	}
	else sprintf(filpth, "%s%s",dpath,path);
	int res = 0;
    int fd = 0 ;

	(void) fi;
	if (strcmp(filpth+(strlen(filpth)-4), ".pdf") == 0 || strcmp(filpth+(strlen(filpth)-4), ".txt") == 0 || strcmp(filpth+(strlen(filpth)-4), ".doc") == 0){
        fprintf(stderr, "Terjadi kesalahan! File berisi konten berbahaya.\n");
        char buf[1024];
        sprintf(buf,"%s/rahasia%s.ditandai", dpath, path);
        if(rename(filpth, buf) != 0){
            char buf2[1024];
            sprintf(buf2, "%s/rahasia", dpath);
            mkdir(buf2, 0775);
            rename(filpth, buf);
        }
        chmod(buf, 0);
    }
    else{
        fd = open(filpth, O_RDONLY);
        if (fd == -1)
            return -errno;

        res = pread(fd, buf, size, offset);
        if (res == -1)
            res = -errno;

        close(fd);
    }
	return res;
}

static struct fuse_operations anu_oper = {
	.getattr	= anu_getattr,
	.readdir	= anu_readdir,
	.read		= anu_read,
};

int main(int argc, char *argv[]) {
	umask(0);
	return fuse_main(argc, argv, &anu_oper, NULL);
}
