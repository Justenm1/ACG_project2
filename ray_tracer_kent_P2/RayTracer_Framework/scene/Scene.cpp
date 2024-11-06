#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene(void)
{	
	Basic_Primitive * prim = NULL;
	for (Prim_List::iterator it = _prim_list.begin(); it != _prim_list.end();)
	{
		prim = * it;
		_prim_list.erase(it++);
		delete prim;
	}
}

void	Scene::assemble()
{
	M3DVector3f	x0y0z0 = {0, 0, 0};
	M3DVector3f	x1y0z0 = {_dim[0], 0, 0};
	M3DVector3f	x0y1z0 = {0,_dim[1], 0};
	M3DVector3f	x1y1z0 = {_dim[0], _dim[1], 0};
	M3DVector3f	x0y0z1 = {0,0,_dim[2]};
	M3DVector3f	x1y0z1 = {_dim[0],0,_dim[2]};
	M3DVector3f	x0y1z1 = { 0,_dim[1],_dim[2]};
	M3DVector3f	x1y1z1 = { _dim[0],_dim[1],_dim[2]};

//Walls
	M3DVector3f wall_color_left = {1.0,0.0,0.0};
	Wall * left_wall = new Wall(x0y1z0,x0y1z1,x0y0z1,x0y0z0,wall_color_left);
	_prim_list.push_back(left_wall); // Left wall
	
	M3DVector3f wall_color_right = {5.0,0.0,5.0};
	Wall * right_wall = new Wall(x1y1z1,x1y1z0,x1y0z0,x1y0z1,wall_color_right);
	_prim_list.push_back(right_wall);	// Right wall
	
	M3DVector3f wall_color = {0.0, 0.4, 0.9};
	Wall * top_wall = new Wall(x1y1z1,x0y1z1,x0y1z0,x1y1z0,wall_color);
	_prim_list.push_back(top_wall);	// Top wall

	Wall * bottom_wall = new Wall(x1y0z1,x1y0z0,x0y0z0,x0y0z1,wall_color);
	bottom_wall->set_properties(0.9, 0, 0.9, 0);
	_prim_list.push_back(bottom_wall);	// Bottom wall

	M3DVector3f back_wall_color = {0.0 ,1.0,0.0};
	Wall * back_wall = new Wall(x1y1z0,x0y1z0,x0y0z0,x1y0z0,back_wall_color);
	_prim_list.push_back(back_wall);	// Back wall
//End of walls

//Spheres
	float	rad = _dim[2]/4.0;
	M3DVector3f sphere_color1 = {1.0, 0.0, 0.0};
	M3DVector3f sphere_center1 = {_dim[0] - rad - 20, rad, _dim[2]*2.0/3.0  - rad};
	Sphere * sp1 = new Sphere(sphere_center1,rad,sphere_color1); 
	sp1->set_properties(0.0, 0.9, 0.0, 0.9);
	_prim_list.push_back(sp1);

	rad /= 1.5;
	M3DVector3f sphere_center2 =  { rad + 20, rad, rad + 20};
	M3DVector3f sphere_color2 = {1.0, 0.5, 0.0};
	Sphere *  sp2 = new Sphere(sphere_center2, rad, sphere_color2); 
	sp2->set_properties(0.0, 0, 0.0, 0);
	_prim_list.push_back(sp2);
//End of Spheres
	
}

Intersect_Cond	Scene::intersection_check(const M3DVector3f start, const M3DVector3f dir, Basic_Primitive * * prim_intersect, M3DVector3f closest_point)
{
	Basic_Primitive * prim = NULL;
	float		distance;
	M3DVector3f point;
	
	float	min_distance = 0x0ffffff;
	*prim_intersect = NULL;

	Intersect_Cond ret = _k_miss;
	Intersect_Cond tmp;
	for (Prim_List::iterator it = _prim_list.begin(); it != _prim_list.end(); it++)
	{
		prim = * it;
		tmp = prim->intersection_check(start,dir,distance,point);
		if (tmp != _k_miss)
		{
			if (distance < min_distance)
			{
				min_distance = distance;
				*prim_intersect = prim;
				m3dCopyVector3(closest_point,point);
				ret = tmp;
			}
		}
	}

	return ret;
}