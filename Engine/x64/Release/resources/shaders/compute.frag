#version 430 core

layout(binding = 0, rgba32f) uniform image2D framebuffer;

// The camera specification
uniform float num_frames;
uniform float accum;
uniform mat4 InvViewMat;
uniform mat4 InvProjectionMat;

const float PI = 3.14159265359;

#define MATERIAL_LAMBERTIAN 0
#define MATERIAL_METAL 1
#define MATERIAL_DIELECTRIC 2
#define MATERIAL_LIGHT 3

#define SHPERE 0
#define BOX 1
#define TRIANGLE 2

float seed = num_frames;
uint g_state = 0;
// _______________________________ RANDOM SPECIFICATION

uint rand(inout uint state)
{
    uint x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

// ------------------------------------------------------------------

float random(inout uint state)
{
    return (rand(state) & 0xFFFFFF) / 16777216.0f;
}

// ------------------------------------------------------------------

vec3 random_in_unit_disk(inout uint state)
{
    float a = random(state) * 2.0f * 3.1415926f;
    vec2 xy = vec2(cos(a), sin(a));
    xy *= sqrt(random(state));
    return vec3(xy, 0);
}

// ------------------------------------------------------------------

vec3 random_in_unit_sphere(inout uint state)
{
    float z = random(state) * 2.0f - 1.0f;
    float t = random(state) * 2.0f * 3.1415926f;
    float r = sqrt(max(0.0, 1.0f - z * z));
    float x = r * cos(t);
    float y = r * sin(t);
    vec3 res = vec3(x, y, z);
    res *= pow(random(state), 1.0 / 3.0);
    return res;
}


// ------------------------------------------------------------------

// _______________________________ Structures

struct Ray 
{
    vec3 direction;
    vec3 origin;
};

struct box 
{
    int material_id;
    vec3 bounds[2];
};

struct sphere 
{
    int material_id;
    float radius;
    vec3 position;
};

struct hitable
{
    int material_id;
    int object_id;
    //sphere
    float radius;
    vec3 position;
    //box
    vec3 bounds[2];
    //triangle
    vec3 t1;
    vec3 t2;
    vec3 t3;
};

struct hitRecord 
{
    float t;
    vec3 position;
    vec3 normal;
    int material_id;
};

struct Metal
{
    float roughness;
};

struct Dielectric
{
    float ref_idx;
    float transmissive;
};

struct Light
{
    vec3 lightPos;
    vec4 lightCol;
    float intensity;
};

struct Material
{
    int type;
    vec3 albedo;
    Metal metal;
    Dielectric dielectric;
    Light light;
};



struct Scene
{   int num_hitable;
    int num_mat;
    hitable objects[32];
    Material materials[32];
};




// ------------------------------------------------------------------
// _______________________________ FUNCTIONS

Ray create_ray(in vec3 origin, in vec3 dir)
{
    Ray r;

    r.origin = origin;
    r.direction = dir;

    return r;
}

Ray compute_ray(float x, float y)
{
    x = x * 2.0 - 1.0;
    y = y * 2.0 - 1.0;

    vec4 clip_pos = vec4(x, y, -1.0, 1.0);
    vec4 view_pos = InvProjectionMat * clip_pos;

    vec3 dir = vec3(InvViewMat * vec4(view_pos.x, view_pos.y, -1.0, 0.0));
    dir = normalize(dir);

    vec4 origin = InvViewMat * vec4(0.0, 0.0, 0.0, 1.0);
    origin.xyz /= origin.w;

    Ray r;

    r.origin = origin.xyz;
    r.direction = dir;

    return r;
}
// ------------------------------------------------------------------

// _______________________________INTERSECTION
bool intersectSphere(in float t_min, in float t_max, in Ray r, in hitable s, out hitRecord hit)
{
   
	vec3 oc = r.origin - s.position;
	float a = dot(r.direction, r.direction);
	float b = dot(oc, r.direction);
	float c = dot(oc, oc) - s.radius * s.radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) 
    {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) 
        {
			hit.t = temp;
			hit.position = r.origin + (hit.t * r.direction);
			

			hit.normal = normalize((hit.position - s.position) / s.radius);
			hit.material_id = s.material_id;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) 
        {
			hit.t = temp;
			hit.position = r.origin + (hit.t * r.direction);
			
			hit.normal = normalize((hit.position - s.position) / s.radius);
			hit.material_id = s.material_id;	
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------------------

bool intersectBox(float t_min, float t_max,in Ray r, in hitable b, out hitRecord rec) {
  float tmin, tmax, tymin, tymax, tzmin, tzmax;

    vec3 invdir =vec3( 1.0f / r.direction.x, 1.0f / r.direction.y, 1.0f / r.direction.z);
    int sign[3];
    if(invdir.x < 0)
    {
        sign[0] = 1;
    }
    else
    {
        sign[0] = 0;
    }
	
    if(invdir.y < 0)
    {
        sign[1] = 1;
    }
    else
    {
        sign[1] = 0;
    }

    if(invdir.z < 0)
    {
        sign[2] = 1;
    }
    else
    {
        sign[2] = 0;
    }

	tmin = (b.bounds[sign[0]].x - r.origin.x) * invdir.x;
	tmax = (b.bounds[1 - sign[0]].x - r.origin.x) * invdir.x;
	tymin = (b.bounds[sign[1]].y - r.origin.y) * invdir.y;
	tymax = (b.bounds[1 - sign[1]].y - r.origin.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (b.bounds[sign[2]].z - r.origin.z) * invdir.z;
	tzmax = (b.bounds[1 - sign[2]].z - r.origin.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
    
	    
    if ( (tmin < t_max) && (tmin > t_min) )
    {
        rec.t = tmin;
        rec.position = r.origin + (rec.t * r.direction);
        rec.material_id = b.material_id;
        
        vec3 c;
        c.x = (b.bounds[0].x + b.bounds[1].x) * 0.5f;
        c.y = (b.bounds[0].y + b.bounds[1].y) * 0.5f;
        c.z = (b.bounds[0].z + b.bounds[1].z) * 0.5f;

        vec3 pi = rec.position -c;
        vec3 d;
        d.x = (b.bounds[0].x - b.bounds[1].x) * 0.5f;
        d.y = (b.bounds[0].y - b.bounds[1].y) * 0.5f;
        d.z = (b.bounds[0].z - b.bounds[1].z) * 0.5f;
        float bias = 1.000001;

        rec.normal = normalize(vec3(((pi.x / abs(d.x) * bias)),
            ((pi.y / abs(d.y) * bias)),
            ((pi.z / abs(d.z) * bias))));

            return true;
    }
    return false;
}

bool intersectTriangle(float t_min, float t_max, in Ray r, in hitable t, out hitRecord rec)
{
    //moller_trumbore optimization 
	vec3 v0v1 = t.t2 - t.t1;
	vec3 v0v2 = t.t3 - t.t1;
	vec3 pvec = cross(r.direction, v0v2);
	float det = dot(v0v1, pvec);

	if (det < 0.0001) return false;

	float invDet = 1 / det;
	vec3 tvec = r.origin - t.t1;
	float tempu, tempv;

	tempu = dot(tvec, pvec) * invDet;
	if (tempu < 0 || tempu >1) return false;

	vec3 qvec = cross(tvec, v0v1);
	tempv = dot(r.direction, qvec) * invDet;
	if (tempv < 0 || tempu + tempv >1) return false;
	
    if ( (dot(v0v2, qvec) * invDet < t_max) && (dot(v0v2, qvec) * invDet > t_min) )
    {
        rec.t = dot(v0v2, qvec) * invDet;
	    rec.normal = normalize(cross(v0v1, v0v2));
	    rec.position = r.origin + (rec.t * r.direction);
        rec.material_id = t.material_id;

        return true;
    }
	
    return false;
}
// ------------------------------------------------------------------

bool scatter_lambertian(in Ray in_ray, in hitRecord rec, in Material mat, out vec4 attenuation, out Ray scattered_ray)
{
    vec3 new_dir = rec.position + rec.normal + random_in_unit_sphere(g_state);
    scattered_ray = create_ray(rec.position, normalize(new_dir - rec.position));
    attenuation = vec4(mat.albedo, 1.0);

    return true;
}

// ------------------------------------------------------------------

vec3 reflect( vec3  v, vec3  n)
{
	return normalize(v - 2 * dot(v, n)*n);
}

bool scatter_metal(in Ray in_ray, in hitRecord rec, in Material mat, out vec4 attenuation, out Ray scattered_ray)
{
    vec3 new_dir = reflect(in_ray.direction, rec.normal);
    scattered_ray = create_ray(rec.position, (new_dir + mat.metal.roughness * random_in_unit_sphere(g_state)));
    attenuation = vec4(mat.albedo, 1.0);

    return (dot(scattered_ray.direction, rec.normal) > 0);
}

// ------------------------------------------------------------------

bool refract(in vec3 v, in vec3 n, in float ni_over_nt, out vec3 refracted)
{

    float cosi = clamp (-1, 1 , dot(v,n));
    float etai = 1;
    float etat = 1/ni_over_nt;
    vec3 N = n;
    if(cosi<0)
    {
        cosi = -cosi;
    }
    else
    {
        float temp;
        temp = etai;
        etai = etat;
        etat = temp;
        N = -n;
    }

    float eta = etai / etat;
    float k = 1 - eta * eta *(1 - cosi * cosi);

    if(k< 0 )
    {
        refracted = vec3(0);
    }
    else
    {
        refracted=eta * v +(eta* cosi - sqrt(k))*n;
    }
    
    return true;
}


float schlick(float cosine, float ref_idx)
{
    float r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5);
}


bool scatter_dielectric(in Ray in_ray,in hitRecord rec, in Material mat, out vec4 attenuation, out Ray scattered_ray)
{
    vec3 outward_normal;
    vec3 reflected =reflect(in_ray.direction, rec.normal);
    float ni_over_nt;
    attenuation = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 refracted;
    float reflect_prob;
    float cosine;

    if (dot(in_ray.direction, rec.normal) > 0)
    {
        outward_normal = - rec.normal;
        ni_over_nt = mat.dielectric.ref_idx;
        cosine = mat.dielectric.ref_idx * dot(in_ray.direction, rec.normal) / length(in_ray.direction);

    }
    else
    {
        outward_normal = rec.normal;
        ni_over_nt = 1.0 / mat.dielectric.ref_idx;
        cosine = - dot(in_ray.direction, rec.normal) / length(in_ray.direction);
    }

    if(refract(in_ray.direction, outward_normal, ni_over_nt, refracted))
        reflect_prob = schlick(cosine, mat.dielectric.ref_idx);
    else
    reflect_prob = 1.0;

    if(random(g_state) > reflect_prob)
        scattered_ray = create_ray(rec.position + 1e-4, normalize(refracted));
    else    
        scattered_ray= create_ray(rec.position - 1e-4, reflected);

    return true;
}
// ------------------------------------------------------------------

bool ray_scene_hit(in float t_min, in float t_max, in Ray ray, in Scene scene, out hitRecord rec)
{
    float closest = t_max;
    bool hit_anything = false;

    for(int i=0; i< scene.num_hitable; i++)
    {
        if(scene.objects[i].object_id == SHPERE)
        {
            if (intersectSphere(t_min, closest, ray, scene.objects[i], rec))
            {
                hit_anything = true;
                closest = rec.t;
            }
        }

        else if (scene.objects[i].object_id == BOX)
        {
             if (intersectBox(t_min, closest, ray, scene.objects[i], rec))
            {
                hit_anything = true;
                closest = rec.t;
            }
        }

        else if(scene.objects[i].object_id == TRIANGLE)
        {
            if(intersectTriangle(t_min, closest, ray, scene.objects[i],rec))
            {
                hit_anything = true;
                closest = rec.t;
            }
            
        }

    }

    return hit_anything;
}
// ------------------------------------------------------------------


bool trace_once(in Ray ray, in Scene scene, out hitRecord rec)
{
    if (ray_scene_hit(0.001, 1000.0, ray, scene, rec))
        return true;
    else
        return false;
}

// ------------------------------------------------------------------

float inShadow(Ray r, Scene scene)
{
    Ray shadowRay = r;
    float cosine = 1;
    hitRecord temp_rec;
    float nshadow;
    while (cosine > 0.001 && random(g_state) < cosine)
    {
        bool vis = trace_once(shadowRay, scene, temp_rec);
        Ray shadowScatter;
        vec4 tempAtten;


        if(!vis)
        {
            nshadow = 1;
            cosine =0;
        }
        else
        {
            if(scene.materials[temp_rec.material_id].type == MATERIAL_DIELECTRIC)
            {
                scatter_dielectric(shadowRay, temp_rec, scene.materials[temp_rec.material_id], tempAtten, shadowScatter);
                shadowRay = shadowScatter;
                cosine = dot(shadowScatter.direction,shadowRay.direction);
                nshadow =  0.7;
            }
            else
            {
                nshadow = 0.1;
                cosine = 0;
            }
        }
    }
    return nshadow;
    

}

// ------------------------------------------------------------------

vec4 trace(in Ray r, in Scene scene) {
  hitRecord rec;
  Ray new_ray = r;
  vec4 attenuation = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 emitted = vec4(0.0);
  int depth = 0;
  vec4 color = vec4 (1.0, 1.0, 1.0, 1.0);

  //light source
  Light l;
    l.lightPos = vec3(500.0, 300.0, 0.0) ;
    l.lightCol = vec4(1.0, 1.0, 1.0, 1.0);
    l.intensity = 0.05;

    //shadow
    Ray shadowRay;
    
    
    while(depth < 100)
    {
        if(trace_once(new_ray, scene, rec))
        {
            Ray scattered_ray;
            

                 if (scene.materials[rec.material_id].type == MATERIAL_LAMBERTIAN)
            {

                vec3 L = - (rec.position - l.lightPos);
                shadowRay.origin = (rec.position + rec.normal + 1e-4) + (2*random(g_state)-1)/2 ;
                shadowRay.direction = normalize(L);
                hitRecord temp_rec;
                float nshadow;
                nshadow = inShadow(shadowRay, scene);

                if( scatter_lambertian(new_ray, rec, scene.materials[rec.material_id], attenuation, scattered_ray))
                {
                    vec3 R = reflect(L, rec.normal);
                    vec4 DifColor  = 0.5* nshadow * attenuation / PI * l.intensity  * max(0.0f , dot(rec.normal, L));
                    vec4 RefColor = vec4(vec3(nshadow * l.intensity * 30 * pow(max(0 , dot(R, new_ray.direction)), 50)), 1.0);

                    color.x *= DifColor.x * 0.8 + RefColor.x * 0.2;
                    color.y *= DifColor.y * 0.8 + RefColor.y * 0.2;
                    color.z *= DifColor.z * 0.8 + RefColor.z * 0.2;
                    new_ray = scattered_ray;
                }
                else
                {
                   attenuation = vec4(0.0, 0.0, 0.0, 1.0);
                    color  *=attenuation;
                    break;
                }
            }

            else if (scene.materials[rec.material_id].type == MATERIAL_METAL)
            {
                
                if (scatter_metal(new_ray, rec, scene.materials[rec.material_id], attenuation, scattered_ray))
                {
                    color  *=  attenuation;
                    new_ray = scattered_ray;
                    new_ray.origin = rec.position + rec.normal *1e-4;
                      
                }
                else
                {
                    attenuation = vec4(0.0, 0.0, 0.0, 1.0);
                    color  *=attenuation;
                    break;
                }
            }
               else if (scene.materials[rec.material_id].type == MATERIAL_DIELECTRIC)
            {
                if (scatter_dielectric(new_ray, rec, scene.materials[rec.material_id], attenuation, scattered_ray))
                {
                    color  *= attenuation;
                    new_ray = scattered_ray;
                }
                      
                }
                else
                {
                    attenuation = vec4(0.0, 0.0, 0.0, 1.0);
                    color  *= attenuation;
                    break;
                }
            }
            
            
        
        else
        {
            float t = 0.5 * (r.direction.y + 1.0);
            vec4 sky_color = (1.0 - t) * vec4(0.5) + t * vec4(0.05, 0.07, .010, 1.0);
            color *= sky_color;
            break;
             
        }
        
        depth++;
    }
       if (depth < 100) 
        return color;
    else
        return vec4(0.0, 0.0, 0.0, 0.0);
    
  
   
}

layout (local_size_x = 8, local_size_y = 8) in;

// ------------------------------------------------------------------

void main(void) {


    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(framebuffer);
    
    if (pix.x >= size.x || pix.y >= size.y) {
        return;
    }

     g_state = uint(num_frames) * 2699 + gl_GlobalInvocationID.x * 1973 + gl_GlobalInvocationID.y * 9277  | 1;

// ------------------------------------------------------------------
//_____________________________________________ SCENE DESCRIPTION
    Scene scene;

    int h = 0;

    // Blue Lambertian
    scene.materials[0].type = MATERIAL_LAMBERTIAN;
    scene.materials[0].albedo = vec3(0.3, 0.3, 0.8);

    // Gold lambertian
    scene.materials[1].type = MATERIAL_LAMBERTIAN;
    scene.materials[1].albedo = vec3(1.0, 0.84, 0.0);

    // Metal
    scene.materials[2].type = MATERIAL_METAL;
    scene.materials[2].albedo = vec3(.71, 0.65, 0.26);
    scene.materials[2].metal.roughness = 0.0;

    // Mirror
    scene.materials[3].type = MATERIAL_METAL;
    scene.materials[3].albedo = vec3(1);
    scene.materials[3].metal.roughness = 0.0;

    // Light sea green lambertian
    scene.materials[4].type = MATERIAL_LAMBERTIAN;
    scene.materials[4].albedo = vec3(.12, 0.69, 0.66);

    // Glass
    scene.materials[5].type = MATERIAL_DIELECTRIC;
    scene.materials[5].albedo = vec3(0.8, 0.8, 0.8);
    scene.materials[5].dielectric.ref_idx = 1.9;
    scene.materials[5].dielectric.transmissive = 0.7;
    
    
   //Glass Sphere
    scene.objects[h].radius = 9.0;
    scene.objects[h].position = vec3(55.0, 10.1, 50);
    scene.objects[h].material_id = 5;
    scene.objects[h++].object_id = SHPERE;

    scene.objects[h].radius = -8.9;
    scene.objects[h].position = vec3(55.0, 10.1, 50.0);
    scene.objects[h].material_id = 5;
    scene.objects[h++].object_id = SHPERE;

    //triangles for shadows of glass

    scene.objects[h].t1 = vec3(25, 0.1, 45 );
    scene.objects[h].t2 = vec3(25, 0.1, 65 );
    scene.objects[h].t3 = vec3(65, 0.1, 45 );
    scene.objects[h].material_id = 1;
    scene.objects[h++].object_id = TRIANGLE; 

    scene.objects[h].t1 = vec3(65, 0.1, 65 );
    scene.objects[h].t2 = vec3(65, 0.1, 45 );
    scene.objects[h].t3 = vec3(25, 0.1, 65 );
    scene.objects[h].material_id = 4;
    scene.objects[h++].object_id = TRIANGLE;       

   //Lambertian Sphere
    scene.objects[h].radius = 3.0;
    scene.objects[h].position = vec3(55.0, 6.5, 50.0);
    scene.objects[h].material_id = 0;
    scene.objects[h++].object_id = SHPERE;

    //Metal Sphere
    scene.objects[h].radius = 5.0;
    scene.objects[h].position = vec3(40.0, 11.1, 90.0);
    scene.objects[h].material_id = 2;
    scene.objects[h++].object_id = SHPERE;

    // base box
    scene.objects[h].bounds[0] = vec3(0,0,70);
    scene.objects[h].bounds[1] = vec3(30,10,100);
    scene.objects[h].material_id = 1;
    scene.objects[h++].object_id = BOX;

    // right
    scene.objects[h].bounds[0] = vec3(0,0,50);
    scene.objects[h].bounds[1] = vec3(30,6,70);
    scene.objects[h].material_id = 4;
    scene.objects[h++].object_id = BOX;

    //triangle fun
    scene.objects[h].t1 = vec3(25, 7, 65 );
    scene.objects[h].t2 = vec3(25, 7, 55 );
    scene.objects[h].t3 = vec3(22, 15, 60 );
    scene.objects[h].material_id = 1;
    scene.objects[h++].object_id = TRIANGLE;

    //left
    scene.objects[h].bounds[0] = vec3(30,0,80);
    scene.objects[h].bounds[1] = vec3(50,6,100);
    scene.objects[h].material_id = 4;
    scene.objects[h++].object_id = BOX;

    // box on box
    scene.objects[h].bounds[0] = vec3(0,10,80);
    scene.objects[h].bounds[1] = vec3(10,17,100);
    scene.objects[h].material_id = 0;
    scene.objects[h++].object_id = BOX;

    //triangle floor
    
    vec3 t1 = vec3(0.0 ,0.0, 0.0);
    vec3 t2 = vec3(0.0 ,0.0, 100.0);
    vec3 t3 = vec3(100.0 ,0.0, 0.0);
    vec3 t4 = vec3(100.0 ,0.0, 100.0);

    scene.objects[h].t1 = t1;
    scene.objects[h].t2 = t2;
    scene.objects[h].t3 = t3;
    scene.objects[h].material_id = 3;
    scene.objects[h++].object_id = TRIANGLE;

    scene.objects[h].t1 = t2;
    scene.objects[h].t2 = t4;
    scene.objects[h].t3 = t3;
    scene.objects[h].material_id = 3;
    scene.objects[h++].object_id = TRIANGLE;

    scene.num_hitable = h;
    scene.num_mat = 5;

    int ns = 5;
    vec4 sumCol= vec4(0.0, 0.0, 0.0, 0.0);

    for(int i=0; i<ns; i++)
    {
        
        vec2 pos = vec2(pix.x, pix.y) / vec2(size.x, size.y);
        vec2 altered_coord = vec2(pix.x +random(g_state), pix.y+random(g_state)) / vec2(size.x, size.y);
        Ray ray = compute_ray(altered_coord.x, altered_coord.y);
        vec4 color = trace(ray, scene);
        sumCol = sumCol + color;
    }

    
    
    vec4 prev_color = vec4(imageLoad(framebuffer,pix).rgb, 1.0);
    sumCol = sumCol / ns;

    if(!(sumCol.x == sumCol.x)) sumCol = vec4(0);
    if(!(sumCol.y == sumCol.y)) sumCol = vec4(0);
    if(!(sumCol.z == sumCol.z)) sumCol = vec4(0);

    vec4 final = mix(sumCol, prev_color, accum);

    

    for(int j =0; j<4; j++)
    {
        if(final[j]>1)
        {
         final[j] = 1;
        }
    }
    
    imageStore(framebuffer, pix, final);
}