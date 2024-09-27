#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;


// Create a color attachment (output for fragment shaders)
// Remember that format determines how many elements (e.g. float vs. vec2 vs. vec3)
unsigned int createColorAttachment(
	int width, int height,
	int internal, int format, int type,
	int texFilter, int colorAttach);

// Create a renderbuffer
unsigned int createDepthRBO(int width, int height);



struct ColorAttach {
    string uniform_name;
    int internal; 
    int format; 
    int type; 
    int texFilter;
    GLuint colorID = 0;
};

struct FBO {
    unsigned int ID;
    int width;
    int height;
    bool is_init = false;
    vector<ColorAttach> colorAtts;
    unsigned int depthRBO;

    void clear() {
        ID = 0;
        width = 0;
        height = 0;
        colorAtts.clear();
        depthRBO = 0;
    };

    // Appends a new set of color attachment parameters
    inline void pushColorAttachment(string unifName, int internal, int format, int type, int texFilter) {
        colorAtts.push_back(ColorAttach{
            unifName,
            internal, format, type,
            texFilter
        });
    };

        /// PRESETS
    // RGB
    inline void pushRGBttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
    // RGBA
    inline void pushRGBAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA, GL_RGBA, GL_FLOAT,
            texFilter
        });
    };
    // Vec3f
    inline void pushVec3fttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB16F, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
    // Vec4f
    inline void pushVec4fttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA16F, GL_RGBA, GL_FLOAT,
            texFilter
        });
    };
    // Byte (0-255)
    inline void pushByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R8, GL_RED, GL_UNSIGNED_BYTE,
            texFilter
        });
    };

    // Called after pushing color attachemnts (see .pushColorAttatchment())
    void init(int width, int height) {
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
                colAtt.texFilter, i
            );

            colAttsGL.push_back(GL_COLOR_ATTACHMENT0 + i);
        };

        glDrawBuffers(colAttsGL.size(), colAttsGL.data());

        depthRBO = createDepthRBO(width, height);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cerr << "ERROR: Incomplete GBuffer::FBO!" << endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };
};

struct GBuffer {
	const FBO *fbo;
	vector<int> locs;

	inline void startGeometry() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);
	};

	inline void endGeomtry() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

	inline void startLighting() {
		for (int i = 0; i < locs.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, fbo->colorAtts.at(i).colorID);
			glUniform1i(locs.at(i), i);
		};
	};

	inline void endLighting() {
		for (int i = 0; i < locs.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		};
	};

    void getLocs(GLuint programID) {
        for (int i = 0; i < fbo->colorAtts.size(); ++i) {
            locs.push_back(
                glGetUniformLocation(programID, fbo->colorAtts[i].uniform_name.c_str())
            );
            cout << "Loc (" << fbo->colorAtts[i].uniform_name.c_str() << "):" << fbo->colorAtts[i].colorID << endl;
        };
    }
};