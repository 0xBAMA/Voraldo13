#version 430
layout( local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
layout( binding = 0, rgba8ui ) uniform uimage3D colorBlockFront;
layout( binding = 1, rgba8ui ) uniform uimage3D colorBlockBack;
layout( binding = 2, r8ui ) uniform uimage3D maskBlockFront;
layout( binding = 3, r8ui ) uniform uimage3D maskBlockBack;
layout( binding = 4 ) uniform sampler2D heightmap;

uniform bool heightColor;
uniform float heightScalar;
uniform int upDirection;

uniform vec4 color;
uniform bool draw;
uniform int mask;

vec4 colorWrite = vec4( 0.0 );
bool inShape () {
	const vec3 location = vec3( gl_GlobalInvocationID.xyz );
	const vec3 blockDims = vec3( imageSize( colorBlockFront ) );

	const vec3 normalizedCoords = ( location + vec3( 0.5 ) ) / blockDims;

	vec2 xyCoord = vec2( 0.0 ); // texture uv
	float zCoord = 0.0;			// for height testing

	// make sure colorWrite is assigned to, here
	switch ( upDirection ) {
	case 0:	// +x is up
		xyCoord = normalizedCoords.yz;
		zCoord = normalizedCoords.x;
		break;
	case 1:	// -x is up
		xyCoord = normalizedCoords.yz;
		zCoord = 1.0 - normalizedCoords.x;
		break;
	case 2: // +y is up
		xyCoord = normalizedCoords.xz;
		zCoord = normalizedCoords.y;
		break;
	case 3:	// -y is up
		xyCoord = normalizedCoords.xz;
		zCoord = 1.0 - normalizedCoords.y;
		break;
	case 4: // +z is up
		xyCoord = normalizedCoords.xy;
		zCoord = normalizedCoords.z;
		break;
	case 5: // -z is up
		xyCoord = normalizedCoords.xy;
		zCoord = 1.0 - normalizedCoords.z;
		break;
	}

	// take a sample at the uv
	float heightVal = texture( heightmap, xyCoord ).x;
	colorWrite.xyz = heightColor ? ( heightVal * color.xyz ) : color.xyz;
	colorWrite.a = color.a;

	// test against the height
	heightVal *= heightScalar;
	return ( zCoord < heightVal );
}

void main () {
	const ivec3 blockLocation = ivec3( gl_GlobalInvocationID.xyz );
	const uint previousMask = imageLoad( maskBlockBack, blockLocation ).x;
	const uvec4 previousColor = imageLoad( colorBlockBack, blockLocation );
	if ( inShape() ) {
		uvec4 blendedColor = uvec4( ( mix( colorWrite, vec4( previousColor ) / 255.0, float( previousMask ) / 255.0 ) ) * 255.0 );
		imageStore( colorBlockFront, blockLocation, draw ? blendedColor : previousColor );
		imageStore( maskBlockFront, blockLocation, uvec4( max( uint( mask ), previousMask ) ) );
	} else {
		imageStore( colorBlockFront, blockLocation, previousColor );
		imageStore( maskBlockFront, blockLocation, uvec4( previousMask ) );
	}
}
