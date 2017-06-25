#include "KernelProgram.hpp"

static bool		contextLoaded = false;

static	std::string	cl_err[65] =	{"CL_SUCCESS",
							"CL_DEVICE_NOT_FOUND",
							"CL_DEVICE_NOT_AVAILABLE",
							"CL_COMPILER_NOT_AVAILABLE",
							"CL_MEM_OBJECT_ALLOCATION_FAILURE",
							"CL_OUT_OF_RESOURCES",
							"CL_OUT_OF_HOST_MEMORY",
							"CL_PROFILING_INFO_NOT_AVAILABLE",
							"CL_MEM_COPY_OVERLAP",
							"CL_IMAGE_FORMAT_MISMATCH",
							"CL_IMAGE_FORMAT_NOT_SUPPORTED",
							"CL_BUILD_PROGRAM_FAILURE",
							"CL_MAP_FAILURE",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							" __ ",
							"CL_INVALID_VALUE",
							"CL_INVALID_DEVICE_TYPE",
							"CL_INVALID_PLATFORM",
							"CL_INVALID_DEVICE",
							"CL_INVALID_CONTEXT",
							"CL_INVALID_QUEUE_PROPERTIES",
							"CL_INVALID_COMMAND_QUEUE",
							"CL_INVALID_HOST_PTR",
							"CL_INVALID_MEM_OBJECT",
							"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
							"CL_INVALID_IMAGE_SIZE",
							"CL_INVALID_SAMPLER",
							"CL_INVALID_BINARY",
							"CL_INVALID_BUILD_OPTIONS",
							"CL_INVALID_PROGRAM",
							"CL_INVALID_PROGRAM_EXECUTABLE",
							"CL_INVALID_KERNEL_NAME",
							"CL_INVALID_KERNEL_DEFINITION",
							"CL_INVALID_KERNEL",
							"CL_INVALID_ARG_INDEX",
							"CL_INVALID_ARG_VALUE",
							"CL_INVALID_ARG_SIZE",
							"CL_INVALID_KERNEL_ARGS",
							"CL_INVALID_WORK_DIMENSION",
							"CL_INVALID_WORK_GROUP_SIZE",
							"CL_INVALID_WORK_ITEM_SIZE",
							"CL_INVALID_GLOBAL_OFFSET",
							"CL_INVALID_EVENT_WAIT_LIST",
							"CL_INVALID_EVENT",
							"CL_INVALID_OPERATION",
							"CL_INVALID_GL_OBJECT",
							"CL_INVALID_BUFFER_SIZE",
							"CL_INVALID_MIP_LEVEL",
							"CL_INVALID_GLOBAL_WORK_SIZE"};


#define TRANS_TEST 23
static	float	trans_raw[TRANS_TEST][4][2] = {
						{{0.000000, 0.000000},{0.402057, 0.109247},{0.518605, 0.114001},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.414170, 0.139253},{0.514511, 0.124288},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.418727, 0.170628},{0.505835, 0.138252},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.418727, 0.170628},{0.505835, 0.138252},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.414170, 0.139253},{0.514511, 0.124288},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.402057, 0.109247},{0.518605, 0.114001},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.375419, 0.053154},{0.509721, 0.084999},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.380994, -0.049551},{0.520627, 0.019576},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.373821, -0.057227},{0.531389, 0.032720},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.369698, -0.062491},{0.538664, 0.039776},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.366295, -0.067355},{0.545302, 0.045265},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.363016, -0.072527},{0.552267, 0.050222},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.359793, -0.078120},{0.559686, 0.054727},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.356243, -0.084934},{0.568560, 0.059203},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.353300, -0.091167},{0.576517, 0.062463},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.350563, -0.097503},{0.584444, 0.065066},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.348209, -0.103425},{0.591701, 0.066922},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.345658, -0.110413},{0.600075, 0.068471},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.343209, -0.117764},{0.608659, 0.069422},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.340918, -0.125326},{0.617243, 0.069746},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.338780, -0.133105},{0.625808, 0.069448},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.336539, -0.142190},{0.635465, 0.068361},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.334383, -0.152098},{0.645562, 0.066351},{1.000000, 0.000000}},
																												};


