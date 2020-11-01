#ifndef TERRAIN_VOLUME
#define TERRAIN_VOLUME

const float PI_CONST = 3.141592653589793, pi2 = PI_CONST * 2;

float d(vec3 p, float t) {

	vec2 hillXz = cos(p.xz * pi2);

	float hills = hillXz.x * hillXz.y * cos(t * (pi2 * 0.05));
	return hills - p.y;	
}

#endif