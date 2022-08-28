#version 430
layout( local_size_x = 16, local_size_y = 16, local_size_z = 1 ) in;
layout( binding = 0, rgba8ui ) uniform uimage2D blueNoiseTexture;
layout( binding = 1, rgba16f ) uniform image2D accumulatorTexture;
layout( binding = 2 ) uniform sampler3D colorBlockFront;
layout( binding = 3 ) uniform sampler3D lightingBlock;

// =============================================================================
// rendering settings - consider packing these
uniform vec4 clearColor;
uniform float scale;
uniform float perspectiveFactor;
uniform float alphaPower;
uniform float blendFactor;
uniform float jitterFactor;
uniform int numSteps;

// basis vectors used to construct view ray
uniform mat3 invBasis;

// offsets, in texels
uniform ivec2 tileOffset;
uniform vec2 renderOffset;
uniform ivec2 noiseOffset;
// =============================================================================

float tMin, tMax; // global state tracking
bool Intersect ( const vec3 rO, vec3 rD ) {
// Intersect() code adapted from:
//    Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
//    "An Efficient and Robust Ray-Box Intersection Algorithm"
//    Journal of graphics tools, 10(1):49-54, 2005
	const float minDistance = 0.0;
	const float maxDistance = 5.0;
	int s[ 3 ]; // sign toggle
	// inverse of ray direction
	const vec3 iD = vec3( 1.0 ) / rD;
	s[ 0 ] = ( iD[ 0 ] < 0 ) ? 1 : 0;
	s[ 1 ] = ( iD[ 1 ] < 0 ) ? 1 : 0;
	s[ 2 ] = ( iD[ 2 ] < 0 ) ? 1 : 0;
	const vec3 min = vec3( -1.0, -1.0, -1.0 );
	const vec3 max = vec3(  1.0,  1.0,  1.0 );
	const vec3 b[ 2 ] = { min, max }; // bounds
	tMin = ( b[ s[ 0 ] ][ 0 ] - rO[ 0 ] ) * iD[ 0 ];
	tMax = ( b[ 1 - s[ 0 ] ][ 0 ] - rO[ 0 ] ) * iD[ 0 ];
	const float tYMin = ( b[ s[ 1 ] ][ 1 ] - rO[ 1 ] ) * iD[ 1 ];
	const float tYMax = ( b[ 1 - s[ 1 ] ][ 1 ] - rO[ 1 ] ) * iD[ 1 ];
	if ( ( tMin > tYMax ) || ( tYMin > tMax ) ) return false;
	if ( tYMin > tMin ) tMin = tYMin;
	if ( tYMax < tMax ) tMax = tYMax;
	const float tZMin = ( b[ s[ 2 ] ][ 2 ] - rO[ 2 ] ) * iD[ 2 ];
	const float tZMax = ( b[ 1 - s[ 2 ] ][ 2 ] - rO[ 2 ] ) * iD[ 2 ];
	if ( ( tMin > tZMax ) || ( tZMin > tMax ) ) return false;
	if ( tZMin > tMin ) tMin = tZMin;
	if ( tZMax < tMax ) tMax = tZMax;
	return ( ( tMin < maxDistance ) && ( tMax > minDistance ) );
}

void getColorForPixel ( vec3 rO, vec3 rD, inout vec4 color ) {
	float tCurrent = tMax;
	const float stepSize = max( float( ( tMax - tMin ) / numSteps ), 0.001 );
	vec3 samplePosition = ( rO + tCurrent * rD + vec3( 1.0 ) ) / 2.0;
	vec4 newRead = texture( colorBlockFront, samplePosition );
	vec4 newLightRead = texture( lightingBlock, samplePosition );
	for ( int i = 0; i < numSteps; i++ ) {
		if( tCurrent >= tMin ) {
			newRead.rgb *= newLightRead.xyz;
			float alphaSquared = pow( newRead.a, alphaPower ); // gives more usable range on the alpha channel
			// alpha blending, new sample over running color
			color.a = max( alphaSquared + color.a * ( 1.0 - alphaSquared ), 0.001 );
			color.rgb = newRead.rgb * alphaSquared + color.rgb * color.a * ( 1.0 - alphaSquared );
			color.rgb /= color.a; // missing piece of a over b alpha blending - can't really see a lot of difference
			tCurrent -= stepSize;
			samplePosition = ( rO + tCurrent * rD + vec3( 1.0 ) ) / 2.0;
			newRead = texture( colorBlockFront, samplePosition );
			newLightRead = texture( lightingBlock, samplePosition );
		}
	}
}

void main () {
	const ivec2 invocation   = ivec2( gl_GlobalInvocationID.xy ) + tileOffset;
	const ivec2 location     = invocation + ivec2( renderOffset );
	const ivec2 iDimensions  = imageSize( accumulatorTexture );
	const vec2 dimensions    = vec2( iDimensions );
	const float aspectRatio  = dimensions.y / dimensions.x;
	const ivec2 noiseLoc     = ( noiseOffset + invocation ) % imageSize( blueNoiseTexture );
	const vec2 blueNoiseRead = jitterFactor * ( imageLoad( blueNoiseTexture, noiseLoc ).xy / 255.0 );
	const vec2 uv            = ( location + blueNoiseRead ) / dimensions;
	const vec2 mappedPos     = scale * ( ( uv - vec2( 0.5 ) ) * vec2( 1.0, aspectRatio ) );
	const vec3 rayOrigin     = invBasis * vec3( mappedPos, 2.0 );
	const vec3 rayDirection  = invBasis * vec3( perspectiveFactor * mappedPos, -2.0 );
	const vec4 prevColor     = imageLoad( accumulatorTexture, invocation );

	vec4 color = clearColor;
	if ( invocation.x < iDimensions.x && invocation.y < iDimensions.y ) {
		if ( Intersect( rayOrigin, rayDirection ) ) {
			getColorForPixel( rayOrigin, rayDirection, color );
		}
		imageStore( accumulatorTexture, invocation, color * ( 1.0 - blendFactor ) + prevColor * blendFactor );
	}
}