static	const char * FRAGMENT_SHADER_HEADER =
"#version 330\n"
"in vec4 outColor;\n"
"out vec4 fragColor;\n"
"\n"
"\n"
"uniform vec2           iResolution;\n"
"uniform float          iGlobalTime;\n"
"uniform sampler2D      iChannel0;\n"
"uniform sampler2D      iChannel1;\n"
"uniform sampler2D      iChannel2;\n"
"uniform sampler2D      iChannel3;\n"
"uniform sampler2D      iChannel4;\n"
"uniform sampler2D      iChannel5;\n"
"uniform sampler2D      iChannel6;\n"
"uniform sampler2D      iChannel7;\n"
"\n"
"void mainImage(vec2 f);\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord, float f)\n"
"{\n"
"       return texture(s, coord, f);\n"
"}\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord)\n"
"{\n"
"       return texture(s, coord);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	vec2 uv = gl_FragCoord.xy / iResolution;\n"
//"	uv.x *= iResolution.y / iResolution.x;\n"
"	fragColor = vec4(texture(iChannel0, uv).xyz, 1);\n"
"}\n"
"#line 1\n";

static	const char * VERTEX_SHADER_DEFAULT =
"#version 330\n"
"in vec2                fragPosition;\n"
"out vec4               outColor;\n"
"void main()\n"
"{\n"
"       gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"}\n";


static	float _renderVertices[] = {
	-1.0f, -1.0f,
	-1.0f,  1.0f,
	1.0f,  1.0f,
	1.0f,  1.0f,
	1.0f, -1.0f,
	-1.0f, -1.0f,
};
static	GLuint _renderCount = 6;



KernelProgram::KernelProgram(void)
{

	cl_int					err[3];
	CGLContextObj			cgl_ctx = CGLGetCurrentContext();              
    CGLShareGroupObj		cgl_sg = CGLGetShareGroup(cgl_ctx);
    cl_context_properties	ctx_props[] = { 
    							CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
								(cl_context_properties) cgl_sg, 0
    						};

	if (!contextLoaded)
	{
			//load OpenCL contex
	//	printf("%s\n", src);
    	err[0] = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &_device_id, NULL);
		_context = clCreateContext(ctx_props, 1, &_device_id, NULL, NULL, err + 1);
		_queue = clCreateCommandQueue(_context, _device_id, 0, err + 2);
		contextLoaded = true;
	}
	_firstUse = true;
	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
	bzero(&(_param), sizeof(t_ifs_param));
	_param.nb_iter = HARD_ITER;
	_param.len_trans = 4;
	_param.len_base = 4;
	CreateVAO();
}

KernelProgram::~KernelProgram(void)
{
	cl_int	err[6];
	
	err[0] = clFlush(_queue);
	err[1] = clFinish(_queue);
	for (const auto& b : _buff)
		if ((err[2] = clReleaseMemObject(b.second)))
			_print_err(err[2], "at line:", __LINE__, __func__, __FILE__);
	for (const auto& k : _kernels)
		if ((err[2] = clReleaseKernel(k.second)))
			_print_err(err[2], "at line:", __LINE__, __func__, __FILE__);
	err[3] = clReleaseCommandQueue(_queue);
	err[4] = clReleaseContext(_context);
	err[5] = clReleaseProgram(_program);
	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
}

std::string		KernelProgram::_LoadSourceFile(const std::string & filePath)
{
	std::string		fileSource = "";
	std::string		line;
	std::ifstream	file(filePath);

	while (std::getline(file, line)) {
			fileSource += line + "\n";
	}
	return fileSource;
}

bool			KernelProgram::LoadSourceFile(const std::string & fileName)
{
	try {
		_cl_source = _LoadSourceFile(fileName);
		return true;
	} catch (const std::exception & e) {
		return false;
	}
	return true;
}

