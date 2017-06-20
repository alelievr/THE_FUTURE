#ifndef KERNELPROGRAM_HPP
# define KERNELPROGRAM_HPP
# include <iostream>
# include <string>
# include "shaderpixel.h"
# include "ICGProgram.hpp"

class		KernelProgram : public ICGProgram
{
	private:
		bool			_firstUse;

	public:
		KernelProgram(void);
		KernelProgram(const KernelProgram&) = delete;
		virtual ~KernelProgram(void);

		KernelProgram &	operator=(KernelProgram const & src) = delete;

		bool	LoadSourceFile(const std::string & fileName);
		bool	CompileAndLink(void);
		void	Use(void);
		void	UpdateUniforms(const vec2 winSize, bool pass = false);
		void	UpdateFramebufferSize(const vec2 fbSize);
		void	Draw(void);
};

std::ostream &	operator<<(std::ostream & o, KernelProgram const & r);

#endif
