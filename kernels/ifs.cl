/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_parallel.cl                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fjanoty <fjanoty@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/11/26 07:31:16 by fjanoty           #+#    #+#             */
/*   Updated: 2016/12/14 07:07:45 by fjanoty          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define MAX_ITER 	80
#define MAX_NODE	16

# define WIN_W		1920
# define WIN_H		1080

# define PART_ATR	10
typedef	struct	s_range
{
	float		beg;
	float		end;
	float		delta;
	float		none;
}				t_range;

typedef	struct	s_ifs_spec
{
	float2	pt_base[MAX_NODE];
	float2	pt_trans[MAX_NODE];
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
}				t_ifs_spec;


typedef	struct	s_particule
{
	float2	pos;
	float2	speed;
	char	alive;
}				t_particule;

__kernel	void	particles(write_only image2d_t img
									   , __global float2 *atr
									   , __global t_particule *part
									   , __global t_ifs_spec *spec)
{
	int		id_part;
	int		id_atr;
	float2	acc, pos, diff;
	float	dist;
	float4	col;

	id_part = get_global_id(0);
	id_atr = id_part / spec->nb_part;
	if (part[id_part].alive)
	{
		col = (float4)(1, 1, 1, 1);
		diff = (atr[id_atr] - part[id_part].pos);
		acc = diff / (dot(diff, diff) + 100);
		
		part[id_part].pos += part[id_part].speed;
		part[id_part].speed += acc;
		int2 wpos;
		wpos.x = (int)part[id_part].pos.x;
		wpos.y = (int)part[id_part].pos.y;
		write_imagef(img, wpos, col);
	}
	else
	{
		part[id_part].pos = (float)(atr[id_atr].x + id_part % spec->nb_part, atr[id_atr].y + id_part % spec->nb_part);
		part[id_part].speed = (float2)(0, 0);
	}
}



__kernel void test_image(__global int *time, __global int *data)
{
	int	id = get_global_id(0);
	int	val;
	
	val = (id + *time) % 256;
	data[id] = ((val * 7)% 256) << 16 | (256 - val) << 8 | abs(128 - val) * 2;
}
uchar4	hsv_to_rgb(__const float hue, __const float sat, __const float val);
int		get_iter(int id, __const char max_iter, __const char len_trans, __const char len_base);
float	get_line_length(float2 p1, float2 p2);
float	rand(int gid, int lid);
void	print_spec(t_ifs_spec *spec);

uchar4	hsv_to_rgb(__const float hue, __const float sat, __const float val)
{
	float	c, x, m, rgb[3];
	char	id_x, id_c, id_0, i;

	c = val * sat;
	x = c * (1 - fabs(fmod(hue / 60.0, 2) - 1));
	m = val - c;
	i = val / 60;
	id_x = (7 - i) % 3;
	id_0 = (2 + (i >> 1)) % 3;
	id_c = 3 - (id_x + id_0);
	rgb[id_x] = (x + m) * 255;
	rgb[id_c] = (c + m) * 255;
	rgb[id_0] = (0 + m) * 255;
	return ((uchar4)(rgb[0], rgb[1], rgb[2], 0));
}

int	get_iter(int id, __const char max_iter, __const char len_trans, __const char len_base)
{
	char	iter;

	iter = 0;
	id /= (len_base - 1);
	while ((id % (len_trans - 1)) == 0 && iter < max_iter)
	{
		id = id / (len_trans - 1);
		iter++;
	}
	return (iter);
}


__kernel	void	define_color(__global uchar4 *col, __global t_ifs_spec *spec)
{
	// puis appeler gentiement la focnitn qui donne les couleurs
	float	hue, sat, val;
//	float	ok;
	int	id, iter;

	id = get_global_id(0);

	iter = get_iter(id, spec[0].nb_iter, spec[0].len_trans, spec[0].len_base);

	hue = ((float) id) / ((float) spec[0].max_pt);
//	sat = (1 - (float)iter / ((float) spec[0].nb_iter));
//	val = (float)(id % (spec->len_trans - 1)) / ((float) (spec->len_trans - 1));
	sat = val * val;
//printf("val_color:%f\n",  spec->color);
	hue = (spec->hue.beg + (hue * spec->hue.delta)) * 360;
//	sat = spec->sat.beg + (sat * spec->sat.delta);
//	val = spec->val.beg + (val * spec->val.delta);
	col[id] = hsv_to_rgb(hue, 0.8, spec->color);
//printf("id:%d\n", id);
}

float	get_line_length(float2 p1, float2 p2)
{
	int		id_max;
	int		max_p[2];

	max_p[0] = p1.x - p2.x;
	max_p[1] = p1.y - p2.y;
	max_p[0] *= (max_p[0] > 0) ? 1 : -1; 
	max_p[1] *= (max_p[1] > 0) ? 1 : -1; 
	id_max = (max_p[0] > max_p[1]) ? 0 : 1;

	return (max_p[id_max]);
}

float		rand(int gid, int lid)
{
	uint	seed = lid + lid * gid % 4242;
	return (float)((seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) / (float)50;
}

#define POINT

__kernel	void	draw_line(write_only image2d_t img
									   , __global float2 *pt
									   , __global char4 *col
									   , __global t_ifs_spec *spec)
{
	float2	p;
	int		id, id_col;
	float4	c;
	bool	is_inside;
	int		w = spec->ecr_x;
	int		h = spec->ecr_y;

	id_col = get_global_id(0);
	id = id_col + spec->beg_id[spec->nb_iter - 1];
	p = pt[id];
	c = (float4)(col[id_col].x, col[id_col].y, col[id_col].z, 0);
	c /= (float)64;

#ifdef POINT
	is_inside = (p.x >= 0 && p.x < w) && (p.y >= 0 && p.y < h);	
	if (is_inside)
		write_imagef(img, (int2){p.x, p.y}, c);
#else
	write_imagef(img, )
	char4	dc;
	float2	unit_pos;
	float4	unit_col;
	int		indice;
	int		col_value;
	float	dist;
	int		nb_point;
	int		i;
	int		gid = get_global_id(0);
	int		lid = get_local_id(0);

	dc = col[id + 1] - col[id];
	dist = get_line_length(pt[id], pt[id + 1]);
	nb_point = dist;
	unit_pos = (pt[id + 1] - pt[id]) / dist;
	unit_col = (float4)(dc.x, dc.y, dc.z, 0) / dist;
	i = 0;
	unit_col /= (float)128;

	while (i <= nb_point)
	{
		is_inside = (p.x >= 0 && p.x < w) && (p.y >= 0 && p.y < h);	
		if (is_inside)
			write_imagef(img, (int2){p.x, p.y}, c);

		p += unit_pos;
		c += unit_col + float4(rand(gid, lid), rand(gid + 11, lid - 11), rand(gid + 101, lid - 101));
		gid++;
		lid--;
		i++;
	}
#endif
}

__kernel	void	calcul_ifs_point(__global float2 *pt_ifs
									, __global t_ifs_spec *spec
									, __const int num_iter)
{
	int		glob_id;
	float2	ux;
	float2	uy;
	int		id_trans;
	int		id_parent;
	int		id_now;

	glob_id = get_global_id(0);
	id_trans = glob_id % (spec->len_trans - 1);
	id_parent = (glob_id / (spec->len_trans - 1)) + spec->beg_id[num_iter - 1];
	id_now = glob_id + spec->beg_id[num_iter];

	ux = (pt_ifs[id_parent + 1] - pt_ifs[id_parent]) * 1.3f;
	uy = (float2)(-ux.y, ux.x);

	pt_ifs[id_now] = pt_ifs[id_parent] + spec->pt_trans[id_trans].x * ux + spec->pt_trans[id_trans].y * uy;
	//printf("glob_id:%d	dad[%d]:{%f, %f}	\n", glob_id, id_parent, pt_ifs[id_parent].x, pt_ifs[id_parent].y);
}

__kernel	void	clear(write_only image2d_t img)
{
	int	x = get_global_id(0);
	int	y = get_global_id(1);

	write_imagef(img, (int2)(x, y), 0);
}
