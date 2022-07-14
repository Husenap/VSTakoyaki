#pragma once

#include <string>
#include <variant>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ht::Commands::Locations {

class UniformLocation {
    const GLint       mProgram;
    const std::string mName;

public:
    UniformLocation(GLint program, std::string_view name)
        : mProgram(program)
        , mName(name) {}

    GLint Get() const { return glGetUniformLocation(mProgram, mName.c_str()); }
};

class AttributeLocation {
    const GLint       mProgram;
    const std::string mName;

public:
    AttributeLocation(GLint program, std::string_view name)
        : mProgram(program)
        , mName(name) {}

    GLint Get() const { return glGetAttribLocation(mProgram, mName.c_str()); }
};

class FixedLocation {
    const GLint mLocation;

public:
    FixedLocation(GLint location)
        : mLocation(location) {}

    GLint Get() const { return mLocation; }
};

struct GetLocation {
    template <typename T>
    GLint operator()(const T& location) {
        return location.Get();
    }
};

}  // namespace ht::Commands::Locations

namespace ht::Commands {

class Viewport {
    const GLint   mX;
    const GLint   mY;
    const GLsizei mW;
    const GLsizei mH;

public:
    Viewport(GLint x, GLint y, GLsizei w, GLsizei h)
        : mX(x)
        , mY(y)
        , mW(w)
        , mH(h) {}

    void Apply() { glViewport(mX, mY, mW, mH); }
};

class ClearColor {
    const GLfloat mR;
    const GLfloat mG;
    const GLfloat mB;
    const GLfloat mA;

public:
    ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
        : mR(r)
        , mG(g)
        , mB(b)
        , mA(a) {}

    void Apply() { glClearColor(mR, mG, mB, mA); }
};

class Clear {
    const GLbitfield mMask;

public:
    Clear(GLbitfield mask)
        : mMask(mask) {}

    void Apply() { glClear(mMask); }
};

class UseProgram {
    const GLuint mProgram;

public:
    UseProgram(GLuint program)
        : mProgram(program) {}

    void Apply() { glUseProgram(mProgram); }
};

class BindVertexArray {
    const GLuint mVertexArrayName;

public:
    BindVertexArray(GLuint vertexArrayName)
        : mVertexArrayName(vertexArrayName) {}

    void Apply() { glBindVertexArray(mVertexArrayName); }
};

class VertexAttribPointer {
    using LocationHolder =
        std::variant<Locations::FixedLocation, Locations::AttributeLocation>;

    const LocationHolder mLocation;
    const GLint          mSize;
    const GLenum         mType;
    const int            mNormalized;
    const GLsizei        mStride;
    const GLvoid*        mPointer;

public:
    VertexAttribPointer(GLint         location,
                        GLint         size,
                        GLenum        type,
                        int           normalized,
                        GLsizei       stride,
                        const GLvoid* pointer = nullptr)
        : mLocation(Locations::FixedLocation(location))
        , mSize(size)
        , mType(type)
        , mNormalized(normalized)
        , mStride(stride)
        , mPointer(pointer) {}
    VertexAttribPointer(GLint            program,
                        std::string_view name,
                        GLint            size,
                        GLenum           type,
                        int              normalized,
                        GLsizei          stride,
                        const GLvoid*    pointer = nullptr)
        : mLocation(Locations::AttributeLocation(program, name))
        , mSize(size)
        , mType(type)
        , mNormalized(normalized)
        , mStride(stride)
        , mPointer(pointer) {}

    void Apply() {
        glVertexAttribPointer(std::visit(Locations::GetLocation{}, mLocation),
                              mSize,
                              mType,
                              (unsigned char)mNormalized,
                              mStride,
                              mPointer);
    }
};

class EnableVertexAttribArray {
    using LocationHolder =
        std::variant<Locations::FixedLocation, Locations::AttributeLocation>;

    const LocationHolder mLocation;

public:
    EnableVertexAttribArray(GLint location)
        : mLocation(Locations::FixedLocation(location)) {}
    EnableVertexAttribArray(GLint program, std::string_view name)
        : mLocation(Locations::AttributeLocation(program, name)) {}

    void Apply() {
        glEnableVertexAttribArray(
            std::visit(Locations::GetLocation{}, mLocation));
    }
};

class DrawArrays {
    const GLenum  mMode;
    const GLint   mFirst;
    const GLsizei mCount;

public:
    DrawArrays(GLenum mode, GLint first, GLsizei count)
        : mMode(mode)
        , mFirst(first)
        , mCount(count) {}

    void Apply() { glDrawArrays(mMode, mFirst, mCount); }
};

class Uniform {
    using LocationHolder =
        std::variant<Locations::FixedLocation, Locations::UniformLocation>;
    using Data = std::variant<GLint,
                              glm::ivec2,
                              glm::ivec3,
                              glm::ivec4,
                              GLfloat,
                              glm::vec2,
                              glm::vec3,
                              glm::vec4,
                              glm::mat2,
                              glm::mat3,
                              glm::mat4>;
    const LocationHolder mLocation;
    const Data           mData;

public:
    Uniform(GLint location, Data&& data)
        : mLocation(Locations::FixedLocation(location))
        , mData(data) {}
    Uniform(GLint program, std::string_view name, Data&& data)
        : mLocation(Locations::UniformLocation(program, name))
        , mData(data) {}

    void Apply() {
        GLint location = std::visit(Locations::GetLocation{}, mLocation);

        std::visit(
            overloaded{
                [location](GLint data) { glUniform1i(location, data); },
                [location](glm::ivec2 data) {
                    glUniform2iv(location, 1, glm::value_ptr(data));
                },
                [location](glm::ivec3 data) {
                    glUniform3iv(location, 1, glm::value_ptr(data));
                },
                [location](glm::ivec4 data) {
                    glUniform4iv(location, 1, glm::value_ptr(data));
                },
                [location](GLfloat data) { glUniform1f(location, data); },
                [location](glm::vec2 data) {
                    glUniform2fv(location, 1, glm::value_ptr(data));
                },
                [location](glm::vec3 data) {
                    glUniform3fv(location, 1, glm::value_ptr(data));
                },
                [location](glm::vec4 data) {
                    glUniform4fv(location, 1, glm::value_ptr(data));
                },
                [location](glm::mat2 data) {
                    glUniformMatrix2fv(
                        location, 1, GL_FALSE, glm::value_ptr(data));
                },
                [location](glm::mat3 data) {
                    glUniformMatrix3fv(
                        location, 1, GL_FALSE, glm::value_ptr(data));
                },
                [location](glm::mat4 data) {
                    glUniformMatrix4fv(
                        location, 1, GL_FALSE, glm::value_ptr(data));
                },
            },
            mData);
    }
};

class BindFramebuffer {
    const GLuint mFramebufferName;

public:
    BindFramebuffer(GLuint framebufferName)
        : mFramebufferName(framebufferName) {}

    void Apply() { glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferName); }
};

}  // namespace ht::Commands
