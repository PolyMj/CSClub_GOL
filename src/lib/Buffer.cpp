#include "Buffer.hpp"

// Create a color attachment (output for fragment shaders)
// Remember that format determines how many elements (e.g. float vs. vec2 vs. vec3)
unsigned int createColorAttachment(
	int width, int height,
	int internal, int format, 
	int type,
	int texFilter, int colorAttach) {
	
	unsigned int texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, type, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);

	// Will probably need to be optional if actual textures are used rather than only gbuffers
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttach, GL_TEXTURE_2D, texID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}


// Create a renderbuffer
unsigned int createDepthRBO(int width, int height) {
	// Generate an rbo
	unsigned int rbo = 0;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); // Bind rbo

	// Establish data storage format and size for the rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// Attaches the rbo to the specified attachment point of the currently bound framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind rbo
	return rbo;
}

// // Create a basic FBO
// void createFBO(FBO &fbo, int width, int height) {
// 	fbo.clear();
// 	glGenFramebuffers(1, &(fbo.ID));
// 	fbo.width = width; fbo.height = height;
// 	glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);

// 	fbo.colorIDs.push_back(createColorAttachment(
// 		width, height,
// 		GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
// 		GL_LINEAR, 0
// 	));

// 	fbo.depthRBO = createDepthRBO(width, height);
// 	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
// 		cerr << "ERROR: Incomplete FBO!" << endl;
// 		fbo.clear();
// 	}

// 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }