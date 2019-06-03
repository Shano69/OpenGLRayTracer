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

#define ZERO (min(num_frames,0))

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

float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float sdPlane( vec3 p )
{
	return p.y;
}

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



const float maxHei = 0.8;

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


bool intersectTriangle(float t_min, float t_max, in Ray r, in hitable t, out hitRecord rec)
{
    //moller_trumbore optimization 
	vec3 v0v1 = abs(t.t2) - abs(t.t1);
	vec3 v0v2 = abs(t.t3) - abs(t.t1);
	vec3 pvec = cross(r.direction, v0v2);
	float det = dot(v0v1, pvec);

    //backface culling errors needs work
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
//              MATERIALS

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

     vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminat = 1.0 - ni_over_nt * ni_over_nt * (1-dt*dt);
    if(discriminat > 0){
        refracted = ni_over_nt * (uv-n*dt) - n*sqrt(discriminat);
        return true;
    }
    else
        return false; // no refracted ray
}


float schlick(float cosine, float ref_idx)
{
    float r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5);
}

bool scatter_dielectric(in Ray in_ray,in vec3 pos ,in vec3 nor, out vec3 attenuation, out Ray scattered_ray)
{
    vec3 outward_normal;
    vec3 reflected =reflect(in_ray.direction, nor);
    float ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    vec3 refracted;
    float reflect_prob;
    float cosine;

    if (dot(in_ray.direction, nor) > 0)
    {
        outward_normal = - nor;
        ni_over_nt = 1.52;
        cosine =  dot(normalize(in_ray.direction), nor);

    }
    else
    {
        outward_normal = nor;
        ni_over_nt = 1.0 / 1.52;
        cosine = - dot(normalize(in_ray.direction), nor);
    }

    if(refract(in_ray.direction, outward_normal, ni_over_nt, refracted))
        reflect_prob = schlick(cosine, 1.52);
    else
    reflect_prob = 1.0;

    if(random(g_state) > reflect_prob)
        scattered_ray = create_ray(pos , normalize(refracted));
    else    
        scattered_ray= create_ray(pos, reflected);

    return true;

    return true;
}
//OLD VERSION SCATTER DIELECTRIC
/* 
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
*/
// ------------------------------------------------------------------

vec2 opU( vec2 d1, vec2 d2 )
{
	return (d1.x<d2.x) ? d1 : d2;
}

vec2 map( in vec3 pos )
{
    vec2 res = vec2( 1e10, 0.0 );
    //METAL = 3 DIELECTRIC = 2

    res = opU( res, vec2( sdSphere(    pos-vec3( 0.0,1.26, 3.0), 1.25 ), 2.0 ) );

    res = opU( res, vec2( sdBox(       pos-vec3( 5.0,2.95, -3.0), vec3(2.75) ), 3.0 ) );   
    res = opU( res, vec2( sdBox(       pos-vec3( 5.0,2.95, 3.0), vec3(2.75) ), 2.0 ) );  
    res = opU( res, vec2( sdBox(       pos-vec3(-2.0, 0.90, 0.0), vec3(0.25) ), 69.0 ) );

    //res = opU( res, vec2( sdPlane(pos-0),3.0 ) );
    return res;
}


// ------------------------------------------------------------------

vec2 trace_once(in Ray ray)
{
  
// ------------------------------------------------------------------
 vec2 res = vec2(-1.0,-1.0);

    float tmin = 1.0;
    float tmax = 600.0;

      // raytrace floor plane
    float tp1 = (0.0-ray.origin.y)/ray.direction.y;
    if( tp1>0.0 )
    {
        tmax = min( tmax, tp1 );
        res = vec2( tp1, 1.1 );
    }
    // raymatch primitives   
    
        float t = tmin;
        for( int i=0; i<100&& t<tmax; i++ )
        {
            vec2 h = map( ray.origin+ray.direction*t );
            if( abs(h.x)<(0.0001*t) )
            { 
                res = vec2(t,h.y); 
                 break;
            }
            t += h.x;
        }
    
    
    return res;
}

vec3 calcNormal( in vec3 pos )
{

    // inspired by klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for( int i=0; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*map(pos+0.0005*e).x;
    }
    return normalize(n);

}

// ------------------------------------------------------------------

