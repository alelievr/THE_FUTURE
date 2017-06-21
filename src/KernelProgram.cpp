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

#define TRANS_TEST 128
static	float	trans_raw[TRANS_TEST][4][2] = {
						{{0.000000, 0.000000},{0.635547, -0.700717},{0.497671, -0.047698},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.632301, -0.705343},{0.498261, -0.055883},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.628625, -0.709354},{0.499492, -0.064428},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.624500, -0.712602},{0.502829, -0.078653},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.620572, -0.714682},{0.505140, -0.086400},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.616174, -0.716030},{0.508161, -0.094419},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.610507, -0.716476},{0.512607, -0.103850},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.605804, -0.715903},{0.516688, -0.110991},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.601543, -0.714738},{0.520644, -0.116973},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.589454, -0.708508},{0.525849, -0.123808},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.552019, -0.704856},{0.531664, -0.130383},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.515712, -0.696356},{0.538711, -0.137191},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.494961, -0.684888},{0.552370, -0.147664},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.481361, -0.669327},{0.568687, -0.156634},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.469257, -0.652953},{0.583624, -0.162199},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.456048, -0.632641},{0.600124, -0.165852},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.444431, -0.612817},{0.614636, -0.167087},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.431905, -0.589485},{0.630123, -0.166435},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.426014, -0.577830},{0.637303, -0.165437},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.410929, -0.546023},{0.655247, -0.160913},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.399228, -0.519408},{0.668588, -0.155490},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.392276, -0.502778},{0.676216, -0.151488},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.384232, -0.482761},{0.684714, -0.146136},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.371881, -0.450345},{0.696971, -0.136414},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.366094, -0.434429},{0.702335, -0.131233},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.353310, -0.397504},{0.713181, -0.118357},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.341748, -0.361836},{0.721602, -0.104990},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.330590, -0.325105},{0.728216, -0.090495},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.319751, -0.286905},{0.732915, -0.074850},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.314728, -0.268232},{0.734408, -0.067053},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.310216, -0.250863},{0.735324, -0.059738},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.304387, -0.227487},{0.735836, -0.049833},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.311041, -0.256407},{0.734786, -0.032917},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.305594, -0.229550},{0.731287, -0.016159},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.322235, -0.264583},{0.728398, -0.007365},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.326028, -0.256324},{0.721563, 0.007688},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.325805, -0.240756},{0.714018, 0.019818},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.325880, -0.232812},{0.709613, 0.025640},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.326552, -0.214374},{0.698008, 0.038089},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.327156, -0.205123},{0.691480, 0.043724},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.328080, -0.194467},{0.683396, 0.049659},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.330009, -0.178001},{0.669750, 0.057552},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.331287, -0.169303},{0.661992, 0.061034},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.332751, -0.160621},{0.653879, 0.063991},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.334383, -0.152098},{0.645562, 0.066351},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.336539, -0.142190},{0.635465, 0.068361},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.338780, -0.133105},{0.625808, 0.069448},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.340918, -0.125326},{0.617243, 0.069746},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.343209, -0.117764},{0.608659, 0.069422},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.345658, -0.110413},{0.600075, 0.068471},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.348209, -0.103425},{0.591701, 0.066922},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.350563, -0.097503},{0.584444, 0.065066},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.353300, -0.091167},{0.576517, 0.062463},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.356243, -0.084934},{0.568560, 0.059203},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.359793, -0.078120},{0.559686, 0.054727},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.363016, -0.072527},{0.552267, 0.050222},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.366295, -0.067355},{0.545302, 0.045265},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.369698, -0.062491},{0.538664, 0.039776},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.373821, -0.057227},{0.531389, 0.032720},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.380994, -0.049551},{0.520627, 0.019576},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.375419, 0.053154},{0.509721, 0.084999},{1.000000, 0.000000}},
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
						{{0.000000, 0.000000},{0.332751, -0.160621},{0.653879, 0.063991},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.331287, -0.169303},{0.661992, 0.061034},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.330009, -0.178001},{0.669750, 0.057552},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.328080, -0.194467},{0.683396, 0.049659},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.327156, -0.205123},{0.691480, 0.043724},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.326552, -0.214374},{0.698008, 0.038089},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.325880, -0.232812},{0.709613, 0.025640},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.325805, -0.240756},{0.714018, 0.019818},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.326028, -0.256324},{0.721563, 0.007688},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.322235, -0.264583},{0.728398, -0.007365},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.305594, -0.229550},{0.731287, -0.016159},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.311041, -0.256407},{0.734786, -0.032917},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.304387, -0.227487},{0.735836, -0.049833},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.310216, -0.250863},{0.735324, -0.059738},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.314728, -0.268232},{0.734408, -0.067053},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.319751, -0.286905},{0.732915, -0.074850},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.330590, -0.325105},{0.728216, -0.090495},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.341748, -0.361836},{0.721602, -0.104990},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.353310, -0.397504},{0.713181, -0.118357},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.366094, -0.434429},{0.702335, -0.131233},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.371881, -0.450345},{0.696971, -0.136414},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.384232, -0.482761},{0.684714, -0.146136},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.392276, -0.502778},{0.676216, -0.151488},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.399228, -0.519408},{0.668588, -0.155490},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.410929, -0.546023},{0.655247, -0.160913},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.426014, -0.577830},{0.637303, -0.165437},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.431905, -0.589485},{0.630123, -0.166435},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.444431, -0.612817},{0.614636, -0.167087},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.456048, -0.632641},{0.600124, -0.165852},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.469257, -0.652953},{0.583624, -0.162199},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.481361, -0.669327},{0.568687, -0.156634},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.494961, -0.684888},{0.552370, -0.147664},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.515712, -0.696356},{0.538711, -0.137191},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.552019, -0.704856},{0.531664, -0.130383},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.589454, -0.708508},{0.525849, -0.123808},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.601543, -0.714738},{0.520644, -0.116973},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.605804, -0.715903},{0.516688, -0.110991},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.610507, -0.716476},{0.512607, -0.103850},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.616174, -0.716030},{0.508161, -0.094419},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.620572, -0.714682},{0.505140, -0.086400},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.624500, -0.712602},{0.502829, -0.078653},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.628625, -0.709354},{0.499492, -0.064428},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.632301, -0.705343},{0.498261, -0.055883},{1.000000, 0.000000}},
						{{0.000000, 0.000000},{0.635547, -0.700717},{0.497671, -0.047698},{1.000000, 0.000000}},
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
"	uv *= iResolution.x / iResolution.y;\n"
"	fragColor = texture(iChannel0, uv);\n"
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
	-.75f, -.75f,
	-.75f,  .75f,
	.75f,  .75f,
	.75f,  .75f,
	.75f, -.75f,
	-.75f, -.75f,
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
	int				mode;
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

	/////////////////////////////////
	const char * 			src = _cl_source.c_str();
	size_t					src_size = _cl_source.length();
	cl_int					err[19];

	_program = clCreateProgramWithSource(_context, 1, &src, &src_size, err + 0);
    err[18] = clBuildProgram(_program, 1, &_device_id, NULL, NULL, NULL);
	std::cout << "-----------" << (err[0]) << std::endl;
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

   /** Create a texture to be displayed as the final image. */
    glGenTextures(1, &_screen_tex);
    glBindTexture(GL_TEXTURE_2D, _screen_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_W, WIN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _buff["screen"] = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _screen_tex, 0);

	_buff["pt_ifs"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, MAX_GPU_BUFF, NULL, err + 4);
	_buff["col_pt"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, sizeof(t_ifs_param), NULL, err + 5);
	_buff["ifs_param"] = clCreateBuffer(_context, CL_MEM_READ_WRITE, MAX_GPU_BUFF / 2, NULL, err + 6);

	err[7] = clEnqueueAcquireGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
    {
		err[8]	= clSetKernelArg(_kernels["draw_line"], 0, sizeof(cl_mem), &_buff["screen"]);
    }
	err[9] = clEnqueueReleaseGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	err[10] = clFlush(_queue);
	
	err[11] = clSetKernelArg(_kernels["define_color"], 0, sizeof(cl_mem), &_buff["col_pt"]);
	err[12] = clSetKernelArg(_kernels["define_color"], 1, sizeof(cl_mem), &_buff["ifs_param"]);

	err[13] = clSetKernelArg(_kernels["calcul_ifs_point"], 0, sizeof(cl_mem), &_buff["pt_ifs"]);
	err[14] = clSetKernelArg(_kernels["calcul_ifs_point"], 1, sizeof(cl_mem), &_buff["ifs_param"]);
	
	err[15] = clSetKernelArg(_kernels["draw_line"], 1, sizeof(cl_mem), &_buff["pt_ifs"]);
	err[16] = clSetKernelArg(_kernels["draw_line"], 2, sizeof(cl_mem), &_buff["col_pt"]);
	err[17] = clSetKernelArg(_kernels["draw_line"], 3, sizeof(cl_mem), &_buff["ifs_param"]);

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
	cl_int	err[4];
	float	hard_base[4][2] =
	{{657.534241, 341.694916},{250.684937, 919.322021},{1035.616455, 1004.745789},{661.643860, 341.694916}};

	err[0] = clEnqueueWriteBuffer(_queue, _buff["pt_ifs"], CL_TRUE, 0, 4 * 2 * sizeof(float), hard_base, 0, NULL, NULL);
	_setIdPtBuff(4, 2, HARD_ITER, _idPtBuff);
	setParam(&_param);
	err[1] = clEnqueueWriteBuffer(_queue, _buff["ifs_param"], CL_TRUE, 0, sizeof(t_ifs_param), &_param, 0, NULL, NULL);

	int id = glGetUniformLocation(_id, "iChannel0");
	glUniform1i(id, _screen_tex);
}

