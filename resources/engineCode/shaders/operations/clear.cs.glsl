#version 430
layout( local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
layout( binding = 0, rgba8ui ) uniform uimage3D colorBlockFront;
layout( binding = 1, rgba8ui ) uniform uimage3D colorBlockBack;
layout( binding = 2, r8ui ) uniform uimage3D maskBlockFront;
layout( binding = 3, r8ui ) uniform uimage3D maskBlockBack;

uniform bool respectMask;
uniform vec4 color;
uniform bool draw;
uniform int mask;

// todo: respect mask is obvious
// draw determines if we fill with the input color, ( 0,0,0,0 ) otherwise - blending logic will be respected if both are true
// mask input works the same as any other operation

void main () {
	const ivec3 blockLocation = ivec3( gl_GlobalInvocationID.xyz );
	const uint previousMask = imageLoad( maskBlockBack, blockLocation ).x;
	const uvec4 previousColor = imageLoad( colorBlockBack, blockLocation );

	if ( previousMask > 0 && respectMask ) {
		uvec4 blendedColor = uvec4( ( mix( color, draw ? ( vec4( previousColor ) / 255.0 ) : vec4( 0.0 ), float( previousMask ) / 255.0 ) ) * 255.0 );
		imageStore( colorBlockFront, blockLocation, blendedColor );
		imageStore( maskBlockFront, blockLocation, uvec4( max( uint( mask ), previousMask ) ) );
	} else if ( draw ) {
		imageStore( colorBlockFront, blockLocation, uvec4( color * 255.0 ) );
		imageStore( maskBlockFront, blockLocation, uvec4( max( uint( mask ), previousMask ) ) );
	} else {
		imageStore( colorBlockFront, blockLocation, uvec4( 0 ) );
		imageStore( maskBlockFront, blockLocation, uvec4( previousMask ) );
	}
}
