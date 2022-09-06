#version 430
layout( local_size_x = 16, local_size_y = 16, local_size_z = 1 ) in;
layout( binding = 0, rgba8ui ) uniform uimage2D blueNoise;
layout( binding = 1, rgba16f ) uniform image2D accumulatorTexture;
layout( binding = 2, rgba8ui ) uniform uimage2D displayTexture;
layout( binding = 3, rgba8ui ) uniform uimage2D pattern;

// ssbo for the palette data, tbd
// layout( binding = 4, std430 ) buffer paletteData {
// 	int count;
// 	vec4 data[];
// };

void main () {

}
