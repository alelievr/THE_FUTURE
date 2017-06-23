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


# define MAX_GPU_BUFF ((size_t)(1048577)) 
# define MAX_ITER 20
# define MAX_NODE 16

# define HARD_ITER 6

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

typedef	struct	s_ifs_param
{
	float	pt_base[MAX_NODE][2];
	float	pt_trans[MAX_NODE][2];
	int		beg_id[MAX_ITER];
	int		len_base;
	int		len_trans;
	int		max_iter;
	int		max_pt;
	int		ecr_x;
	int		ecr_y;
	int		nb_iter;
	int		nb_part;
	t_range	hue;
	t_range	sat;
	t_range	val;
}				t_ifs_param;

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

		GLuint								_id; //opengl program id
		int									_renderId;
		int									_framebufferId;
		GLuint								_vao;
		GLuint								_vbo;

		void								_set_base();
		std::string							_LoadSourceFile(std::string & filePath);
		void								_print_err(cl_int err, std::string msg, int line, std::string func, std::string file);
		bool								_check_err_tab(cl_int * err, int nb_err, std::string func, std::string file);
		std::string							_LoadSourceFile(const std::string & filePath);
		void								_setIdPtBuff(int nb_base, int nb_trans, int nb_iter, int *indice_beg);
		void								_setRangeVal(t_range *value, float beg, float end);
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
