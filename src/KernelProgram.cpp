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

static	float	anime_trans[20][8] = {
		{0.000000, 0.000000, 0.059173, -0.065980, -0.065980, -0.059173, 0.000000, 0.000000},
		{0.495863, -0.229901, 0.059173, -0.065980, -0.072465, -0.063004, 1.993007, 0.000000},
		{0.354023, -0.222185, -0.047905, 0.042355, -0.065980, -0.059173, 2.342657, 0.000000},
		{0.642503, 0.119864, -0.042669, 0.023241, -0.065980, -0.059173, 1.958042, 0.000000},
		{0.878478, 0.073556, 0.059173, -0.065980, -0.036211, -0.050332, 3.461539, 0.000000},
		{1.000000, 0.000000, 0.059173, -0.065980, -0.065980, -0.059173, 0.000000, 0.000000}
		};

static	const	float	tr1[][8] = {
		{0.000000, 0.000000, 0.059173, -0.065980, -0.065980, -0.059173, 0.000000, 0.000000},
		{0.495863, -0.229901, 0.059173, -0.065980, -0.072465, -0.063004, 1.993007, 0.000000},
		{0.354023, -0.222185, -0.047905, 0.042355, -0.065980, -0.059173, 2.342657, 0.000000},
		{0.642503, 0.119864, -0.042669, 0.023241, -0.065980, -0.059173, 1.958042, 0.000000},
		{0.878478, 0.073556, 0.059173, -0.065980, -0.036211, -0.050332, 3.461539, 0.000000},
		{1.000000, 0.000000, 0.059173, -0.065980, -0.065980, -0.059173, 0.000000, 0.000000}
		};

static	const	float	tr2[][8] = {
 {0.000000, 0.000000, -0.000966, -0.028188, -0.084421, 0.024106, 1.608392, 0.000000},
 {0.393019, -0.407196, -0.085639, 0.073778, -0.027376, -0.032149, 1.258741, 0.000000},
 {0.536538, 0.009842, 0.058538, -0.103601, -0.085826, -0.107804, 2.657343, 0.000000},
 {0.773706, 0.121025, 0.068643, -0.177906, 0.057309, -0.094022, 1.818182, 0.000000},
 {0.579890, -0.082534, 0.036253, -0.032577, -0.012322, -0.024490, 2.727273, 0.000000},
 {1.000000, -0.000000, 0.061555, -0.078014, -0.078014, -0.061555, 0.000000, 0.000000}
		};

static	const	float	tr3[][8] = {
{0.000000, 0.000000, -0.000966, -0.028188, -0.084421, 0.024106, 1.608392, 0.000000},
 {0.581053, 0.167329, -0.085639, 0.073778, -0.027376, -0.032149, 1.258741, 0.000000},
 {0.286840, 0.112401, 0.015427, -0.129178, -0.209364, -0.015405, 2.657343, 0.000000},
 {0.633589, 0.132831, 0.068643, -0.177906, 0.057309, -0.094022, 1.818182, 0.000000},
 {0.660954, 0.124888, 0.137199, -0.176556, -0.012322, -0.024490, 2.727273, 0.000000},
 {1.000000, -0.000000, 0.061555, -0.078014, -0.078014, -0.061555, 0.000000, 0.000000}
};

static	const	float	tr4[][8] = {
 {0.000000, 0.000000, -0.000249, -0.028346, -0.085436, 0.022062, 1.608392, 0.000000},
 {0.737955, 0.080898, -0.087930, 0.071937, -0.026683, -0.033001, 1.258741, 0.000000},
 {0.527880, -0.197606, 0.018803, -0.129393, -0.209958, -0.020831, 2.657343, 0.000000},
 {0.684655, 0.066899, 0.073516, -0.176991, 0.059983, -0.093001, 1.818182, 0.000000},
 {1.000000, 0.000000, 0.142361, -0.173882, -0.011754, -0.024921, 2.727273, 0.000000}
};

