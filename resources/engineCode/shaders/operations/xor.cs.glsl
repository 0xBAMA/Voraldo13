#version 430
layout( local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
layout( binding = 0, rgba8ui ) uniform uimage3D colorBlockFront;
layout( binding = 1, rgba8ui ) uniform uimage3D colorBlockBack;
layout( binding = 2, r8ui ) uniform uimage3D maskBlockFront;
layout( binding = 3, r8ui ) uniform uimage3D maskBlockBack;
// layout( binding = 4, rgba16f ) uniform image3D lightBlock;

uniform bool respectMask;

void main () {
	// do I want to support masking with this one? tbd
	const ivec3 blockLocation = ivec3( gl_GlobalInvocationID.xyz );
	const uvec3 blockLocationU = uvec3( gl_GlobalInvocationID.xyz );
	const uvec4 previousColor = imageLoad( colorBlockBack, blockLocation );
	const uint previousMask = imageLoad( maskBlockBack, blockLocation ).x;

	const vec4 color = vec4( float( blockLocationU.x ^ blockLocationU.y ^ blockLocationU.z ) / 255.0 );

	uvec4 blendedColor = respectMask ? uvec4( ( mix( color, vec4( previousColor ) / 255.0, float( previousMask ) / 255.0 ) ) * 255.0 ) : uvec4( color * 255 );

	imageStore( colorBlockFront, blockLocation, blendedColor );
	imageStore( maskBlockFront, blockLocation, uvec4( previousMask ) );
}
