#version 430
layout( local_size_x = 16, local_size_y = 16, local_size_z = 1 ) in;
layout( binding = 0, rgba16f ) uniform image2D accumulatorTexture;
layout( binding = 1, rgba8ui ) uniform uimage2D displayTexture;
#include "tonemap.glsl" // tonemapping curves

uniform int tonemapMode;
uniform float gamma;
uniform vec3 colorTempAdjust;


vec4 linearInterpolatedSample ( vec2 location ) {
	const vec2 fractionalPart = fract( location );
	const vec2 wholePart = floor( location );

	const vec4 sample0 = imageLoad( accumulatorTexture, ivec2( wholePart ) );
	const vec4 sample1 = imageLoad( accumulatorTexture, ivec2( wholePart ) + ivec2( 1, 0 ) );
	const vec4 sample2 = imageLoad( accumulatorTexture, ivec2( wholePart ) + ivec2( 0, 1 ) );
	const vec4 sample3 = imageLoad( accumulatorTexture, ivec2( wholePart ) + ivec2( 1, 1 ) );

	const vec4 xBlend0 = mix( sample0, sample1, fractionalPart.x );
	const vec4 xBlend1 = mix( sample2, sample3, fractionalPart.x );
	return mix( xBlend0, xBlend1, fractionalPart.y );
}


// https://www.shadertoy.com/view/flVGRd // tbd, not working
#define PI    3.1415926535897932384626433
#define PI_SQ 9.8696044010893586188344910
float lanczosWeight ( float x, float r ) {
	if ( x == 0.0 ) return 1.0;
	return ( r * sin( PI * x ) * sin( PI * ( x / r ) ) ) / ( PI_SQ * x * x );
}
float lanczosWeight ( vec2 x, float r ) {
	return lanczosWeight( x.x, r ) * lanczosWeight( x.y, r );
}
vec4 lanczos ( vec2 coord ) {
	int r = 5;
	vec2 res = vec2( imageSize( accumulatorTexture ) );
	coord += -0.5 / res;
	vec2 ccoord = floor( coord * res ) / res;
	vec3 total = vec3( 0.0 );
	for ( int x = -r; x <= r; x++ ) {
		for ( int y = -r; y <= r; y++ ) {
			vec2 offs = vec2( x, y );
			vec2 sco = ( offs / res ) + ccoord;
			vec2 d = clamp( ( sco - coord ) * res, vec2( -r ), vec2( r ) );
			// vec3 val = imageLoad( accumulatorTexture, ivec2( sco * res ) ).xyz;
			vec3 val = linearInterpolatedSample( sco * res ).xyz;
			float weight = lanczosWeight( d, float( r ) );
			total += val * weight;
		}
	}
	return vec4( total, 1.0 );
}


// also want to try these
	// https://www.shadertoy.com/view/4sGcRW
	// https://www.shadertoy.com/view/sl3cz8
	// https://www.shadertoy.com/view/3t3Szr

void main () {
	ivec2 loc = ivec2( gl_GlobalInvocationID.xy );

	// compute SSFACTOR
	vec2 scalar = vec2( imageSize( accumulatorTexture ) ) / vec2( imageSize( displayTexture ) );
	vec2 samplePosition = scalar * vec2( loc );


	// uvec4 originalValue = imageLoad( accumulatorTexture, loc );
	// vec4 originalValue = linearInterpolatedSample( samplePosition );
	vec4 originalValue = lanczos( samplePosition / vec2( imageSize( accumulatorTexture ) ) );


	// vec3 color = tonemap( tonemapMode, colorTempAdjust * originalValue.xyz * 255.0 );
	vec3 color = tonemap( tonemapMode, colorTempAdjust * originalValue.xyz );
	color = gammaCorrect( gamma, color );
	// uvec4 tonemappedValue = uvec4( uvec3( color * 255.0 ), originalValue.a * 255 );
	uvec4 tonemappedValue = uvec4( uvec3( color * 255.0 ), 255 );

	imageStore( displayTexture, loc, tonemappedValue );
}
