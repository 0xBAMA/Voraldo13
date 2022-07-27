#version 430
layout( local_size_x = 8, local_size_y = 16, local_size_z = 1 ) in;
layout( binding = 0, rgba8ui ) uniform uimage2D tridentStorage;
layout( binding = 1, rgba8ui ) uniform uimage2D writeTarget;

void main () {

}
