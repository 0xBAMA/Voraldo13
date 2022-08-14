/*==============================================================================
need to handle the binding of images to the binding points for each shader
==============================================================================*/
struct binding {
	GLuint texture;
	GLenum type;
	int bindPoint;
};
struct bindSet {
	std::vector<binding> bindings;
	void applyBindset () {
		for ( auto b : bindings ) {
			glBindImageTexture( b.bindPoint, b.texture, 0, GL_FALSE, 0, GL_READ_WRITE, b.type );
		}
	}
};
