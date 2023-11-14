#version 330 core

in vec3 vertexPos;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 cameraFwd;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

// SETTINGS
uniform int maxBounces;
uniform int samplesPerPixel;

uniform float time;

struct Material {
	vec3 color;
	vec3 emissionColor;
	float emissionStrength;
	float smoothness;
};

struct Sphere {
	vec3 position;
	float radius;

	Material material;
};

struct HitInfo {
	bool hit;
	vec3 hitPoint;
	vec3 hitNormal;

	Material material;
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Plane {
	vec3 position;
	vec3 normal;

	Material material;
};

HitInfo TraceSphere(Ray ray, Sphere sphere) {
    vec3 oc = ray.origin - sphere.position;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

    float disc = b * b - 4.0 * a * c;

    if (disc < 0) return HitInfo(false, vec3(0.0), vec3(0.0), sphere.material);

    float t = (-b - sqrt(disc)) / (2.0 * a);
    if(t < 0) return HitInfo(false, vec3(0.0), vec3(0.0), sphere.material);

    vec3 hitPoint = ray.origin + ray.direction * t;
    vec3 hitNormal = normalize(hitPoint - sphere.position);

    return HitInfo(true, hitPoint, hitNormal, sphere.material);
}

HitInfo TracePlane(Ray ray, Plane plane){
	float denom = dot(plane.normal, ray.direction);
	if (abs(denom) > 0.001f){
		float t = dot(plane.position - ray.origin, plane.normal) / denom;
		if (t >= 0) return HitInfo(true, ray.origin + ray.direction * t, plane.normal, plane.material);
	}
	return HitInfo(false, vec3(0.0), vec3(0.0), plane.material);
}

#define MAX_FLOAT 1e8

HitInfo TraceScene(Ray ray) {
	Sphere spheres[9];
	spheres[0] = Sphere(vec3(0.0, 0.5, 5.0), 0.5, Material(vec3(0.2), vec3(0.0), 0.0, 0.9));
	spheres[1] = Sphere(vec3(0.0, 0.5, 0.0), 0.5, Material(vec3(0.2, 0.5, 0.6), vec3(0.0), 0.0, 0.9));
	spheres[2] = Sphere(vec3(2.0, 1.0, 4.0), 1.0, Material(vec3(0.8, 0.5, 0.8), vec3(0.8, 0.5, 0.8), 1.0, 0.4));
	spheres[3] = Sphere(vec3(1.0, 0.5, 3.0), 0.5, Material(vec3(0.2, 0.8, 0.3), vec3(0.0), 0.0, 0.8));
	spheres[4] = Sphere(vec3(3.0, 0.5, 2.0), 0.3, Material(vec3(0.5, 0.3, 0.5), vec3(0.0), 0.0, 0.8));
	spheres[5] = Sphere(vec3(5.0, 0.5, 0.0), 0.5, Material(vec3(0.2, 0.3, 0.5), vec3(0.0), 0.0, 0.8));
	spheres[6] = Sphere(vec3(3.0, 0.5, 2.0), 0.5, Material(vec3(0.2, 0.3, 0.5), vec3(0.0), 0.0, 0.8));
	spheres[7] = Sphere(vec3(4.0, 0.5, 1.0), 0.5, Material(vec3(0.2, 0.3, 0.5), vec3(0.0), 0.0, 0.8));
	spheres[8] = Sphere(vec3(5.0, 0.5, 4.0), 0.5, Material(vec3(0.2, 0.3, 0.5), vec3(0.0), 0.0, 0.8));

	Plane planes[2];
	planes[0] = Plane(vec3(0.0, 0.0, 0.0),  vec3(0.0, 1.0, 0.0),  Material(vec3(0.7, 0.5, 0.8), vec3(0.0), 0.0, 0.5));
	planes[1] = Plane(vec3(10.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), Material(vec3(0.7, 0.5, 0.8), vec3(0.0), 0.0, 0.5));

    float closestT = MAX_FLOAT;
    HitInfo closestHit = HitInfo(false, vec3(0.0), vec3(0.0), Material(vec3(0.0), vec3(0.0), 0.0, 0.0));

    for(int i = 0; i < 9; i++) {
        HitInfo hit = TraceSphere(ray, spheres[i]);
        if(hit.hit && dot(ray.direction, hit.hitPoint - ray.origin) < closestT) {
            closestT = dot(ray.direction, hit.hitPoint - ray.origin);
            closestHit = hit;
        }
    }

	for (int i = 0; i < 2; i++){
		HitInfo hit = TracePlane(ray, planes[i]);
		if (hit.hit && dot(ray.direction, hit.hitPoint - ray.origin) < closestT){
			closestT = dot(ray.direction, hit.hitPoint - ray.origin);
			closestHit = hit;
		}
	}

    return closestHit;
}

const uint k = 1103515245U;
uvec3 seed;
void hash(){	
	seed = ((seed >> 8u)^seed.yzx) * k;
	seed = ((seed >> 8u)^seed.yzx) * k;
	seed = ((seed >> 8u)^seed.yzx) * k;
}

float RandomValue(){
	hash();
	return float(seed) / 4294967296.0;
}


#define PI 3.141592653589793

vec3 RandomHemisphereDirection(vec3 normal){
	float x = RandomValue() * 2.0 - 1.0;
	float y = RandomValue() * 2.0 - 1.0;
	float z = RandomValue() * 2.0 - 1.0;

	vec3 randomDirection = normalize(vec3(x, y, z));

	if (dot(randomDirection, normal) < 0.0){
		randomDirection = -randomDirection;
	}

	return randomDirection;
}

vec3 RandomDirection(){
	float x = RandomValue() * 2.0 - 1.0;
	float y = RandomValue() * 2.0 - 1.0;
	float z = RandomValue() * 2.0 - 1.0;

	vec3 randomDirection = normalize(vec3(x, y, z));

	return randomDirection;
}

vec3 TraceRay(Ray ray){
	vec3 incomingLight = vec3(0.0);
	vec3 rayColor = vec3(1.0);

	for (int i = 0; i < maxBounces; i++){
		HitInfo hitInfo = TraceScene(ray);
		
		if (hitInfo.hit){
			bool isSpecularBounce = hitInfo.material.smoothness >= RandomValue();
			ray.origin = hitInfo.hitPoint + hitInfo.hitNormal * 0.01;

			vec3 diffuseDir = normalize(hitInfo.hitNormal + RandomDirection());
			vec3 specularDir = reflect(ray.direction, hitInfo.hitNormal);
			if (isSpecularBounce){
				ray.direction = normalize(mix(diffuseDir, specularDir, hitInfo.material.smoothness));
			} else {
				ray.direction = diffuseDir;
			}

			vec3 emittedLight = hitInfo.material.emissionColor * hitInfo.material.emissionStrength;
			incomingLight += emittedLight * rayColor;
			rayColor *= hitInfo.material.color;

			float p = max(rayColor.r, max(rayColor.g, rayColor.b));
			
			if (RandomValue() > p){
				break;
			}

			rayColor /= p;

		} else {
			break;
		}
	}

	return incomingLight;
}

#define PI 3.141592653589793
#define DEG_TO_RAD PI/180.0

void main(){
	seed = uvec3(abs(vertexPos.x) * 1000.0, abs(vertexPos.y) * 1000.0, uint(time * 1000.0));

	float fov = 90.0 * DEG_TO_RAD;
	float _tan = tan(fov * 0.5);
	
	vec3 rayOrigin = cameraPos;
	vec3 rayDirection = normalize(cameraFwd + cameraRight * vertexPos.x * _tan + cameraUp * vertexPos.y * _tan);
	
	Ray ray = Ray(rayOrigin, rayDirection);

	vec3 color = vec3(0.0);

	for (int i = 0; i < samplesPerPixel; i++){
		color += TraceRay(ray);
	}
	
	FragColor = vec4(color, 1.0);
}