static	const	float	tr5[][8]{
{0.000000, 0.000000, -0.076981, 0.070566, 0.003019, -0.069434, 2.482517, 0.000000},
 {0.552830, 0.184906, -0.069151, 0.090472, -0.062264, -0.067925, 0.909091, 0.000000},
 {0.413774, -0.066792, 0.141415, -0.152547, 0.011226, -0.158208, 2.867133, 0.000000},
 {0.629811, -0.135660, -0.054434, 0.051981, 0.112170, -0.179906, 0.734266, 0.000000},
 {1.000000, 0.000000, 0.075660, 0.059811, -0.025660, 0.040189, 4.405594, 0.000000}
};

static	const	float	tr6[][8]{
 {0.000000, 0.000000, 0.078872, -0.106331, -0.106331, -0.078872, 2.027972, 0.000000},
 {0.053049, 0.017371, 0.078872, -0.106331, -0.106331, -0.078872, 0.000000, 0.000000},
 {0.558726, 0.172681, 0.046184, -0.028929, 0.199829, -0.099028, 4.720280, 0.000000},
 {0.565697, 0.155875, 0.136955, -0.055006, 0.061978, 0.120148, 1.538462, 0.000000},
 {0.949814, 0.023214, 0.078872, -0.106331, -0.106331, -0.078872, 0.000000, 0.000000},
 {1.000000, 0.000000, -0.097733, 0.128055, -0.106331, -0.078872, 2.937063, 0.000000}
};

static	const	float	tr7[][8]{
 {0.000000, 0.000000, 0.018302, -0.007804, -0.001632, -0.142119, 4.755245, 0.000000},
 {0.320940, 0.351753, 0.187205, -0.036263, -0.015873, 0.010027, 4.020979, 0.000000},
 {0.688919, 0.081377, 0.132271, -0.171359, 0.032879, 0.005537, 2.097902, 0.000000},
 {0.632292, -0.144554, -0.044202, 0.001958, -0.023291, -0.157477, 1.398601, 0.000000},
 {0.432143, 0.020488, 0.005846, -0.025900, -0.144532, -0.105487, 1.328671, 0.000000},
 {1.000000, 0.000000, 0.013856, -0.002945, 0.006068, -0.126143, 1.923077, 0.000000}
};

static	const	float	tr8[][8]{
 {-1.000000, 0.000000, 0.074547, -0.064220, -0.064220, -0.074547, 0.139860, 0.000000},
 {0.271347, -0.177481, 0.174878, -0.131172, 0.006594, 0.005142, 2.902098, 0.000000},
 {0.476980, -0.315667, 0.074547, -0.064220, -0.064220, -0.074547, 0.629371, 0.027972},
 {0.554495, -0.044782, -0.030776, 0.069803, -0.008746, -0.226232, 2.832168, 0.000000},
 {0.507767, 0.333136, -0.072578, 0.069017, -0.064220, -0.074547, 0.384615, 0.000000},
 {0.785805, 0.128246, 0.152611, -0.144457, -0.048396, -0.005927, 1.643357, 0.000000},
 {1.000000, -0.000000, 0.074547, -0.064220, -0.064220, -0.074547, 0.000000, 0.000000}
};

