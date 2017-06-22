
__kernel	void	clear(write_only image2d_t img)
{
	int	x = get_global_id(0);
	int	y = get_global_id(1);

	write_imagef(img, (int2)(x, y), 0);
}


__kernel	void	draw_line(write_only image2d_t img
									   , __global float2 *pt
									   , __global char4 *col
									   , __global t_ifs_spec *spec)
{
	char4	dc;
	float2	unit_pos;
	float4	unit_col;
	float2	p;
	float4	c;
	int		indice;
	int		col_value;
	float	dist;
	int		nb_point;
	int		i;
	int		id, id_col;
	bool	is_inside;
	int2	pos;
	int		w = spec->ecr_x;
	int		h = spec->ecr_y;

	id_col = get_global_id(0);
	id = id_col + spec->beg_id[spec->nb_iter - 1];
	dc = col[id + 1] - col[id];
	dist = get_line_length(pt[id], pt[id + 1]);
	nb_point = dist;
	unit_pos = (pt[id + 1] - pt[id]) / dist;
	unit_col = (float4)(dc.x, dc.y, dc.z, 0) / dist;
	i = 0;
	p = pt[id];
	c = (float4)(col[id].x, col[id].y, col[id].z, 0);
	while(i <= nb_point)
	{
		indice = (int)p.x + (int)p.y * w;
		col_value = 0xFFFFFF;//((((int)c.x) & 0xFF) << 16) | ((((int)c.y) & 0xFF) << 8) | ((((int)c.z)) & 0xFF);
		is_inside = (p.x >= 0 && p.x < w) && (p.y >= 0 && p.y < h);	
		pos.x =(int)p.x;
		pos.y =(int)p.y;
		if (is_inside)
			write_imagef(img, pos, col_value);;//			img[indice] = col_value;
		p += unit_pos;
		c += unit_col;
		i++;
	}
}


