/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/09 12:36:31 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/20 01:28:33 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <algorithm>

struct MatchPathSeparator
{
	bool operator()( char ch ) const
	{
		return ch == '/';
	}
};


bool		CheckFileExtension(const char *file, const char **exts)
{
	const char	*ext = file + strlen(file) - 1;

	while (ext != file && *ext != '.')
		ext--;
	ext++;
	while (*exts)
	{
		if (!strcmp(ext, *exts))
			return (1);
		exts++;
	}
	return (0);
}

std::string	basename( std::string const & pathname )
{
	return std::string( 
			std::find_if( pathname.rbegin(), pathname.rend(),
				MatchPathSeparator() ).base(),
			pathname.end() );
}

