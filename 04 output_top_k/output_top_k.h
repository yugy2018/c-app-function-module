/****
	@output_top_n.h
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


/**
	@fun 	:calculate_top_n(float result, const char *jpg_path)
	@brief	:calculate result from top to buttom
	@param	:result	  -- new result from mgvl1_compare()
	@param	:jpg_path -- whitch jpg file(path) corresponding to result
**/
int calculate_top_n(float result, const char *jpg_path);

/**
	@fun 	:reset_top_k(void)
	@brief	:clear old top k in memory
**/
void reset_top_k(void);

/**
	@fun 	:output_top_k(void)
	@brief	:out put top k in memory
**/
void output_top_k(void);

/**
	@fun 	:init_top_k(unsigned int top_k)
	@brief	:init top k memory
	@param	:top_k -- out put rules from top to buttom
**/
int init_top_k(unsigned int top_k);

/**
	@fun 	:free_top_k(void)
	@brief	:free top k memory
**/
void free_top_k(void);