/*
//////////////////	From ShaderProgram	\\\\\\\\\\\\\\\\\\

void	ShaderProgram::UpdateFramebufferSize(const vec2 fbSize)
{
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
		if (_channels[i].getType() == ShaderChannelType::CHANNEL_PROGRAM)
		{
			auto program = _channels[i].getProgram();
			glTexImage2D(program->GetRenderId(), 0, GL_RGB, framebuffer_size.x, framebuffer_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			if (program != this)
				program->UpdateFramebufferSize(fbSize);
		}
}
*/

// <---
void		KernelProgram::UpdateFramebufferSize(const vec2 fbSize)
{
}

void		KernelProgram::Draw(void)
{
	// -------- on lance le calcul des point
	cl_int		err[3];
	size_t		global_work_size[3] = {1, 0, 0};
	size_t		local_work_size[3] = {1, 0, 0};
	int	i;

	i = 1;
	while (i < HARD_ITER)
	{
		global_work_size[0] = _idPtBuff[i + 1] - _idPtBuff[i];

		err[0] = clSetKernelArg(_kernels["calcul_ifs_point"], 2, sizeof(int), &i);
		err[1] = clEnqueueNDRangeKernel(_queue, _kernels["calcul_ifs_point"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
		i++;
	}

	//////////

//	#------		On draw les ligne dans le buffer partager	------#


	clEnqueueAcquireGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	{
		global_work_size[0] = _idPtBuff[HARD_ITER - 1] - _idPtBuff[HARD_ITER - 2] - 1;
		err[2] = clEnqueueNDRangeKernel(_queue, _kernels["draw_line"], 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	}
	clEnqueueReleaseGLObjects(_queue, 1, &_buff["screen"], 0, NULL, NULL);
	clFlush(_queue);

	_check_err_tab(err, sizeof(err) / sizeof(cl_int), __func__, __FILE__);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (_renderId != -1)
	{
		GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffers);
	}
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
	int		sum;
	int		iter_max;
	int		i;

	i = 1;
	indice_beg[0] = 0;
	sum = nb_base;
	while (i < MAX_ITER)
	{
		sum += (sum - 1) * (nb_trans);// le truc presedent
		indice_beg[i] = indice_beg[i - 1] + sum;
		i++;
	}
}

void	KernelProgram::setParam(t_ifs_param *spec)
{
	static		int id_trans = 0;
	int			i;


	for (i = 0; i < 4; i++)
	{
		spec->pt_trans[i][0] = trans_raw[id_trans][i][0];
		spec->pt_trans[i][1] = trans_raw[id_trans][i][1];
	}
	id_trans = (id_trans + 1) % TRANS_TEST;
	spec->len_trans = 2;


	spec->len_base = 4;//get_polygone_len(e->base);

	spec->max_iter = 4;
	_setIdPtBuff(spec->len_base, spec->len_trans, spec->max_iter, _idPtBuff);

	spec->max_pt = _idPtBuff[HARD_ITER] - _idPtBuff[HARD_ITER - 1]; 
	spec->ecr_x = WIN_W;
	spec->ecr_y = WIN_H;
	spec->nb_iter = HARD_ITER;

	_setRangeVal(&(spec->hue), 0, 1);
	_setRangeVal(&(spec->sat), 0, 1);
	_setRangeVal(&(spec->val), 0, 1);

	memmove(&(spec->beg_id), _idPtBuff, sizeof(int) * MAX_ITER);
//	printf("spec->ecrX:%d	spec->ecrY:%d\n", spec->dim_ecr[0], spec->dim_ecr[1]);
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
