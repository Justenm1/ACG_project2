#include "Wall.h"
#include <math.h>
#include "../Imageio/Imageio.h"

Intersect_Cond Wall::intersection_check(const M3DVector3f start, const M3DVector3f dir, float &distance, M3DVector3f intersection_p)
{
	if (_tr1.intersection_check(start,dir,distance,intersection_p) == _k_hit)
	{
		return _k_hit;
	}else
	{
		return _tr2.intersection_check(start,dir,distance,intersection_p);
	}
}


void	Wall::shade(M3DVector3f view, M3DVector3f intersect_p,const Light & sp_light, M3DVector3f am_light, M3DVector3f color,bool shadow)
{
	// Step 1: Ambient Lighting
    m3dScaleVector3(am_light, _ka); // Scale ambient light by material's ambient coefficient (_ka)

	// Compute the surface normal
    M3DVector3f normal;
    _tr1.normal(normal); // Get normal from one of the triangles (I chose _tr1)
    //printf("The wall normal is [%f, %f, %f]\n", normal[0], normal[1], normal[2]);
    //printf("The wall normalized normal is [%f, %f, %f]\n", normal[0], normal[1], normal[2]);
    
    // Adjust the normal based on the view direction
    m3dNormalizeVector(view); // Normalize the view direction
    // printf("The wall normalized view is [%f, %f, %f]\n", view[0], view[1], view[2]);

    // Check if the normal is facing the viewer. If not, flip it.
    float dotProduct = m3dDotProduct(normal, view);
    if (dotProduct < 0.0f) {
        m3dScaleVector3(normal, -1.0f); // Flip the normal
    }
    // printf("the dot product of normal and veiewer is %f/n", dotProduct);

	// Diffuse lighting
	M3DVector3f light_dir;
	M3DVector3f light_pos;
	M3DVector3f light_color;
	sp_light.get_light(light_pos, light_color);
    m3dSubtractVectors3(light_dir, intersect_p, light_pos); // Light direction (from intersection point to light)
    m3dNormalizeVector(light_dir); // Normalize the light direction
    // printf("The light_dir is [%f, %f, %f]\n", light_dir[0], light_dir[1], light_dir[2]);
    // printf("The light_pos is [%f, %f, %f]\n", light_pos[0], light_pos[1], light_pos[2]);
    // printf("The light_color is [%f, %f, %f]\n", light_color[0], light_color[1], light_color[2]);
    // printf("The intersect_p is [%f, %f, %f]\n", intersect_p[0], intersect_p[1], intersect_p[2]);

	float NdotL = m3dDotProduct(normal, light_dir); // Dot product of normal and light direction
    NdotL = fmin(fmax(NdotL, 0.0f), 1.0f); // Ensure non-negative (clamp to 0)
    //printf("NdotL after clamping: %f\n", NdotL);
    
    M3DVector3f Diffuse;
    m3dCopyVector3(Diffuse, _color); // Copy the light color
    m3dScaleVector3(Diffuse, _kd * NdotL); // Scale by material's diffuse coefficient and NdotL

	//Specular Lighting
	//Calculate reflection vector R = 2 * (N dot L) * N - L
    M3DVector3f reflect_dir;
    float NL = m3dDotProduct(normal, light_dir);
    M3DVector3f scaled_normal;
    m3dCopyVector3(scaled_normal, normal);
    m3dScaleVector3(scaled_normal, 2.0f * NL);
    m3dSubtractVectors3(reflect_dir, scaled_normal, light_dir); // reflect_dir = 2*(N.L)*N - L
    m3dNormalizeVector(reflect_dir); // Normalize reflect direction
    // printf("The light_dir at specular is [%f, %f, %f]\n", reflect_dir[0], reflect_dir[1], reflect_dir[2]);
    // printf("The reflect_dir is [%f, %f, %f]\n", reflect_dir[0], reflect_dir[1], reflect_dir[2]);
    // printf("the dot product of normal and veiewer is %f/n", NL);
    // printf("The scaled_normal is [%f, %f, %f]\n", scaled_normal[0], scaled_normal[1], scaled_normal[2]);

	//calculate shininess coefficient
    float cosAlpha = fmax(m3dDotProduct(view, reflect_dir), 0.0f);// Calculate the cosine of the angle between the view direction and the reflection direction (v dot r)^α 
    // printf("the dot product of reflect_dir and veiewer is %f/n", NL);

    // Calculate specular intensity: ks * max( V . R, 0)^α
    float spec_intensity = pow(cosAlpha, _shininess);
    // printf("the spec_intensity is %f/n", spec_intensity);

	M3DVector3f Specular;
    m3dCopyVector3(Specular, _color); // Copy light color for specular
    m3dScaleVector3(Specular, _ks * spec_intensity); // Scale by material's specular coefficient and intensity
    // printf("The _color at specular is [%f, %f, %f]\n", reflect_dir[0], reflect_dir[1], reflect_dir[2]);
    

    // Combine ambient, diffuse, and specular components
    for (int i = 0; i < 3; i++) {
        color[i] = fmin(fmax(am_light[i] + Diffuse[i] + Specular[i], 0.0f), 1.0f); // Final color is the sum of ambient, diffuse, and specular components
	// printf("Ambient part %f\n", Ambient[i]);
	// printf("Diffuse Part %f\n", Diffuse[i]);
	// printf("Specular part %f\n", Specular[i]);
	// printf("color part %f\n", color[i]);
    }
}


void	Wall::get_reflect_direct(const M3DVector3f direct0,const M3DVector3f intersect_p, M3DVector3f reflect_direct)
{
	
}


void Wall::load_texture(std::string file_name)
{
	
}

void Wall::texture_color(M3DVector3f pos, M3DVector3f color)
{
}

void	Wall::get_texel(float x, float y, M3DVector3f color)
{
	return;
}