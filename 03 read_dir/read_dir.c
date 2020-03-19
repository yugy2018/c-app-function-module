/**
	@file	:red_dir.c
	@brief	:show file while search dir,and sub dir .
	@author	: YUGY 2020/3/19
**/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <errno.h>
#include <dirent.h>

/**
	@fun	:search_dir(char *path)
	@brief	:show file while search dir,and sub dir .
	@param	: path, whitch dir path you want to search
**/
int search_dir(char *path)
{
	DIR *dir=NULL;
	struct dirent *entry=NULL;
	
	if((dir=opendir(path))==NULL)
	{
		printf("opendir failed\n");
		return -1;
	}
	else
	{
		printf("[%s]\n",path);
		while((entry=readdir(dir))!=NULL)///循环读取列表中目录（子目录）或文件的信息
		{
			/** 滤过本地目录及上级目录的信息，防止死循环 **/
			if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
				continue;

			/** 子目录，递归继续遍历子目录 **/
			if(DT_DIR==entry->d_type)
			{
				char pathbuf[1024]={0};
				sprintf(pathbuf,"%s/%s",path,entry->d_name);
				search_dir(pathbuf);
			}
			else/** 文件 **/
			{
				printf("\t%s/%s\n",path,entry->d_name);
			}
		}
	}
	return 0;
}



int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("usage");
		printf("%s <dir_path>\n",argv[0]);
		return -1;
	}
	search_dir(argv[1]);
	return 0;
}

