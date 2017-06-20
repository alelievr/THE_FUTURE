#include "KernelProgram.hpp"

static bool		contextLoaded = false;

KernelProgram::KernelProgram(void)
{
	if (!contextLoaded)
	{
		//load OpenCL contex
		contextLoaded = true;
	}
	_firstUse = true;
}

KernelProgram::~KernelProgram(void)
{
}

bool		KernelProgram::LoadSourceFile(const std::string & fileName)
{
	return false;
}

bool		KernelProgram::CompileAndLink(void)
{
	return false;
}

void		KernelProgram::Use(void)
{
	if (_firstUse)
		__localParams["localStartTime"] = glfwGetTime(), _firstUse = false;
}

void		KernelProgram::UpdateUniforms(const vec2 winSize, bool pass)
{
	
}

void		KernelProgram::UpdateFramebufferSize(const vec2 fbSize)
{
	
}

void		KernelProgram::Draw(void)
{
	
}
