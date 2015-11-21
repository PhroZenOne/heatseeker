/*
 * fixeclipse.h
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#ifndef PLAYGROUND_FIXECLIPSE_H_
#define PLAYGROUND_FIXECLIPSE_H_

// Because Eclipse sucks donkey balls do the below to

#define GL_VERTEX_ARRAY_BINDING 0x85B5

typedef void (GLAPIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (GLAPIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (GLAPIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef GLboolean (GLAPIENTRY * PFNGLISVERTEXARRAYPROC) (GLuint array);

#define glBindVertexArray GLEW_GET_FUN(__glewBindVertexArray)
#define glDeleteVertexArrays GLEW_GET_FUN(__glewDeleteVertexArrays)
#define glGenVertexArrays GLEW_GET_FUN(__glewGenVertexArrays)
#define glIsVertexArray GLEW_GET_FUN(__glewIsVertexArray)

#define GLEW_ARB_vertex_array_object GLEW_GET_VAR(__GLEW_ARB_vertex_array_object)

#endif /* PLAYGROUND_FIXECLIPSE_H_ */
