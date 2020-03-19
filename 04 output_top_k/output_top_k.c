/****
	@output_top_n.c
	@YUGY 2020/3/18/

	@usage:
		1.init_top_k(top_k);
		2.calculate_top_n( result, jpg_path);
		3.output_top_k();
		4.reset_top_k();	//clear old top k in memory when use calculate_top_n( result, jpg_path); again
		5.calculate_top_n( result, jpg_path);
		6.output_top_k();
		7.free_top_k();
****/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "output_top_k.h"

#define JPGPATHLEN 256

typedef struct FACE_TOP_N_ST
{
    char  jpg_path[JPGPATHLEN];
    float result;
} FACE_TOP_N_S;

static FACE_TOP_N_S *SAVE_K = NULL;
static unsigned int TOP_K = 1;

static void jpg_path_dump(char *path_dst, const char *path_src)
{
	memset(path_dst,0x00, JPGPATHLEN);
	strncpy(path_dst, path_src, JPGPATHLEN);
}

/**
	@fun 	:init_top_k(unsigned int top_k)
	@brief	:init top k memory
	@param	:top_k -- out put rules from top to buttom
**/
int init_top_k(unsigned int top_k)
{
	char *p1 = NULL;

	if(SAVE_K)
	{
		printf("init_top_k already!\n");
		return -1;
	}

	if(top_k<=0)
	{
		top_k=1;
	}
	TOP_K = top_k;

	p1 = (char *)malloc((sizeof(FACE_TOP_N_S) * TOP_K));
	if(NULL == p1 )
	{
		printf("malloc error\n");
		return -2;
	}
	memset(p1, 0x00, (sizeof(FACE_TOP_N_S) * TOP_K));

	SAVE_K = (FACE_TOP_N_S *)p1;

	return 0;
}

/**
	@fun 	:clear_top_k(void)
	@brief	:free top k memory
**/
void free_top_k(void)
{
	if(SAVE_K)
	{
		free(SAVE_K);
		SAVE_K = NULL;
	}
}

/**
	@fun 	:output_top_k(void)
	@brief	:out put top k in memory
**/
void output_top_k(void)
{
	int i = 0;

	if(NULL == SAVE_K)
	{
		printf("No top k result saved\n");
		return;
	}
	printf("top_k=%d\n",TOP_K);
	for(; i<TOP_K; i++)
	{
		printf("[%03d] lib face path:%s  score:%f\n",i,SAVE_K[i].jpg_path, SAVE_K[i].result);
	}
}

/**
	@fun 	:reset_top_k(void)
	@brief	:clear old top k in memory
**/
void reset_top_k(void)
{
	if(NULL == SAVE_K)
	{
		printf("%s :mem SAVE_K uninit,must run init_top_k() first\n",__FUNCTION__);
		return ;
	}

	memset(SAVE_K, 0x00, (sizeof(FACE_TOP_N_S) * TOP_K));
}

/**
	@fun 	:calculate_top_n(float result, const char *jpg_path)
	@brief	:calculate result from top to buttom
	@param	:result	  -- new result from mgvl1_compare()
	@param	:jpg_path -- whitch jpg file(path) corresponding to result
**/
int calculate_top_n(float result, const char *jpg_path)
{
	unsigned int j = 0 , position = 0;
	float result_tmp = 0.0;
	char  tmp_jpg_path[JPGPATHLEN] = {0};

	FACE_TOP_N_S *RET = NULL;

	if(NULL == SAVE_K)
	{
		printf("%s :mem SAVE_K uninit,must run init_top_k() first\n",__FUNCTION__);
		return -1;
	}
	RET = SAVE_K;

	for(j=0; j<TOP_K; j++)
	{
		if(result > RET[j].result)
		{	
			/** 备份 **/
			result_tmp = RET[j].result;
			jpg_path_dump(tmp_jpg_path,RET[j].jpg_path);

			/** 插入 **/
			RET[j].result = result;
			jpg_path_dump(RET[j].jpg_path, jpg_path);

			position = ((TOP_K-1) - j); /** TOP_K-1 :是最后一个数组的下标**/

			/** 重新排列 **/
			if(position)	/** 如果输入数值不止大于原TOP_K中最小一个 ，重新排列top_n**/
			{
				for( j=(TOP_K-1); position > 1; position--,j--)
				{
					RET[j].result = RET[j-1].result;
					jpg_path_dump(RET[j].jpg_path, RET[j-1].jpg_path);
				}
				RET[j].result = result_tmp;
				jpg_path_dump(RET[j].jpg_path, tmp_jpg_path);
			}
		}

		/** 排列完成后,跳出！---很重要！ **/
		break;
	}

	return 0;
}
# if 1
int main(int argc, char **argv)
{
	int i=0,size=30;
	FACE_TOP_N_S FACE[size];
	FACE_TOP_N_S FACE2[size];

	if(argc < 2)
	{
		printf("usage:\n");
		printf("%s <top_k>\n",argv[0]);
		return -1;
	}
	
	float val = 61.55;
	for (; i < size; i++,val++)
	{
		FACE[i].result = val;
		sprintf(FACE[i].jpg_path,"/test/%d.jpg",i);

		
		FACE2[i].result = val;
		sprintf(FACE2[i].jpg_path,"/TEST/%d.jpg",i);
	}


	init_top_k(atoi(argv[1]));
	
	for (i=0; i < size; i++)
	{
		printf("ret:%f [%s]\n",FACE[i].result,FACE[i].jpg_path);
		calculate_top_n(FACE[i].result,FACE[i].jpg_path);
	}
	output_top_k();

	reset_top_k();
	for (i=0; i < size; i++)
	{
		printf("ret:%f [%s]\n",FACE2[i].result,FACE2[i].jpg_path);
		calculate_top_n(FACE2[i].result,FACE2[i].jpg_path);
	}
	output_top_k();

	return 0;
}
#endif
