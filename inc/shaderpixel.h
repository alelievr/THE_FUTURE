/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/11 18:11:58 by alelievr          #+#    #+#             */
/*   Updated  2016/07/25 19:04:51 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <iostream>
# define GL_GLEXT_PROTOTYPES
# define GLFW_INCLUDE_GLCOREARB
# include "GLFW/glfw3.h"
# include "SOIL2.h"

# define CLUSTER_MAX_ROWS		13
# define CLUSTER_MAX_ROW_SEATS	23

# if __APPLE__
#  include <OpenGL/glext.h>
# else
#  include <bsd/string.h>
# endif

# define unused __attribute__((unused))

# define STRESS_TEST_PACKET_COUNT	8192
# define MAX_CHANNEL_COUNT	8
# define WIN_W				1920
# define WIN_H				1080
# define SCALE				70

typedef struct s_vec2
{
	float x;
	float y;
}				vec2;

typedef struct s_vec3
{
	float x;
	float y;
	float z;
}				vec3;

typedef struct s_vec4
{
	float x;
	float y;
	float z;
	float w;
}				vec4;

typedef struct	s_riff_header
{
	int			:32;				//0
	int			chunk_size;			//4
	char		format[4];			//8
	int			:32;				//12
	int			sub_chunk1_size;	//16
	short		audio_format;		//20
	short		num_channels;		//22
	int			sample_rate;		//24
	int			byte_rate;			//28
	short		block_align;		//32
	short		bit_per_sample;		//34
}				riff_header;

enum			e_channel_type
{
	CHAN_NONE,
	CHAN_IMAGE,
	CHAN_SOUND,
	CHAN_SHADER,
};

enum			e_channel_mode
{
	CHAN_NEAREST			= 0x01,
	CHAN_LINEAR				= 0x02,
	CHAN_MIPMAP				= 0x04,
	CHAN_VFLIP				= 0x08,
//	CHAN_CLAMP_BORDER		= 0x10,
//	CHAN_CLAMP_EDGE			= 0x20,
	CHAN_CLAMP				= 0x40,
};

enum			KEY_BITS
{
	RIGHT,
	LEFT,
	UP,
	DOWN,
	FORWARD,
	BACK,
	PLUS,
	MOIN,
};

enum			SOUND_FORMAT
{
	WAVE,
};

typedef struct	s_sound
{
	int				id;
	int				fd;
	int				tex_length;
	GLint			gl_id;
	char			*image_buffer;
	enum SOUND_FORMAT	sound_format;
	riff_header	riff;
}				t_sound;

#define BIT_SET(i, pos, v) (v) ? (i |= 1 << pos) : (i &= ~(1 << pos))
#define BIT_GET(i, pos) (i >> pos) & 1
#define MOVE_AMOUNT 0.01f;

extern vec2			framebuffer_size;

//UTILS
bool			CheckFileExtension(const char *filename, const char **exts);
std::string		basename( std::string const & pathname );

//SOUND LOAD
int				load_wav_file(const char *f);

//SOUND CONTROL
GLuint			get_sound_texture(int id);

#endif