bool			KernelProgram::CompileAndLink(void)
{
//	if (_fragmentFileSources.size() == 0)
//		return false;

	///////////////		OpenGL part	///////////////////
	GLuint		fragmentShaderId;
	//TODO: multi-shader file management for fragments and vertex
	const char	*fragmentSources[] = {FRAGMENT_SHADER_HEADER};// fichirer en dure 
	const char	*vertexSources[] = {VERTEX_SHADER_DEFAULT};
   
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, fragmentSources, NULL);
	glCompileShader(fragmentShaderId);
	if (!CheckCompilation(fragmentShaderId))
		return false;
	GLuint		vertexShaderId;

	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, vertexSources, NULL);
	glCompileShader(vertexShaderId);
	if (!CheckCompilation(vertexShaderId))
		return false;

	if (_id != 0)
		glDeleteProgram(_id);
	_id = glCreateProgram();
	glAttachShader(_id, vertexShaderId);
	glAttachShader(_id, fragmentShaderId);
	glLinkProgram(_id);
	if (!CheckLink(_id))
		return false;
	//inizialize fragPosition
	GLint       fragPos;

	if ((fragPos = glGetAttribLocation(_id, "fragPosition")) < 0)
		return false;

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glEnableVertexAttribArray(fragPos);

	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);


	//////////////	OpenCL part ///////////////////
	const char * 			src = _cl_source.c_str();
	size_t					src_size = _cl_source.length();
	cl_int					err[20];

	bzero(err, sizeof(err));
	_program = clCreateProgramWithSource(_context, 1, &src, &src_size, err + 0);
    err[18] = clBuildProgram(_program, 1, &_device_id, NULL, NULL, NULL);
	if (err[18])
	{
		size_t	size = 0;
		clGetProgramBuildInfo(_program, _device_id, CL_PROGRAM_BUILD_LOG , 0, NULL, &size);
		char *buildlog = (char*)malloc(size);
		clGetProgramBuildInfo(_program, _device_id, CL_PROGRAM_BUILD_LOG , size, buildlog,NULL);
		std::cout << buildlog << std::endl;
		free(buildlog);
	}
	_kernels["calcul_ifs_point"] = clCreateKernel(_program, "calcul_ifs_point", err + 1);
	_kernels["define_color"] = clCreateKernel(_program, "define_color", err + 2);
	_kernels["draw_line"] = clCreateKernel(_program, "draw_line", err + 3);	
	_kernels["clear"] = clCreateKernel(_program, "clear", 0);

   /** Create a texture to be displayed as the final image. */
	glActiveTexture(GL_TEXTURE1);
//	_screen_tex = SOIL_load_OGL_texture("textures/Kifs1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS |	SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	glGenTextures(1, &_screen_tex);
    glBindTexture(GL_TEXTURE_2D, _screen_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_size.x, framebuffer_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	printf("==>%d\n", _screen_tex);
    _buff["screen"] = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _screen_tex, 0);

	_buff["pt_ifs"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, MAX_GPU_BUFF, NULL, err + 4);
	_buff["col_pt"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, MAX_GPU_BUFF / 2, NULL, err + 5);
	_buff["ifs_param"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, sizeof(t_ifs_param), NULL, err + 6);
	err[11] = clSetKernelArg(_kernels["define_color"], 0, sizeof(cl_mem), &_buff["col_pt"]);
	err[12] = clSetKernelArg(_kernels["define_color"], 1, sizeof(cl_mem), &_buff["ifs_param"]);

	err[13] = clSetKernelArg(_kernels["calcul_ifs_point"], 0, sizeof(cl_mem), &_buff["pt_ifs"]);
	err[14] = clSetKernelArg(_kernels["calcul_ifs_point"], 1, sizeof(cl_mem), &_buff["ifs_param"]);
	
	err[15] = clSetKernelArg(_kernels["draw_line"], 1, sizeof(cl_mem), &_buff["pt_ifs"]);
	err[16] = clSetKernelArg(_kernels["draw_line"], 2, sizeof(cl_mem), &_buff["col_pt"]);
	err[17] = clSetKernelArg(_kernels["draw_line"], 3, sizeof(cl_mem), &_buff["ifs_param"]);

	clEnqueueAcquireGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	err[8]	= clSetKernelArg(_kernels["draw_line"], 0, sizeof(cl_mem), &_buff["screen"]);
	err[19] = clSetKernelArg(_kernels["clear"], 0, sizeof(cl_mem), &_buff["screen"]);
	clEnqueueReleaseGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	clFinish(_queue);
	_loaded = true;
	// ---- 
	return (_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__));
}

// <---
void		KernelProgram::Use(void)
{
	if (_firstUse)
		__localParams["localStartTime"] = glfwGetTime(), _firstUse = false;
	glUseProgram(_id);
}


// <---
void			KernelProgram::CreateVAO(void)
{
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//TODO: Vector3 management
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _renderCount * 2, _renderVertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
}

