#include "Buffer.hpp"


	/// FBO MEMBER FUNCTIONS ///


// Called after pushing color attachemnts (see .pushColorAttatchment())
void FBO::init(int width, int height) {
	// Create and bind framebuffer
	glGenFramebuffers(1, &ID);
	this->width = width; 
	this->height = height;
	this->is_init = true;
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	vector<unsigned int> colAttsGL;
	colAttsGL.reserve(colorAtts.size());

	for (int i = 0; i < colorAtts.size(); ++i) {
		ColorAttach &colAtt = colorAtts[i];
		colAtt.colorID = createColorAttachment(
			width, height,
			colAtt.internal, colAtt.format, colAtt.type,
			colAtt.texFilter, colAtt.wrapping, i
		);

		colAttsGL.push_back(GL_COLOR_ATTACHMENT0 + i);
	};

	glDrawBuffers(colAttsGL.size(), colAttsGL.data());

	depthRBO = createDepthRBO(width, height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "ERROR: Incomplete GBuffer::FBO!" << endl;
		switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
			case GL_FRAMEBUFFER_UNDEFINED:
				cout << "UNDEFINED" << endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				cout << "INCOMPLETE_ATTACHMENT" << endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				cout << "INCOMPLETE_MISSING_ATTACHMENT" << endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				cout << "INCOMPLETE_DRAW_BUFFER" << endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				cout << "INCOMPLETE_READ_BUFFER" << endl;
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				cout << "UNSUPPORTED" << endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				cout << "INCOMPLETE_MULTISAMPLE" << endl;
				break;
			default:
				cout << "Something else" << endl;
				break;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

// Create a color attachment (i.e. an element of the frambuffer like color, position, etc.)
unsigned int FBO::createColorAttachment(
	int width, int height,
	int internal, int format, 
	int type,
	int texFilter, int wrapping, int colorAttach) {
	
	unsigned int texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, type, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);

	// Will probably need to be optional if actual textures are used rather than only gbuffers
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttach, GL_TEXTURE_2D, texID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
};


// Create a renderbuffer
unsigned int FBO::createDepthRBO(int width, int height) {
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
};


	/// GBuffer MEMBER FUNCTIONS



// Uses the GBuGBuffer::ffer as input. Make sure to use to correct program with the correct GBuffer
void GBuffer::use(const vector<GLuint> &locs) {
	for (int i = 0; i < locs.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, fbo->colorAtts.at(i).colorID);
		glUniform1i(locs.at(i), i);
	};
};

// Stop using the GBuffer as input
void GBuffer::unuse(const vector<GLuint> &locs) {
	for (int i = 0; i < locs.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	};
};

// Get uniform locations of all color attachments from the associated FBO
void GBuffer::getLocs(GLuint programID, vector<GLuint> &locs) {
	for (int i = 0; i < fbo->colorAtts.size(); ++i) {
		locs.push_back(
			glGetUniformLocation(programID, fbo->colorAtts[i].uniform_name.c_str())
		);
		cout << "Loc (" << fbo->colorAtts[i].uniform_name.c_str() << ") = " << fbo->colorAtts[i].colorID << endl;
	};
};