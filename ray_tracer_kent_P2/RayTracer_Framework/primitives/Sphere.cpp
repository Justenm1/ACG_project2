#include "sphere.h"
#include "../Imageio/Imageio.h"
#include <math.h>

Intersect_Cond	Sphere::intersection_check(const M3DVector3f start, const M3DVector3f dir, float & distance, M3DVector3f intersection_p)
{
	M3DVector3f ray_to_center;
	m3dSubtractVectors3(ray_to_center,_pos,start);
	float ray_center_length = m3dDotProduct(ray_to_center, ray_to_center); 

	float closest_point = m3dDotProduct( ray_to_center, dir);
	if (closest_point < 0) 
	{
		return _k_miss;
	}

	float halfCord2 = (_rad * _rad) - (ray_center_length - (closest_point * closest_point));   
	if(halfCord2 < 0) 
	{
		return _k_miss;
	}

	Intersect_Cond type;
	M3DVector3f tmp;
	m3dSubtractVectors3(tmp,start,_pos);
	float length = m3dDotProduct(tmp,tmp);
	if (length < _rad2)
	{
		type = _k_inside;
		distance = closest_point + sqrt(halfCord2);
	}else
	{
		type = _k_hit;
		distance = closest_point - sqrt(halfCord2);
	}

	M3DVector3f tmp_v;
	m3dCopyVector3(tmp_v, dir);
	m3dScaleVector3(tmp_v,distance);
	m3dAddVectors3(intersection_p,start,tmp_v);

	return type;
}

void	Sphere::shade(M3DVector3f view,M3DVector3f intersect_p,const Light & sp_light, M3DVector3f am_light, M3DVector3f color, bool shadow)
{
	//Ambient Light
	m3dScaleVector3(am_light, _ka); // Scale ambient light by material's ambient coefficient (_ka)
	//printf("Sphere Ambient is [%f, %f, %f]\n", Ambient[0], Ambient[1], Ambient[2]);

	//getting Normal vector
	M3DVector3f Normal;
	m3dSubtractVectors3(Normal, intersect_p, _pos); // Normal = intersect_p - sphere center (_pos)
	//printf("The Sphere normal is [%f, %f, %f]\n", Normal[0], Normal[1], Normal[2]);
	m3dNormalizeVector(Normal); // Normalize the normal vector
	//printf("The Sphere normalized normal is [%f, %f, %f]\n", Normal[0], Normal[1], Normal[2]);

	//Get the Light position from the Light object
	M3DVector3f light_pos;
	M3DVector3f sp_light_color;
	sp_light.get_light(light_pos, sp_light_color);
	//printf("The Sphere light_pos is [%f, %f, %f]\n", light_pos[0], light_pos[1], light_pos[2]);
	//printf("The Sphere light_color is [%f, %f, %f]\n", sp_light_color[0], sp_light_color[1], sp_light_color[2]);

	//Calculate the direction from the intersection point to the light source
	M3DVector3f light_dir;
	m3dSubtractVectors3(light_dir, intersect_p, light_pos); // Light direction = light_pos - intersect_p
	//printf("This Sphere light_dir is [%f, %f, %f]\n", light_dir[0], light_dir[1], light_dir[2]);
	m3dNormalizeVector(light_dir); // Normalize the light direction
	//printf("This Sphere normalized light_dir is [%f, %f, %f]\n", light_dir[0], light_dir[1], light_dir[2]);

	//Diffuse intensity
	float diff = fmax(m3dDotProduct(Normal, light_dir), 0.0f);
	//printf("The sphere diffuse intensity is %d/n", diff);
	
	
	// Calculate the diffuse light contribution (light_color * diff * _kd)
    M3DVector3f Diffuse;
    m3dCopyVector3(Diffuse, _color); // Start with color
    m3dScaleVector3(Diffuse, diff * _kd); // Scale by the diffuse factor and the material's diffuse coefficient (_kd)

	//Specular Lighting
	//Calculate reflection vector R = 2 * (N dot L) * N - L
    M3DVector3f reflect_dir;
    float NL = fmax(fmin(m3dDotProduct(Normal, light_dir), 0.0f), 1.0f);
    M3DVector3f scaled_normal;
    m3dCopyVector3(scaled_normal, Normal);
    m3dScaleVector3(scaled_normal, 2.0f * NL);
    m3dSubtractVectors3(reflect_dir, scaled_normal, light_dir); // reflect_dir = 2*(N.L)*N - L
    m3dNormalizeVector(reflect_dir);

    // Normalize view direction (already passed as 'view') and reflection direction
    m3dNormalizeVector(view);
	m3dNormalizeVector(reflect_dir);

	//calculate shininess coefficient
    float cosAlpha = fmax(m3dDotProduct(view, reflect_dir), 0.0f); // Calculate the cosine of the angle between the view direction and the reflection direction (v dot r)^α 

    // Calculate specular intensity: ks * max( V . R, 0)^α
    float spec_intensity = pow(cosAlpha, _shininess);

	// Calculate Specular contribution to final color
	M3DVector3f Specular;
    m3dCopyVector3(Specular, sp_light_color); // Copy object color for specular
    m3dScaleVector3(Specular, _ks * spec_intensity); // Scale by material's specular coefficient and intensity

    // Combine ambient, diffuse, and specular components
    for (int i = 0; i < 3; i++) {
    color[i] = fminf(fmaxf(am_light[i] + Diffuse[i] + Specular[i], 0.0f), 1.0f); // Final color is the sum of ambient, diffuse, and specular components
	// printf("Ambient part %f\n", Ambient[i]);
	// printf("Diffuse Part %f\n", Diffuse[i]);
	// printf("Specular part %f\n", Specular[i]);
	// printf("color part %f\n", color[i]);	
	}
}