float shadow(in vec3 ro, in vec3 rd, in vec3 pos,in vec3 nor, float mint, float maxt, float k)
{
    //simple hard shadow
    /*
    for( float t=mint; t < maxt; )
    {
        float h = map(ro + rd*t).x;
        if( h<0.001 )
            return 0.0;
        t += h;
    }
    return 1.0;
    */

    //soft shadow
    /*
    float res = 1.0;
    for( float t=mint; t < maxt; )
    {
        float h = map(ro + rd*t).x;
        if( h<0.001 )
            return 0.0;
        res = min( res, k*h/t );
        t += h;
    }
    return res;    
    */
    //Improved soft shadows
    float res = 1.0;
    float ph = 1e20;
    float cosine = 1;
    Ray shadowRay = create_ray(ro,rd);
    
        Ray shadowScatter;
        vec3 tempAtten;
        
        for( float t=mint; t < maxt; )
        {
            float h = map(shadowRay.origin+ shadowRay.direction*t).x;

            if( h<0.001 )
                return 0.0;
            if(map(shadowRay.origin +shadowRay.direction*t).y == 2.0)
            {
                //in Ray in_ray,in vec3 pos ,in vec3 nor, out vec3 attenuation, out Ray scattered_ray
                scatter_dielectric(shadowRay, pos, nor, tempAtten,shadowScatter );
                shadowRay = shadowScatter;
                cosine = dot(shadowScatter.direction,shadowRay.direction);
                float y = h*h/(2.0*ph);
                float d = sqrt(h*h-y*y);
                res = min( res, k*d/max(0.0,t-y) );
            }
            else
            {
                float y = h*h/(2.0*ph);
                float d = sqrt(h*h-y*y);
                res = min( res, k*d/max(0.0,t-y) );
            }

            ph = h;
            t += h;
        
        }
        

    return res;
}
// ------------------------------------------------------------------

float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<4; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) * (0.5+0.5*nor.y);
}

// ------------------------------------------------------------------

vec4 trace(in Ray r, in Scene scene) 
{
    int depth = 0;
    vec3 col = vec3(1.0f);
    while(depth<100)
    {
        col = vec3(0.7, 0.9, 1.0) +r.direction.y*0.8;
        vec2 res = trace_once(r);
        float t = res.x;
	    float m = res.y;
    
        vec3 pos = r.origin + t*r.direction;
        vec3 nor = (m<1.5) ? vec3(0.0,1.0,0.0) : calcNormal( pos );

        //metal reflect
        
        if(m==3.0f)
        {
            //scatter the ray
            vec3 new_dir = reflect(r.direction,nor);
            Ray scattered_ray = create_ray(pos, (new_dir + 0.01 * random_in_unit_sphere(g_state)));
            vec4 attenuation = vec4(vec3(1.0, 0.84, 0.0), 1.0);
 
            
            if (dot(scattered_ray.direction, nor) > 0)
            {
                col  *=  vec3(attenuation.x,attenuation.y,attenuation.z);
                r = scattered_ray;
                //r.origin = pos + nor *1e-4;
            }
           
        }
        //Dielectric reflect
        else if (m == 2.0f)
        {
            Ray scattered_ray = r;
            vec3 attenuation=vec3(0.0);
            bool pass = scatter_dielectric(r, pos, nor, attenuation, scattered_ray);
            
            col  *=  vec3(attenuation.x,attenuation.y,attenuation.z);
            r = scattered_ray;
            
        }
        //lambertian reflect

        else if( m>-0.5 && m!=1.0f && m!= 2.0f)
        {
            vec3 ref = reflect( r.direction, nor );

            col = 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(m-1.0) );

            //Calculate light elemets
            //light values
            vec3  lig = normalize( vec3(-0.4, 0.7, -0.6) );
            vec3  hal = normalize( lig-r.direction );
            //Ambient Occlusion
             float occ = calcAO( pos, nor );
            //Ambient(sky)
            float sky = clamp( 0.5+0.5*nor.y, 0.0, 1.0 );
            //Diffuse(sun)
            float sun = clamp( dot( nor, lig ), 0.0, 1.0 );
            //Fresnel light
            float fre = pow( clamp(1.0+dot(nor,r.direction),0.0,1.0), 2.0 );
            //Specular light
            float spe = pow( clamp( dot( nor, hal ), 0.0, 1.0 ),16.0)*
                        sun *
                        (0.04 + 0.96*pow( clamp(1.0+dot(hal,r.direction),0.0,1.0), 5.0 ));

            //Shadow
            sun = sun * shadow(pos, lig, pos, nor, 0.02, 10.5, 8);

            vec3 lin = vec3(0.0);

            lin += 1.40*sun*vec3(1.00,0.80,0.55);
            lin += 0.20*sky*vec3(0.40,0.60,1.00)*occ;
            lin += 0.25*fre*vec3(1.00,1.00,1.00)*occ;
            col = col*lin;
            col += 9.00*spe*vec3(1.00,0.90,0.70);
            
            break;
        }
        
        else
        {
            col  *=vec3(0.7, 0.9, 0.9) +r.direction.y*0.6;
            break;
        }

         depth++;
    }
   
    return vec4( clamp(col,0.0,1.0),1.0 );
    
}

layout (local_size_x = 8, local_size_y = 8) in;

// ------------------------------------------------------------------



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

    

   //Lambertian Sphere
    scene.objects[h].radius = 3.0;
    scene.objects[h].position = vec3(55.0, 6.5, 50.0);
    scene.objects[h].material_id = 0;
    scene.objects[h++].object_id = SHPERE;

    

    scene.num_hitable = h;
    scene.num_mat = 1;

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