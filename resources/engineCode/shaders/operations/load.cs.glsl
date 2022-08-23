#version 430
layout( local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
layout( binding = 0, rgba8ui ) uniform uimage3D colorBlockFront;
layout( binding = 1, rgba8ui ) uniform uimage3D colorBlockBack;
layout( binding = 2, r8ui ) uniform uimage3D maskBlockFront;
layout( binding = 3, r8ui ) uniform uimage3D maskBlockBack;
layout( binding = 4, rgba8ui ) uniform uimage3D loadBuffer;

uniform bool respectMask;

void main () {
	const ivec3 blockLocation = ivec3( gl_GlobalInvocationID.xyz );
	const uvec4 previousColor = imageLoad( colorBlockBack, blockLocation );
	const uint previousMask = imageLoad( maskBlockBack, blockLocation ).x;
	const uvec4 loadedColor = imageLoad( loadBuffer, blockLocation );

	const uvec4 blendedColor = respectMask ? uvec4( mix( vec4( loadedColor ), vec4( previousColor ), float( previousMask ) / 255.0 ) ) : loadedColor;

	imageStore( maskBlockFront, blockLocation, uvec4( previousMask ) );
	imageStore( colorBlockFront, blockLocation, blendedColor );
}