// feuille
static	const	float	tr9[][8]{
 {0.000000, 0.000000, 0.115612, -0.065580, -0.026854, -0.058117, 0.000000, 0.000000},
 {0.005518, -0.076480, -0.012618, -0.013625, -0.015366, -0.020624, 3.881119, 0.000000},
 {0.534169, -0.211314, 0.027623, -0.018114, -0.074478, -0.012765, 1.888112, 0.010490},
 {1.036747, -0.025734, 0.032123, -0.023111, 0.009136, -0.052860, 0.000000, 0.000000},
 {1.000000, -0.000000, 0.067502, -0.074964, -0.074964, -0.067502, 0.000000, 0.300699}
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
"const float blurSize = 1.0/512.0;\n"
"const float intensity = 1.55;\n"
"void main()\n"
"{\n"
"	vec2 fragCoord = gl_FragCoord.xy;\n"
"   vec4 sum = vec4(0);\n"
"   vec2 texcoord = fragCoord.xy/iResolution.xy;\n"
"   int j;\n"
"   int i;\n"
"\n"
"   //thank you! http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/ for the \n"
"   //blur tutorial\n"
"   // blur in y (vertical)\n"
"   // take nine samples, with the distance blurSize between them\n"
"   sum += texture(iChannel0, vec2(texcoord.x - 4.0*blurSize, texcoord.y)) * 0.05;\n"
"   sum += texture(iChannel0, vec2(texcoord.x - 3.0*blurSize, texcoord.y)) * 0.09;\n"
"   sum += texture(iChannel0, vec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.12;\n"
"   sum += texture(iChannel0, vec2(texcoord.x - blurSize, texcoord.y)) * 0.15;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y)) * 0.16;\n"
"   sum += texture(iChannel0, vec2(texcoord.x + blurSize, texcoord.y)) * 0.15;\n"
"   sum += texture(iChannel0, vec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.12;\n"
"   sum += texture(iChannel0, vec2(texcoord.x + 3.0*blurSize, texcoord.y)) * 0.09;\n"
"   sum += texture(iChannel0, vec2(texcoord.x + 4.0*blurSize, texcoord.y)) * 0.05;\n"
"	\n"
"	// blur in y (vertical)\n"
"   // take nine samples, with the distance blurSize between them\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y - 4.0*blurSize)) * 0.05;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y - 3.0*blurSize)) * 0.09;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.12;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y - blurSize)) * 0.15;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y)) * 0.16;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y + blurSize)) * 0.15;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.12;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y + 3.0*blurSize)) * 0.09;\n"
"   sum += texture(iChannel0, vec2(texcoord.x, texcoord.y + 4.0*blurSize)) * 0.05;\n"
"\n"
"   //increase blur with intensity!\n"
"   fragColor = sum * intensity + texture(iChannel0, texcoord); \n"
"	fragColor.a = 1;\n"
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
    	err[0] = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &_device_id, NULL);
		_context = clCreateContext(ctx_props, 1, &_device_id, NULL, NULL, err + 1);
		_queue = clCreateCommandQueue(_context, _device_id, 0, err + 2);
		contextLoaded = true;
	}
	_firstUse = true;
	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
	bzero(&(_param), sizeof(t_ifs_param));
	_size_buff = MAX_GPU_BUFF;
//	_param.nb_iter = HARD_ITER;
//	_param.len_trans = 4;
	_param.len_trans = 6;//sizeof(anime_trans) / (sizeof(float) * 8);
	_param.len_base = 2;
	_Ajust_iter();
	_prev_anime = -1;
	_need_update = true;

	_SetRangeVal(&(_param.hue), 0., 0.9);
	_SetRangeVal(&(_param.sat), 0.3, 0.8);
	_SetRangeVal(&(_param.val), 0.3, 0.8);



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
	///////////////		OpenGL part	///////////////////
	GLuint		fragmentShaderId;
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
	return (_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__));
}

void		KernelProgram::Use(void)
{
	if (_firstUse)
		__localParams->at("localStartTime") = glfwGetTime(), _firstUse = false;
	glUseProgram(_id);
}


