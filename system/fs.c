#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

struct dirent *dirent_ptr;
struct filetable *filetable_ptr;
#define FIRST_DATA_BLOCK (2+NUM_INODE_BLOCKS)
static int next_free_data_block;
static int next_free_inode;



int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}


int fs_open(char *filename, int flags) {

//flag is the mode of the file which can be O_RDONLY,O_WRONLY,O_RDWR
	int i;
//	struct dirent *dirent_ptr;
//	struct filetable *filetable_ptr;

	if(flags>=0&&flags<=2)
	{
		//check if the file is existing or not
		for(i=0;i<DIRECTORY_SIZE;i++)
		{
			dirent_ptr=&fsd.root_dir.entry[i];
			if(strncmp(dirent_ptr->name,filename,FILENAMELEN)==0)
			{
				//implies file is existing;
				//therefore allocate an entry in file table
				break;
			}
	
		}
		if(i==DIRECTORY_SIZE)
		{
			printf("FILE NOT FOUND\n");
			return SYSERR;
		}
		//since we found file is exisiting check if there is any empty slot in the file table or not.We can open a file,only if there is an empty slot it the file table.
		for(i=0;i<NUM_FD;i++)
		{
			if(oft[next_open_fd].state==FSTATE_CLOSED)
			{
				//we found one empty slot
				break;
			}
			next_open_fd++;
			if(next_open_fd>=NUM_FD)
			{
				next_open_fd=0;
			}
		}
		if(oft[next_open_fd].state!=FSTATE_CLOSED)
		{
			printf("file exists but cannot open since max files are already opened");
			return SYSERR;
		}
		
		filetable_ptr=&oft[next_open_fd];

		filetable_ptr->state=FSTATE_OPEN;
		filetable_ptr->fileptr=0;
        	filetable_ptr->de=dirent_ptr;

		fs_get_inode_by_num(dev0,dirent_ptr->inode_num,&filetable_ptr->in);

//		memcpy(&filetable_ptr->in,&in,sizeof(struct inode));

//		memcpy(oft+fsd,&filetable,sizeof(struct filetable));
		return next_open_fd;

	}

 	 return SYSERR;
}

int fs_close(int fd) {

	filetable_ptr=&oft[fd];
	if(filetable_ptr->state==FSTATE_OPEN)
	{
		filetable_ptr->state=FSTATE_CLOSED;
		return OK;	
	}
	else
	{
		printf("file is not open\n");
		
	}
	return SYSERR;

}

int fs_create(char *filename, int mode) {

	int i;

	if(mode!=O_CREAT)
	{
		return SYSERR;
	}
	if(strlen(filename)>FILENAMELEN)
	{
		return SYSERR;
	}
	//iterate through the directory structure if the file already exists or not
	for(i=0;i<DIRECTORY_SIZE;i++)
	{
		dirent_ptr=&fsd.root_dir.entry[i];
		if(strncmp(dirent_ptr->name,filename,FILENAMELEN))
		{
			printf("file already exists");
			return SYSERR;
		}
	
	}
	 //increase the numofentries and assign the new dirent_ptr
	 dirent_ptr=&fsd.root_dir.entry[fsd.root_dir.numentries++];
	 strncpy(dirent_ptr->name,filename,FILENAMELEN-1);
	 dirent_ptr->name[FILENAMELEN-1]='\0';
	 dirent_ptr->inode_num=-1;
	 
	 //create an entry in file table
	 for(i=0;i<NUM_FD;i++)
	 {
		if(oft[next_open_fd].state==FSTATE_CLOSED)
		{
			break;
		}
		next_open_fd++;
		if(next_open_fd>=NUM_FD)
		{
			next_open_fd=0;
		}
	 }
	 if(oft[next_open_fd].state!=FSTATE_CLOSED)
	 {
		printf("sorry max files opened already");
		return SYSERR;
	 }
	 filetable_ptr=&oft[next_open_fd];
	 filetable_ptr->state=FSTATE_OPEN;
	 filetable_ptr->fileptr=0;
	 filetable_ptr->de=dirent_ptr;

	//search for empty inode
	for(i=0;i<fsd.ninodes;i++)
	{
		fs_get_inode_by_num(0, next_free_inode, &filetable_ptr->in);
		if(filetable_ptr->in.id < 0 || filetable_ptr->in.id >= fsd.ninodes) {
			break;
		}
		next_free_inode++;
		if(next_free_inode>=fsd.ninodes)
		{
			next_free_inode=0;
		}
	}

	if(!(filetable_ptr->in.id < 0 || filetable_ptr->in.id >= fsd.ninodes)) {
		printf("max files limit reached\n");
		return SYSERR;			
	}

	filetable_ptr->in.id = next_free_inode;
	filetable_ptr->in.type = INODE_TYPE_FILE;
	filetable_ptr->in.nlink = 0;
	filetable_ptr->in.device = 0;
	filetable_ptr->in.size = 0;

	dirent_ptr->inode_num = next_free_inode;

	fsd.inodes_used++;
	/* write inode to disk */	
	fs_put_inode_by_num(dev0, filetable_ptr->in.id, &filetable_ptr->in);

	printf("\nFile %s created and opened successfully.\n",dirent_ptr->name);
	return next_open_fd;


}

