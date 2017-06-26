#pragma once
#include <map>

class		ICGProgram
{
	protected:
		std::map< std::string, float >	*__localParams;

	public:
		ICGProgram() {
			__localParams = new std::map< std::string, float >();
			__localParams->insert(std::pair< std::string, float >("localStartTime", glfwGetTime()));
		}
		virtual bool	LoadSourceFile(const std::string & fileName) = 0;

		virtual bool	CompileAndLink(void) = 0;

		virtual void	UpdateLocalParam(const std::string & name, const float value, const bool reset = false)
		{
			if (name == "localStartTime" && reset)
				__localParams->at(name) = glfwGetTime();
			else
				__localParams->at(name) = value;
		}

		virtual void	Use(void) = 0;
		virtual void	UpdateUniforms(const vec2 winSize, bool pass = false) = 0;
		virtual void	UpdateFramebufferSize(const vec2 fbSize) = 0;
		virtual void	Draw(void) = 0;

		virtual ~ICGProgram() {
			delete __localParams;
		}
};