void			KernelProgram::CreateVAO(void)
{
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
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
	int		id_anime;
	cl_int	err[4];

	(void)pass;
	bzero(err, sizeof(err));

	float time = glfwGetTime() - __localParams->at("localStartTime");
	id_anime = ((int)(time / 1)) % 9;

	id_anime = 8;
//	id_anime = __localParams["idAnime"];
	if (id_anime != _prev_anime)
	{
		_prev_anime = id_anime;
		_need_update = true;
	}
	SetParamAnime(id_anime);
	setParam(&_param);
//	_Set_base();



	err[0] = clEnqueueWriteBuffer(_queue, _buff["pt_ifs"], CL_TRUE, 0, 4 * 2 * sizeof(float), &_param.pt_base, 0, NULL, NULL);
	err[1] = clEnqueueWriteBuffer(_queue, _buff["ifs_param"], CL_TRUE, 0, sizeof(t_ifs_param), &_param, 0, NULL, NULL);
	global_work_size[0] = _idPtBuff[_param.nb_iter - 1] - _idPtBuff[_param.nb_iter - 2];
//	std::cout << "work_size:" << global_work_size[0] << std::endl;
	if (_need_update)
	{
		err[2] = clEnqueueNDRangeKernel(_queue, _kernels["define_color"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
		_need_update = false;
	}
	if (!_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__))
	{
		exit(0);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _screen_tex);
	int id = glGetUniformLocation(_id, "iChannel0");
	glUniform1i(id, _screen_tex);
	id = glGetUniformLocation(_id, "iResolution");
	glUniform1f(glGetUniformLocation(_id, "iGlobalTime"), glfwGetTime() - __localParams->at("localStartTime"));
	glUniform2f(id, winSize.x, winSize.y);
}

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

	_buff["screen"] = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _screen_tex, 0);
	
	err[0]	= clSetKernelArg(_kernels["draw_line"], 0, sizeof(cl_mem), &_buff["screen"]);
	err[1] = clSetKernelArg(_kernels["clear"], 0, sizeof(cl_mem), &_buff["screen"]);

	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
}

