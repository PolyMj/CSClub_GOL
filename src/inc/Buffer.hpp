#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;

#define DEFAULT_FILTER      GL_NEAREST
#define DEFAULT_WRAPPING    GL_REPEAT


struct ColorAttach {
    string uniform_name;
    int internal; 
    int format; 
    int type; 
    int texFilter;
    int wrapping;
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

    // Create a renderbuffer
    unsigned int createDepthRBO(int width, int height);

    // Create a color attachment (i.e. an element of the frambuffer like color, position, etc.) | See for more details: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    unsigned int createColorAttachment(
        int width, int height,
        int internal, int format, int type,
        int texFilter, int wrapping, int colorAttach);

    // Appends a new set of color attachment parameters
    inline void pushColorAttachment(string unifName, int internal, int format, int type, int texFilter, int wrapping) {
        colorAtts.push_back(ColorAttach{
            unifName,
            internal, format, type,
            texFilter, wrapping
        });
    };

        ///## PRESETS ##///
// COLORS //
    // RGBA | Four-color
    inline void pushRGBAttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
            texFilter, wrapping
        });
    };
    // RGB | Three-color
    inline void pushRGBttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
            texFilter, wrapping
        });
    };
    // RG | Two-color
    inline void pushRGttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG, GL_RGB, GL_UNSIGNED_BYTE,
            texFilter, wrapping
        });
    };
    // Red | One-color
    inline void pushRttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R, GL_RGB, GL_UNSIGNED_BYTE,
            texFilter, wrapping
        });
    };
// FLOATS (16-bit) //
    // Vec4f
    inline void pushVec4Float16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA16F, GL_RGBA, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Vec3f
    inline void pushVec3Float16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB16F, GL_RGB, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Vec2f
    inline void pushVec2Float16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG16F, GL_RG, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Single float
    inline void pushFloat16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R16F, GL_RED, GL_FLOAT,
            texFilter, wrapping
        });
    };
// FLOATS (32-bit) //
    // Vec4f
    inline void pushVec4Float32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA32F, GL_RGBA, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Vec3f
    inline void pushVec3Float32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB32F, GL_RGB, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Vec2f
    inline void pushVec2Float32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG32F, GL_RG, GL_FLOAT,
            texFilter, wrapping
        });
    };
    // Single flaot
    inline void pushFloat32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R32F, GL_RED, GL_FLOAT,
            texFilter, wrapping
        });
    };
// INTS (16-bit) //
    // Vec4 of ints
    inline void pushVec4Int16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA16I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Vec3 of ints
    inline void pushVec3Int16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB16I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Vec2 of ints
    inline void pushVec2Int16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG16I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Single int
    inline void pushInt16Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA16I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
// INTS (32-bit) //
    // Vec4 of ints
    inline void pushVec4Int32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA32I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Vec3 of ints
    inline void pushVec3Int32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB32I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Vec2 of ints
    inline void pushVec2Int32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG32I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };
    // Single int
    inline void pushInt32Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA32I, GL_RGBA, GL_INT,
            texFilter, wrapping
        });
    };

// UINT BYTES //
    // Vec4 of bytes (0-255)
    inline void pushVec4UInt8Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
            texFilter, wrapping
        });
    };
    // Vec3 of bytes (0-255)
    inline void pushVec3UInt8ttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_INT,
            texFilter, wrapping
        });
    };
    // Vec2 of bytes (0-255)
    inline void pushVec2UInt8ttachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_INT,
            texFilter, wrapping
        });
    };
    // Single byte (0-255)
    inline void pushUInt8Attachment(string unifName, int texFilter = DEFAULT_FILTER, int wrapping = DEFAULT_WRAPPING) {
        colorAtts.push_back(ColorAttach{
            unifName,
            GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
            texFilter, wrapping
        });
    };
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