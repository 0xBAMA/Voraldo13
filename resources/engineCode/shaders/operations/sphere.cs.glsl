#version 430
layout( local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
layout( binding = 0, rgba8ui ) uniform uimage3D colorBlockFront;
layout( binding = 1, rgba8ui ) uniform uimage3D colorBlockBack;
layout( binding = 2, r8ui ) uniform uimage3D maskBlockFront;
layout( binding = 3, r8ui ) uniform uimage3D maskBlockBack;

uniform vec3 center;
uniform float radius;
uniform vec4 color;
uniform bool draw;
uniform int mask;

bool inShape () {
	return distance( vec3( gl_GlobalInvocationID.xyz ), center ) < radius;
}

void main () {
	const ivec3 blockLocation = ivec3( gl_GlobalInvocationID.xyz );
	const uint previousMask = imageLoad( maskBlockBack, blockLocation ).x;
	const uvec4 previousColor = imageLoad( colorBlockBack, blockLocation );
	if ( inShape() ) {
		uvec4 blendedColor = uvec4( ( mix( color, vec4( previousColor ) / 255.0, float( previousMask ) / 255.0 ) ) * 255.0 );
		imageStore( colorBlockFront, blockLocation, draw ? blendedColor : previousColor );
		imageStore( maskBlockFront, blockLocation, uvec4( max( uint( mask ), previousMask ) ) );
	} else {
		imageStore( colorBlockFront, blockLocation, previousColor );
		imageStore( maskBlockFront, blockLocation, uvec4( previousMask ) );
	}
}