void		KernelProgram::Draw(void)
{
	// -------- on lance le calcul des point
	cl_int		err[3];
	size_t		screen[2];
	size_t		global_work_size[3] = {1, 0, 0};
	size_t		local_work_size[3] = {1, 0, 0};
	int	i;

	_SetIdPtBuff();
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
		err[2] = clEnqueueNDRangeKernel(_queue, _kernels["draw_line"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	}
	clEnqueueReleaseGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	clFinish(_queue);
	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);
//	#------		On affiche le buffer avec OpenGL	------#

#ifdef DEBUG
	std::cout << "drawing program: " << _id << " to " << _vao << "\n";
#endif
	glBindVertexArray(_vao);
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

void		KernelProgram::_SetRangeVal(t_range *value, float beg, float end)
{
	value->beg = beg;
	value->end = end;
	value->delta = end - beg;
}

void		KernelProgram::_SetIdPtBuff()
{
//	(void)nb_iter;
	int		sum;
	int		i;

	i = 1;
	_idPtBuff[0] = 0;
	sum = _param.len_base;
	while (i < MAX_ITER)
	{
		sum += (sum - 1) * (_param.len_trans - 2);// le truc presedent
		_idPtBuff[i] = _idPtBuff[i - 1] + sum;
		i++;
	}
}

void	set_trans_ovaloid(t_ifs_param *param, float time)
{
	int				i;
	vec_2	beg, ux, uy, ret;
	float	speed, offset;

//	std::cout << "len_trans:" << param->len_trans << std::endl;
//	param->len_trans = sizeof(anime_trans) / (sizeof(float) * 8);
	for (i = 0; i < param->len_trans; i++)
	{
//		std::cout << "i:" << i << std::endl;
		beg.x = anime_trans[i][0];
		beg.y = anime_trans[i][1];
		ux.x = anime_trans[i][2];
		ux.y = anime_trans[i][3];
		uy.x = anime_trans[i][4];
		uy.y = anime_trans[i][5];
		speed = anime_trans[i][6] / 8;
		offset = anime_trans[i][7];
		ret =  add_rot(beg, ux, uy, 1, time, speed, offset);
		param->pt_trans[i][0] = ret.x;
		param->pt_trans[i][1] = ret.y;
	}
}

void	set_trans_ovaloid2(t_ifs_param *param, float time, float trans[][8], int size)
{
	int				i;
	vec_2	beg, ux, uy, ret;
	float	speed, offset;

	param->len_trans = size;
//	std::cout << "len_trans:" << param->len_trans << std::endl;
//	param->len_trans = sizeof(anime_trans) / (sizeof(float) * 8);
	for (i = 0; i < param->len_trans; i++)
	{
//		std::cout << "i:" << i << std::endl;
		beg.x = trans[i][0];
		beg.y = trans[i][1];
		ux.x = trans[i][2];
		ux.y = trans[i][3];
		uy.x = trans[i][4];
		uy.y = trans[i][5];
		speed = trans[i][6] / 5;
		offset = trans[i][7];
		ret =  add_rot(beg, ux, uy, 1, time, speed, offset);
		param->pt_trans[i][0] = ret.x;
		param->pt_trans[i][1] = ret.y;
	}
}




void	KernelProgram::setParam(t_ifs_param *param)
{
	(void)param;
	float time = glfwGetTime() - __localParams->at("localStartTime");

//	set_trans_raw(param);
	set_trans_ovaloid(param, time);
//	set_trans_ovaloid2(&_param, time, anime_trans, 6)

	_param.max_pt = _idPtBuff[_param.nb_iter] - _idPtBuff[_param.nb_iter - 1]; 
	_param.ecr_x = framebuffer_size.x;
	_param.ecr_y = framebuffer_size.y;
	

	memmove(&(param->beg_id), _idPtBuff, sizeof(int) * MAX_ITER);
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

void	KernelProgram::_Set_base()
{
	float	r;
	float	time = glfwGetTime() - __localParams->at("localStartTime");
	time /= 4;
	vec_2	beg = {framebuffer_size.x / 2, framebuffer_size.y / 2};
	vec_2	ux = {0, 1};
	vec_2	uy = {1, 0};
	vec_2	base[MAX_NODE];
	r = 400 + 20 * sin(time);
	bzero(base, sizeof(base));

	_param.len_base = 3;
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
	for (int i = 0; i < MAX_NODE; i++)
	{
		_param.pt_base[i][0] = base[i].x;
		_param.pt_base[i][1] = base[i].y;
	}
}

void	KernelProgram::_SetBaseFix()
{
	vec_2	beg = {framebuffer_size.x / 2, framebuffer_size.y / 2};
//	vec_2	ux = {0, 1};
//	vec_2	uy = {1, 0};
	vec_2	base[MAX_NODE];

	bzero(base, sizeof(base));
	_param.len_base = 2;
	base[0].x = beg.x;
	base[0].y = beg.y * 3.0 / 5.0;
	base[1].x = beg.x;
	base[1].y = beg.y * 7.0 / 5.0;
	base[2] = base[1];
	base[3] = base[0];
	for (int i = 0; i < MAX_NODE; i++)
	{
		_param.pt_base[i][0] = base[i].x;
		_param.pt_base[i][1] = base[i].y;
	}
}

void								KernelProgram::_SetBasefeuille()
{
	float	r;
	float	time = glfwGetTime() - __localParams->at("localStartTime");
	time /= 4;
	vec_2	beg = {framebuffer_size.x / 2, framebuffer_size.y / 2};
	vec_2	ux = {0, 1};
	vec_2	uy = {1, 0};
	vec_2	base[MAX_NODE];
	r = 250 + 20 * sin(time) * cos(time);
	bzero(base, sizeof(base));

	_param.len_base = 2;
	base[0] = add_rot(beg, ux, uy, r, time, -0.05 * cos(time) - 0.1, 0);
	base[0] = add_rot(base[0], ux, uy, 100, time, 0.1, 0);
	base[0] = add_rot(base[0], ux, uy, 30, time, -0.2, 0);
	base[0] = add_rot(base[0], ux, uy, 10, time, 0.3, 0);

	base[1] = add_rot(beg, ux, uy, r, time, 0.05 * cos(time) + 0.2, 0.5);
	base[1] = add_rot(base[1], ux, uy, 130, time, -0.1, 0);
	base[1] = add_rot(base[1], ux, uy, 30, time, 0.2, 0);
	base[1] = add_rot(base[1], ux, uy, 10, time, -0.5, 0);

	for (int i = 0; i < MAX_NODE; i++)
	{
		_param.pt_base[i][0] = base[i].x;
		_param.pt_base[i][1] = base[i].y;
	}
}

void	gl_test(const int line, const char * func, const char * file)
{
	GLenum gl_err = 0;

	while((gl_err = glGetError()) != GL_NO_ERROR)
	{
		//Process/log the error.
		std::cout << "err code:"<< gl_err << "	file:" << file << "	func:" << func << "	line:"<< line << std::endl;
//		exit(0);
	}
}

void	KernelProgram::SetParamAnime(int id)
{
	std::cout << "anime:" << id << std::endl;
	switch (id)
	{
		case 0:
			_Set_base();
			_SetRangeVal(&(_param.hue), 0, 0.4);
			_param.color = 0.45;
			_param.len_trans = sizeof(tr1) / (sizeof(float) * 8);
			memmove(anime_trans, tr1, sizeof(tr1));
			break;
		case 1:
			_SetBaseFix();
			_SetRangeVal(&(_param.hue), 0.1, 0.4);
			_param.color = 0.45;
			_param.len_trans = sizeof(tr2) / (sizeof(float) * 8);
			memmove(anime_trans, tr2, sizeof(tr2));
			break;
		case 2:
			_Set_base();
			_SetRangeVal(&(_param.hue), 0.3, 0.49);
			_param.color = 0.9;
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			memmove(anime_trans, tr3, sizeof(tr3));
			break;
		case 3:
			_SetBaseFix();
			_SetRangeVal(&(_param.hue), 0.6, 0.8);
			_param.color = 0.9;
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			_param.len_trans = sizeof(tr4) / (sizeof(float) * 8);
			memmove(anime_trans, tr4, sizeof(tr4));
			break;
		case 4:
			_Set_base();
			_SetRangeVal(&(_param.hue), 0.6, 0.99);
			_param.len_trans = sizeof(tr5) / (sizeof(float) * 8);
			memmove(anime_trans, tr5, sizeof(tr5));
			break;
		case 5:
//			_param.color = 0.9;
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			_param.len_trans = sizeof(tr6) / (sizeof(float) * 8);
			memmove(anime_trans, tr6, sizeof(tr6));
			break;
		case 6:
//			_param.color = 0.9;
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			_param.len_trans = sizeof(tr7) / (sizeof(float) * 8);
			memmove(anime_trans, tr7, sizeof(tr7));
			break;
		case 7:
//			_param.color = 0.9;
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			_param.len_trans = sizeof(tr8) / (sizeof(float) * 8);
			memmove(anime_trans, tr8, sizeof(tr8));
			break;
		case 8:
			_param.color = 0.7;
			_SetRangeVal(&(_param.hue), 0.6, 0.99);
			_param.len_trans = sizeof(tr3) / (sizeof(float) * 8);
			_SetBasefeuille();
//			_SetBaseFix();
			_param.len_trans = sizeof(tr9) / (sizeof(float) * 8);
			memmove(anime_trans, tr9, sizeof(tr9));
			break;
		default:
			_param.len_trans = sizeof(tr1) / (sizeof(float) * 8);
			memmove(anime_trans, tr1, sizeof(tr1));
			break;
	}
	_Ajust_iter();
	_SetIdPtBuff();
}


void								KernelProgram::_Ajust_iter()
{
	int	i;
	bool	again = true;

	_SetIdPtBuff();
	for (i = 2; i <  MAX_ITER && again; i++)
	{
		if (((_idPtBuff[i]) * (2 * sizeof(float) * _param.len_base)) > _size_buff)
		{
			_param.nb_iter = i - 1;
			again = false;
		}
	}
}