int fs_seek(int fd, int offset) {

	int maxlimit;
	filetable_ptr=&oft[fd];
	//check in the filetable, if the file is open or not
	if(filetable_ptr->state==FSTATE_OPEN)
	{

		offset=offset+filetable_ptr->fileptr;
	
		//max file length;
		maxlimit=MDEV_BLOCK_SIZE*INODEDIRECTBLOCKS;
	
		if(offset<0)
		{
			return SYSERR;
		}  
	
		//check if we are trying to read the block greater than the size of the file
		if( (offset >= filetable_ptr->in.size) || (offset>=maxlimit))
		{
			return SYSERR;
		}
		filetable_ptr=offset;
		return filetable_ptr;
	}
	return SYSERR;
}
/*
int fs_read(int fd, void *buf, int nbytes) {

	filetable_ptr=&oft[fd];
	
	int maxlength;
	int offset,block;
	int currentblocknumber,currentoffsetnumber,length;
	int transferred_bytes;
	
	maxlength=MDEV_BLOCK_SIZE*INODEDIRECTBLOCKS;
	offset=nbytes+filetable_ptr->fileptr;

	
	if(nbytes<=0)
	{
		return SYSERR;
	}
	//reading will happen from the place where file descriptor is present. therefore check if reading nbytes from where filepointer is, will exceed the file size. If exceeds, make it to read only till the end.
	if(offset>filetable_ptr->in.size)
	{
		nbytes=filetable_ptr->in.size - filetable_ptr->fileptr;
	}
	//if reading nbytes exceeds the maxfilelength
	if(offset>=maxlength)
	{
		return SYSERR;
	}

	//before we try to read, check if the file is open
	if(filetable_ptr->state==FSTATE_OPEN)
	{
		//find the block and the offset
		currentblocknumber=filetable_ptr->in.blocks[filetable_ptr->fileptr/MDEV_BLOCK_SIZE];
//		currentblocknumber=filetable_ptr->fileptr/MDEV_BLOCK_SIZE; 
		currentoffsetnumber=filetable_ptr->fileptr%MDEV_BLOCK_SIZE;

		if(nbytes<(MDEV_BLOCK_SIZE-currentoffsetnumber))
		{
			length=nbytes;
		}
		else
		{
			length=MDEV_BLOCK_SIZE-currentoffsetnumber;
		}
		if(bs_bread(dev0,currentblocknumber,currentoffsetnumber,buf,length)!=OK)
		{
			return SYSERR;
		}
		
		filetable_ptr->fileptr=filetable_ptr+length;
		nbytes=nbytes-length;
		transferred_bytes=transferred_bytes+length;
		

	//	block=filetable_ptr->in.blocks[currentblocknumber]		  

		while(nbytes>0)
		{
			currentblocknumber=filetable_ptr->in.blocks[filetable_ptr/MDEV_BLOCK_SIZE];
			currentoffsetnumber=0;
			if(nbytes<MDEV_BLOCK_SIZE)
			{
				length=nbytes;
			}
			else{
				length=MDEV_BLOCK_SIZE;
			}
			if(block==-1)
			{
				break;
			}
			if(bs_bread(dev0,block,offset,buf+transferred_bytes,length)!=OK)
			{
				return SYSERR;
			}
			filetable_ptr->fileptr=filetable_ptr+length;
			nbytes=nbytes-length;
			transferred_bytes=transferred_bytes+length;

		}
		return nbytes;

	}

	  return SYSERR;
}

int fs_write(int fd, void *buf, int nbytes) {

	int currentblocknumber,currentoffsetnumber,length=0;
	filetable_ptr=&oft[fd];
	int length,m;
	int add_eof=0;
	int transferred_bytes;

	if(nbytes<0)
	{
		return SYSERR;
	}

	if(offset>=maxlength)
	{
		return SYSERR;
	}


	if(filetable_ptr->state==FSTATE_OPEN)
        {

		 maxlength=MDEV_BLOCK_SIZE*INODEDIRECTBLOCKS;
	         offset=nbytes+filetable_ptr->fileptr;

		currentblocknumber=filetable_ptr->in.blocks[filetable_ptr->fileptr/MDEV_BLOCK_SIZE];
		currentoffsetnumber=filetable_ptr->fileptr%MDEV_BLOCK_SIZE;
        
		 if(offset>filetable_ptr->in.size)
        	 {
                		add_eof=1;
        	 }


		if(nbytes<(MDEV_BLOCK_SIZE-currentoffsetnumber))
        	{
                	length=nbytes;
        	}
        	else
        	{
                	length=MDEV_BLOCK_SIZE-currentoffsetnumber;
        	}
	
		if(bs_bwrite(dev0,currentblocknumber,currentoffsetnumber,buf,length)!=OK)
        	{
                        return SYSERR;
        	}
		filetable_ptr->fileptr=filetable_ptr+length;
        	nbytes=nbytes-length;
        	transferred_bytes=transferred_bytes+length;

		while(nbytes>0)
		{
			for(i=FIRST_DATA_BLOCK;i<fsd.nblocks;i++)
			{
				m=fs_getmaskbit(next_free_data_block);
				if(m==0)
				{
					break;
				}	
				next_free_data_block++;
				if(next_free_data_block>=MDEV_NUM_BLOCKS)
				next_free_data_block=FIRST_DATA_BLOCK;		
			}
			if(m!=0)
			{
				return SYSERR;
			}
			fs_setmaskbit(next_free_data_block);
			fs_setmaskbit(BM_BLK);
			bs_bwrite(dev0,BM_BLK,0,fsd.freemask,fsd.freemaskbytes);

			filetable_ptr->in.blocks[filetable_ptr->fileptr/MDEV_BLOCK_SIZE] = next_free_data_block;

			blocknumber = filetable_ptr->in.blocks[filetable_ptr->fileptr/MDEV_BLOCK_SIZE];
			offsetnumber = 0;
			length = nbytes < MDEV_BLOCK_SIZE ? nbytes : MDEV_BLOCK_SIZE;
			if (bwrite(dev0, block, offset, buf+transferred_bytes, length)!=OK) 
			{
		
				return SYSERR;
			}
			filetable_ptr->fileptr =filetable_ptr->fileptr+length;
			nbytes =nbytes-length;
			transferred_bytes = transferred_bytes+length;

		}	

		if(add_eof)
		{
			filetable_ptr->in.size=filetable_ptr->fileptr;
		}
		put_inode_by_num(dev0, filetable_ptr->in.id, &filetable_ptr->in);
		return nbytes;
	}
  return SYSERR;
}
*/
#endif /* FS */
