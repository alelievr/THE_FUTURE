#ifndef KERNELPROGRAM_HPP
# define KERNELPROGRAM_HPP
# include <iostream>
# include <string>
# include <map>
# include <exception>
# include <fstream>
# include <OpenCL/OpenCL.h>
# include <OpenGL/OpenGL.h>
# include <cmath>
# include "shaderpixel.h"
# include "ICGProgram.hpp"


# define MAX_GPU_BUFF ((size_t)(1048577 * 5)) 
# define MAX_ITER 20
# define MAX_NODE 16
# define NB_ANIME 5

# define HARD_ITER 8

typedef	struct	s_vec_2
{
	float	x;
	float	y;
}				vec_2;

typedef	struct	s_range
{
	float		beg;
	float		end;
	float		delta;
	float		none;
}				t_range;

typedef	struct	t_trans_param
{
	int		len_base;
	int		len_trans;
	float	val[20][8];
}				tr_param;

typedef	struct	s_ifs_param
{
	float	pt_base[MAX_NODE][2];
	float	pt_trans[MAX_NODE][2];
	int		beg_id[MAX_ITER];
	int		len_base;
	int		len_trans;
	int		max_pt;
	int		ecr_x;
	int		ecr_y;
	int		nb_iter;
	int		nb_part;
	float	color;
	t_range	hue;
	t_range	sat;
	t_range	val;
}				t_ifs_param;

void	set_trans_ovaloid2(t_ifs_param *param, float time, float trans[][8], int size);
void	gl_test(const int line, const char * func, const char * file);
vec_2	add_rot(vec_2 beg, vec_2 ux, vec_2 uy, const float r, float val, float speed, float offset);

class		KernelProgram : public ICGProgram
{
	private:
		std::string							_cl_source;
		cl_context							_context;
		cl_command_queue					_queue;
		cl_program							_program;
		std::map< std::string, cl_kernel>	_kernels;
		std::map< std::string, cl_mem>		_buff;
		t_ifs_param							_param;
		int									_idPtBuff[MAX_ITER];
    	GLuint 								_screen_tex;
		bool								_loaded;
    	cl_device_id						_device_id;
		bool								_firstUse;
		bool								_need_update;
		size_t								_size_buff;
		tr_param							_anime[NB_ANIME];
		int									_prev_anime;
		float								_time;
//		int									_nbAnime;

		GLuint								_id; //opengl program id
		int									_renderId;
		int									_framebufferId;
		GLuint								_vao;
		GLuint								_vbo;

		void								_SetBaseFix();
		void								_Ajust_iter();
		void								_InitAnime();
		void								SetParamAnime(int id);
		void								_Set_base();
		void								_SetBasefeuille();
		std::string							_LoadSourceFile(std::string & filePath);
		void								_print_err(cl_int err, std::string msg, int line, std::string func, std::string file);
		bool								_check_err_tab(cl_int * err, int nb_err, std::string func, std::string file);
		std::string							_LoadSourceFile(const std::string & filePath);
		void								_SetIdPtBuff();
		void								_SetRangeVal(t_range *value, float beg, float end);
		void								CreateVAO(void);
		bool								CheckCompilation(GLuint shader);
		bool								CheckLink(GLuint program);


	public:
		KernelProgram(void);
		KernelProgram(const KernelProgram&) = delete;
		virtual ~KernelProgram(void);
		KernelProgram &	operator=(KernelProgram const & src) = delete;


		void	setParam(t_ifs_param *spec);
		bool	LoadSourceFile(const std::string & fileName);
		bool	CompileAndLink(void);
		void	Use(void);
		void	UpdateUniforms(const vec2 winSize, bool pass = false);
		void	UpdateFramebufferSize(const vec2 fbSize);
		void	Draw(void);
};

std::ostream &	operator<<(std::ostream & o, KernelProgram const & r);

#endif
