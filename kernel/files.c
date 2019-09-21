#include "files.h"
#include "flash.h"
#include "string.h"

#define FS_NAME_OFFSET	0
#define FS_SIZE_OFFSET	16
#define FS_DATA_OFFSET	(PAGE_LEN)


static char page[PAGE_LEN];



int format(char *name){
	int i;
	// create header file
	memset(page, 0, PAGE_LEN);
	strncpy(page, name, FS_MAXNAME);
	flash_writePage(0*PAGE_LEN, page);
	// clean all other files
	memset(page, 0, PAGE_LEN);
	for(i=1;i<FS_MAXFILES;++i){
		flash_writePage(i*FS_MAXSIZE, page);
	}
	return 0;
}

int remove(char *name){
	int i;
	for(i=1;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(strcmp(page, name)==0){
			page[0]=0;
			flash_writePage(i*FS_MAXSIZE, page);
			return 0;
		}
	}
	return -1;
}

int exist(char *name){
	int i;
	for(i=0;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(strcmp(page, name)==0){
			return *((int*)(page+FS_SIZE_OFFSET));
		}
	}
	return -1;
}

int rename(char *n1, char *n2){
	int i;
	for(i=1;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(strcmp(page, n1)==0){
			strncpy(page, n2, FS_MAXNAME);
			flash_writePage(i*FS_MAXSIZE, page);
			return 0;
		}
	}
	return -1;
}

int list(int *next, char *buff){
	int i=*next;
	int size;
	buff[0]=0;

	while(i<FS_MAXFILES){
		flash_readPage(i*FS_MAXSIZE, page);
		size=*((int*)(page+FS_SIZE_OFFSET));
		++i;
		if(page[0]){
			strcpy(buff, page);
			*next=i;
			return size;
		}
	}
	return -1;
}

int load(char *name, char *buff){
	int i, addr, size;
	for(i=0;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(strcmp(page, name)==0){
			addr=i*FS_MAXSIZE+FS_DATA_OFFSET;
			size=*((int*)(page+FS_SIZE_OFFSET));

			for(i=0;i<size;i=i+PAGE_LEN){
				flash_readPage(addr+i, buff+i);
			}
			return size;
		}
	}
	return -1;
}

int save(char *name, char *buff, int n){
	int i, addr;
	if(n>(FS_MAXSIZE-PAGE_LEN)){
		return -1;
	}

	// find if file exist
	for(i=0;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(strcmp(name, page)==0){
			// write header
			*((int*)(page+FS_SIZE_OFFSET))=n;
			flash_writePage(i*FS_MAXSIZE, page);
			addr=i*FS_MAXSIZE+FS_DATA_OFFSET;
			// write date
			for(i=0;i<n;i=i+PAGE_LEN){
				flash_writePage(addr+i, buff+i);
			}
			return 0;
		}
	}

	// find free space
	for(i=0;i<FS_MAXFILES;++i){
		flash_readPage(i*FS_MAXSIZE, page);
		if(!page[0]){
			// write header
			addr=i*FS_MAXSIZE;
			memset(page, 0, PAGE_LEN);
			strncpy(page, name, FS_MAXNAME);
			*((int*)(page+FS_SIZE_OFFSET))=n;
			flash_writePage(addr, page);
			addr += FS_DATA_OFFSET;
			// write date
			for(i=0;i<n;i=i+PAGE_LEN){
				flash_writePage(addr+i, buff+i);
			}
			return 0;
		}
	}
	return -2;
}


