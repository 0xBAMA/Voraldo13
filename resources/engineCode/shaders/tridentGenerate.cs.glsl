#version 430
layout( local_size_x = 8, local_size_y = 16, local_size_z = 1 ) in;
layout( binding = 0, rgba8ui ) uniform uimage2D tridentStorage;

uniform vec3 basisX;
uniform vec3 basisY;
uniform vec3 basisZ;

#define MAXSTEPS 30
#define MAXDIST 2.5
#define EPSILON 0.01

// version without materials
float SdSmoothMin( float a, float b ) {
	float k = 0.12;
	float h = clamp( 0.5 + 0.5 * ( b - a ) / k, 0.0, 1.0 );
	return mix( b, a, h ) - k * h * ( 1.0 - h );
}

// version with materials
float SdSmoothMin( float a, float b, vec3 mtl1, vec3 mtl0, inout vec3 mtl ) {
	float k = 0.12;
	float h = clamp( 0.5 + 0.5 * ( b - a ) / k, 0.0, 1.0 );
	float s = mix( b, a, h ) - k * h * ( 1.0 - h );
	float sA = s - a;
	float sB = s - b;
	float r = sA / ( sA + sB );
	mtl = mix( mtl1, mtl0, r );
	return s;
}

float deRoundedCone ( vec3 p, vec3 a, vec3 b ) {
	float r1 = 0.1; // radius at b
	float r2 = 0.02; // radius at a

	vec3  ba = b - a;
	float l2 = dot( ba, ba );
	float rr = r1 - r2;
	float a2 = l2 - rr * rr;
	float il2 = 1.0 / l2;

	vec3 pa = p - a;
	float y = dot( pa, ba );
	float z = y - l2;
	vec3 d2 =  pa * l2 - ba * y;
	float x2 = dot( d2, d2 );
	float y2 = y * y * l2;
	float z2 = z * z * l2;

	float k = sign( rr ) * rr * rr * x2;
	if( sign( z ) * a2 * z2 > k )
		return sqrt( x2 + z2 ) * il2 - r2;
	if( sign( y ) * a2 * y2 < k )
		return sqrt( x2 + y2 ) * il2 - r1;
	return ( sqrt( x2 * a2 * il2 ) + y * rr ) * il2 - r1;
}

// with materials
vec4 deMat ( vec3 p ) {
	// return value has .rgb color and .a is distance
	vec4 result = vec4( 1000.0 );
	float x = deRoundedCone( p, vec3( 0.0 ), basisX / 2.0 );
	vec3 xc = vec3( 1.0, 0.0, 0.0 );
	float y = deRoundedCone( p, vec3( 0.0 ), basisY / 2.0 );
	vec3 yc = vec3( 0.0, 1.0, 0.0 );
	float z = deRoundedCone( p, vec3( 0.0 ), basisZ / 2.0 );
	vec3 zc = vec3( 0.0, 0.0, 1.0 );
	float c = distance( vec3( 0.0 ), p ) - 0.18;
	vec3 cc = vec3( 0.16 );
	result.a = SdSmoothMin( 1000.0, x, vec3( 0.0 ), xc, result.rgb );
	result.a = SdSmoothMin( result.a, y, result.rgb, yc, result.rgb );
	result.a = SdSmoothMin( result.a, z, result.rgb, zc, result.rgb );
	result.a = SdSmoothMin( result.a, c, result.rgb, cc, result.rgb );
	return result;
}

// without materials
float de ( vec3 p ) {
	float x = deRoundedCone( p, vec3( 0.0 ), basisX / 2.0 );
	float y = deRoundedCone( p, vec3( 0.0 ), basisY / 2.0 );
	float z = deRoundedCone( p, vec3( 0.0 ), basisZ / 2.0 );
	float c = distance( vec3( 0.0 ), p ) - 0.18;
	return SdSmoothMin( SdSmoothMin( SdSmoothMin( x, y ), z ), c );
}

void main () {
	ivec2 loc = ivec2( gl_GlobalInvocationID.xy );
	vec2 position = vec2( loc ) / vec2( imageSize( tridentStorage ).xy );
	uvec4 colorResult = uvec4( 0 );

	// ray starting location comes from mapped location on the texture
	position = ( 2.0 * position ) - vec2( 1.0 );
	position.x *= ( float( imageSize( tridentStorage ).x ) / float( imageSize( tridentStorage ).y ) );

	vec3 rayOrigin = vec3( position * 0.55, -1.0 );
	vec3 rayDirection = vec3( 0.0, 0.0, 1.0 );

	float t = 0.1;
	for ( int i = 0; i < MAXSTEPS; i++ ) {
		vec3 p = rayOrigin + t * rayDirection;
		float dist = de( p );
		if ( dist < EPSILON || t > MAXDIST ) {
			break;
		}
		t += dist;
	}

	vec4 wCol = deMat( rayOrigin + t * rayDirection );

	colorResult = uvec4( uvec3( wCol.rgb * 255 ), wCol.a < ( EPSILON * 5.0 ) ? 255 : 0 );
	imageStore( tridentStorage, loc, colorResult );
}
