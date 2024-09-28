#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;


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

    FBO() = default;
    FBO(const FBO &other) {
        this->colorAtts = vector<ColorAttach>(other.colorAtts);

        if (other.is_init) {
            this->init(other.width, other.height);
        }
        else {
            this->width = other.width;
            this->height = other.height;
            is_init = false;
        }
    };

    void clear() {
        ID = 0;
        width = 0;
        height = 0;
        colorAtts.clear();
        depthRBO = 0;
    };

    // Bind the FBO to output
	inline void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
	};

    // Unbind the FBO; no longer use as output
	inline void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

    // Called after pushing color attachemnts (see .pushColorAttatchment())
    void init(int width, int height);

    // Create a color attachment (i.e. an element of the frambuffer like color, position, etc.) | See for more details: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    unsigned int createColorAttachment(
        int width, int height,
        int internal, int format, int type,
        int texFilter, int colorAttach);

    // Appends a new set of color attachment parameters
    inline void pushColorAttachment(string unifName, int internal, int format, int type, int texFilter) {
        colorAtts.push_back(ColorAttach{
            unifName,
            internal, format, type,
            texFilter
        });
    };

        ///## PRESETS ##///
// COLORS //
    // RGBA | Four-color
    inline void pushRGBAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA, GL_RGBA, GL_FLOAT,
            texFilter
        });
    };
    // RGB | Three-color
    inline void pushRGBttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
    // RG | Two-color
    inline void pushRGttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
    // Red | One-color
    inline void pushRttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
// FLOATS //
    // Vec4f
    inline void pushVec4fAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA16F, GL_RGBA, GL_FLOAT,
            texFilter
        });
    };
    // Vec3f
    inline void pushVec3fAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB16F, GL_RGB, GL_FLOAT,
            texFilter
        });
    };
    // Vec2f
    inline void pushVec2fAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG16F, GL_RG, GL_FLOAT,
            texFilter
        });
    };
    // Single flaot
    inline void pushFloatAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R16F, GL_R, GL_FLOAT,
            texFilter
        });
    };
// INTS //
    // Vec4 of ints
    inline void pushVec4iAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA8I, GL_RGBA, GL_FLOAT,
            texFilter
        });
    };


// UINT BYTES //
    // Vec4 of bytes (0-255)
    inline void pushVec4xByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
            texFilter
        });
    };
    // Vec3 of bytes (0-255)
    inline void pushVec3xByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_INT,
            texFilter
        });
    };
    // Vec2 of bytes (0-255)
    inline void pushVec2xByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_INT,
            texFilter
        });
    };
    // Single byte (0-255)
    inline void pushByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
            texFilter
        });
    };

// BITS //
    // Vec4 of half-bytes (0-15)
    inline void pushVec4xHByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA4, GL_RED, GL_UNSIGNED_BYTE,
            texFilter
        });
    };
    // Vec4 of 2bit (0-3)
    inline void pushVec4xQByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA2, GL_RED, GL_UNSIGNED_BYTE,
            texFilter
        });
    };
    // Vec3 of half-bytes (0-15)
    inline void pushVec3xHByteAttachment(string unifName, int texFilter = GL_NEAREST) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB4, GL_RED, GL_UNSIGNED_BYTE,
            texFilter
        });
    };

    // Create a renderbuffer
    unsigned int createDepthRBO(int width, int height);
};

struct GBuffer {
	const FBO *fbo;

    // Bind the underlying FBO to output
	inline void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);
	};

    // Unbind the underlying FBO; no longer use as output
	inline void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

    // Uses the GBuffer as input. Make sure to use to correct program with the correct GBuffer
	void use(const vector<GLuint> &locs);

    // Stop using the GBuffer as input
	void unuse(const vector<GLuint> &locs);

    // Get uniform locations of all color attachments from the associated FBO
    void getLocs(GLuint programID, vector<GLuint> &locs);
};

struct GDBuffer : GBuffer {
    const FBO *fboBack;
    int height, width;

    GDBuffer() = default;
    GDBuffer(const FBO &newFbo) {
        if (!newFbo.is_init) {
            cerr << "ERROR: FBO must be initialized before placment in GDBuffer" << endl;
            exit(-1);
        }

        fbo = &newFbo;
        fboBack = new FBO(newFbo);

        this->width = newFbo.width;
        this->height = newFbo.height;
    };

    // Bind the underlying FBO to output
	inline void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fboBack->ID);
	};

    inline void swap() {
        std::swap(fbo, fboBack);
    };
};