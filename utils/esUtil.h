//
// Book: OpenGL(R) ES 2.0 Programming Guide
// Authors:  Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:  0321502795
// ISBN-13:  9780321502797
// Publisher: Addison-Wesley Professional
// URLs: http://safari.informit.com/9780321563835
// http://www.opengles-book.com
//

//
/// \file ESUtil.h
/// \brief A utility library for OpenGL ES. This library provides a
/// basic common framework for the example applications in the
/// OpenGL ES 2.0 Programming Guide.
//
#ifndef ESUTIL_H
#define ESUTIL_H

///
// Includes
//
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#ifdef __cplusplus

extern "C" {
#endif


///
// Macros
//
#define ESUTIL_API
#define ESCALLBACK


/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB  0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA  1
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH  2
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL  4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE  8


///
// Types
//

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef struct {
   GLfloat  m[4][4];
} ESMatrix;


typedef struct {
   // The mixed program object, ie for rendering both webcam and ir.
   GLuint mixedProgramObject;

   GLuint irOnlyProgramObject;

   GLuint webcamOnlyProgramObject;

   // Attribute locations
   GLint positionLoc;
   GLint texCoordLoc;

   // Sampler locations
   GLint cameraMapLoc;
   GLint irMapLoc;
   GLint irTransformLoc;

   // Texture handle
   GLuint cameraMapTexId;
   GLuint irMapTexId;

} GlData;

typedef struct _escontext {
   //GL handles and other dynamic stuff
   GlData * glData;

   /// Window width
   GLint width;

   /// Window height
   GLint height;

   /// Window handle
   EGLNativeWindowType hWnd;

   /// EGL display
   EGLDisplay eglDisplay;

   /// EGL context
   EGLContext eglContext;

   /// EGL surface
   EGLSurface eglSurface;

   /// Callbacks
   void (ESCALLBACK *drawFunc)(struct _escontext *);
   void (ESCALLBACK *keyFunc)(struct _escontext *, unsigned char, int, int);
   void (ESCALLBACK *updateFunc)(struct _escontext *, float deltaTime);
} ESContext;


///
// Public Functions
//

//
///
/// \brief Initialize ES framework context. This must be called before calling any other functions.
/// \param esContext Application context
//
void ESUTIL_API esInitContext(ESContext *esContext);

//
/// \brief Create a window with the specified parameters
/// \param esContext Application context
/// \param title Name for title bar of window
/// \param width Width in pixels of window to create
/// \param height Height in pixels of window to create
/// \param flags Bitfield for the window creation flags
/// ES_WINDOW_RGB  - specifies that the color buffer should have R,G,B channels
/// ES_WINDOW_ALPHA  - specifies that the color buffer should have alpha
/// ES_WINDOW_DEPTH  - specifies that a depth buffer should be created
/// ES_WINDOW_STENCIL - specifies that a stencil buffer should be created
/// ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
/// \return GL_TRUE if window creation is succesful, GL_FALSE otherwise
GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags);


//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//
GLuint ESUTIL_API esLoadShader(GLenum type, const char *shaderSrc);

//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
/// Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//
GLuint ESUTIL_API esLoadProgram(const char *vertShaderSrc, const char *fragShaderSrc);



#ifdef __cplusplus
}
#endif

#endif // ESUTIL_H