// <---
void		KernelProgram::UpdateUniforms(const vec2 winSize, bool pass)
{
	size_t	global_work_size[3] = {1, 0, 0};
	size_t	local_work_size[3] = {1, 0, 0};
//	cl_int	err[10];
	cl_int	err[4];

	(void)pass;
	bzero(err, sizeof(err));
	_set_base();
	err[0] = clEnqueueWriteBuffer(_queue, _buff["pt_ifs"], CL_TRUE, 0, 4 * 2 * sizeof(float), &_param.pt_base, 0, NULL, NULL);
	_setIdPtBuff(_param.len_base, _param.len_trans, _param.nb_iter, _idPtBuff);
	setParam(&_param);
	err[1] = clEnqueueWriteBuffer(_queue, _buff["ifs_param"], CL_TRUE, 0, sizeof(t_ifs_param), &_param, 0, NULL, NULL);
	global_work_size[0] = _idPtBuff[_param.nb_iter - 1] - _idPtBuff[_param.nb_iter - 2];
//	printf("worksize def_col:%d\n", global_work_size[0]);
	err[2] = clEnqueueNDRangeKernel(_queue, _kernels["define_color"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	if (!_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__))
	{
		exit(0);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _screen_tex);
	int id = glGetUniformLocation(_id, "iChannel0");
	glUniform1i(id, _screen_tex);
	id = glGetUniformLocation(_id, "iResolution");
	glUniform2f(id, winSize.x, winSize.y);
}

// <---
void		KernelProgram::UpdateFramebufferSize(const vec2 fbSize)
{
	cl_int	err[5];

	bzero(err, sizeof(err));

	clReleaseMemObject(_buff["screen"]);
	glDeleteTextures(1, &_screen_tex);

	glGenTextures(1, &_screen_tex);
    glBindTexture(GL_TEXTURE_2D, _screen_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbSize.x, fbSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	printf("==>%d\n", _screen_tex);

	_buff["screen"] = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _screen_tex, 0);
	
	err[0]	= clSetKernelArg(_kernels["draw_line"], 0, sizeof(cl_mem), &_buff["screen"]);
	err[1] = clSetKernelArg(_kernels["clear"], 0, sizeof(cl_mem), &_buff["screen"]);

	if (!_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__))
		exit(0);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_size.x, framebuffer_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void		KernelProgram::Draw(void)
{
	// -------- on lance le calcul des point
	cl_int		err[3];
	size_t		screen[2];
	size_t		global_work_size[3] = {1, 0, 0};
	size_t		local_work_size[3] = {1, 0, 0};
	int	i;

	bzero(err, sizeof(err));
	i = 1;
	
	while (i < _param.nb_iter)
	{
		global_work_size[0] = _idPtBuff[i + 1] - _idPtBuff[i];

		err[0] = clSetKernelArg(_kernels["calcul_ifs_point"], 2, sizeof(int), &i);
		err[1] = clEnqueueNDRangeKernel(_queue, _kernels["calcul_ifs_point"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
		i++;
	}

	screen[0] = framebuffer_size.x;
	screen[1] = framebuffer_size.y;

	//////////

//	#------		On draw les ligne dans le buffer partager	------#

	clEnqueueAcquireGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	{
		clEnqueueNDRangeKernel(_queue, _kernels["clear"], 2, NULL, screen, NULL, 0, NULL, NULL);
		global_work_size[0] = _idPtBuff[_param.nb_iter - 1] - _idPtBuff[_param.nb_iter - 2] - 1;
//		printf("glob:");
		err[2] = clEnqueueNDRangeKernel(_queue, _kernels["draw_line"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	}
//	printf("worksize last_draw_line:%d\n", global_work_size[0]);
	clEnqueueReleaseGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	clFinish(_queue);
	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
//	#------		On affiche le buffer avec OpenGL	------#

#ifdef DEBUG
	std::cout << "drawing program: " << _id << " to " << _vao << "\n";
#endif
	glBindVertexArray(_vao);
// $---
	glDrawArrays(GL_TRIANGLE_STRIP, 0, _renderCount);
}

void		KernelProgram::_print_err(cl_int err, std::string msg, int line, std::string func, std::string file)
{
	std::cout << "code_error:"<< err <<"	 "<< msg << line << "	function:" << func << "	file:" << file;
	if (err > -63 || err <= 0)
		std::cout << " *** " << cl_err[-err] << " *** " << std::endl;
	else
		std::cout << " *** not a cl error ***" << std::endl;
}

bool		KernelProgram::_check_err_tab(cl_int * err, int nb_err, std::string func, std::string file)
{
	bool	err_found = false;

	for (int i = 0; i < nb_err; i++)
		if (err[i])
			_print_err(err[i], " at id:", i, func, file), err_found = true;
	if (err_found)
	{
		for (int k = 0; k < 10000; k++);
		exit(0);
	}
	return (!err_found);
}

void		KernelProgram::_setRangeVal(t_range *value, float beg, float end)
{
	value->beg = beg;
	value->end = end;
	value->delta = end - beg;
}

void		KernelProgram::_setIdPtBuff(int nb_base, int nb_trans, int nb_iter, int *indice_beg)
{
	(void)nb_iter;
	int		sum;
	int		i;

	i = 1;
	indice_beg[0] = 0;
	sum = nb_base;
	while (i < MAX_ITER)
	{
		sum += (sum - 1) * (nb_trans - 2);// le truc presedent
		indice_beg[i] = indice_beg[i - 1] + sum;
		i++;
	}
}

void	KernelProgram::setParam(t_ifs_param *param)
{
	static	float	add = 0.5;
	static	float	id_trans = 11;
	int				lim = 9;
	int				beg = 11;
	int				i;

//	id_trans = 32;
	for (i = 0; i < 4; i++)
	{
		param->pt_trans[i][0] = trans_raw[(int)id_trans][i][0];
		param->pt_trans[i][1] = trans_raw[(int)id_trans][i][1];
	}
	if ((add > 0 && id_trans > beg + lim) || (add < 0 && id_trans < beg - lim))
		add *= -1;
	id_trans += add;

//	std::cout << "len_trans:" << _param.len_trans << std::endl; 
//	exit(0);

//	param->len_trans = 4;			// ##<<
//	param->len_base = 4;//get_polygone_len(e->base);
	_setIdPtBuff(param->len_base, param->len_trans, param->nb_iter, _idPtBuff);
	param->max_pt = _idPtBuff[_param.nb_iter] - _idPtBuff[_param.nb_iter - 1]; 
	param->ecr_x = framebuffer_size.x;
	param->ecr_y = framebuffer_size.y;
	

	_setRangeVal(&(param->hue), 0.3, 0.4);
	_setRangeVal(&(param->sat), 0.5, 0.6);
	_setRangeVal(&(param->val), 0.8, 0.99);

	memmove(&(param->beg_id), _idPtBuff, sizeof(int) * MAX_ITER);
//	printf("param->ecrX:%d	param->ecrY:%d\n", param->dim_ecr[0], param->dim_ecr[1]);
}


bool		KernelProgram::CheckCompilation(GLuint shader)
{
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteShader(shader);
		return false;
	}
	return true;
}

bool		KernelProgram::CheckLink(GLuint program)
{
	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteProgram(program);
		return false;
	}
	return true;
}


vec_2	add_rot(vec_2 beg, vec_2 ux, vec_2 uy, const float r, float val, float speed, float offset)
{
	vec_2 ret;
	
	val = (val * speed + offset) * 2 * M_PI;
	ret.x = beg.x + ux.x * cos(val) * r + uy.x * sin(val) * r;
	ret.y = beg.y + ux.y * cos(val) * r + uy.y * sin(val) * r;
	return (ret);
}

void	KernelProgram::_set_base()
{
	float	r;
	float	time = glfwGetTime() - __localParams["localStartTime"];
//	std::cout << (glfwGetTime() - __localParams["localStartTime"]) << std::endl;
	vec_2	beg = {framebuffer_size.x / 2, framebuffer_size.y / 2};
//	vec_2	ret;
	vec_2	ux = {0, 1};
	vec_2	uy = {1, 0};
	vec_2	base[MAX_NODE];
	r = 200 + 20 * sin(time);
	bzero(base, sizeof(base));

	base[0] = add_rot(beg, ux, uy, r, time, 0.005 * cos(time) + 0.2, 0);
	base[1] = add_rot(beg, ux, uy, r, time, 0.005 * cos(time) + 0.2, 1.0 / 3.0);
	base[2] = add_rot(beg, ux, uy, r, time, 0.005 * cos(time) + 0.2, 2.0 / 3.0);
	base[3] = add_rot(beg, ux, uy, r, time, 0.005 * cos(time) + 0.2, 0);

	base[1] = add_rot(base[1], ux, uy, 100, time, -0.5, 0);
	base[2] = add_rot(base[2], ux, uy, 70, time, 0.6, 0);
	base[3] = add_rot(base[3], ux, uy, 30, time, -0.3, 0);

	base[1] = add_rot(base[1], ux, uy, 30, time, -0.6, 0);
	base[2] = add_rot(base[2], ux, uy, 20, time, -0.3, 0);
	base[3] = add_rot(base[3], ux, uy, 40, time, 0.4, 0);

	base[0] = base[3];
//	std::cout << "" ;
	for (int i = 0; i < MAX_NODE; i++)
	{
		this->_param.pt_base[i][0] = base[i].x;
		this->_param.pt_base[i][1] = base[i].y;
//		std::cout << "pt:{" << _param.pt_base[i][0] << ", " << _param.pt_base[i][1]<< "}"<< std::endl;
	}
}


void	gl_test(const int line, const char * func, const char * file)
{
	GLenum gl_err = 0;

//	std::cout << "err code:"<< gl_err << "	file:" << file << "	func:" << func << "	line:"<< line << std::endl;
	while((gl_err = glGetError()) != GL_NO_ERROR)
	{
		  //Process/log the error.
		std::cout << "err code:"<< gl_err << "	file:" << file << "	func:" << func << "	line:"<< line << std::endl;
//		exit(0);
	}
}
