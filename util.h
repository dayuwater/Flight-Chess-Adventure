//
//  util.h
//  Assignment-5
//
//  Updated by Robert Falk on 10/25/14.
//
//

#ifndef Assignment_2_util_h
#define Assignment_2_util_h


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#import <Foundation/Foundation.h>
#import <AppKit/NSImage.h>
#else
#endif

#include "Vec3.h"

#include <stdio.h>
#include <sys/stat.h>
#include <string>

using namespace std;


// Global Variables
string g_compile_link_log;
GLsizei g_window_width;
GLsizei g_window_height;

/**********************************************************************
 * Load an image and return a texture id or 0 on failure
 **********************************************************************/
GLuint getTextureFromFile(const char *fname)
{
    NSString *str = [[NSString alloc] initWithCString:fname encoding:NSASCIIStringEncoding];
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:str];
    
    // error loading image
    if (![image isValid])
        return 0;
    
    //Only support square images for opengl textures:
    //if ([image size].width != [image size].height) {
    //    return 0;
    //}
    
    //int width = [image size].width;
    // And images must have a size that is a power of 2 (e.g. 32x32, 64x64 128x128, etc)
    // if ((width & (width - 1)) != 0)
    //    return 0;
    
    int texformat = GL_RGB;
    
    // convert our NSImage to a NSBitmapImageRep
    NSBitmapImageRep * imgBitmap =[ [ NSBitmapImageRep alloc ]initWithData: [ image TIFFRepresentation ] ];
    
    //[ imgBitmap retain ];
    // examine & remap format
    switch( [ imgBitmap samplesPerPixel ] )
    {
        case 4:texformat = GL_RGBA;
            break;
        case 3:texformat = GL_RGB;
            break;
        case 2:texformat = GL_LUMINANCE_ALPHA;
            break;
        case 1:texformat = GL_LUMINANCE;
            break;
        default:
            break;
    }
    
    
    GLuint tex_id;
    
    glGenTextures (1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, texformat, [image size].width, [image size].height,
                 0, texformat, GL_UNSIGNED_BYTE, [ imgBitmap bitmapData ]);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return tex_id;
}



bool glErrorCheck()
{
    GLenum e = glGetError();
    std::string err_string;
    
    switch (e) {
        case GL_NO_ERROR:
            return false;
        case GL_INVALID_ENUM:
            err_string = "Invalid Enum";
            break;
        case GL_INVALID_VALUE:
            err_string = "Invalid Value";
            break;
        case GL_INVALID_OPERATION:
            err_string = "Invalid Value";
            break;
        case GL_STACK_OVERFLOW:
            err_string = "Invalid Value";
            break;
        case GL_STACK_UNDERFLOW:
            err_string = "Invalid Value";
            break;
        case GL_OUT_OF_MEMORY:
            err_string = "Invalid Value";
            break;
        case GL_TABLE_TOO_LARGE:
            err_string = "Invalid Value";
            break;
        default:
            err_string = "Undefined Error";
            break;
    }
    
    fprintf(stderr,"%s\n",err_string.c_str());
    
    return true;
}

void drawGlutString(const char* str)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_window_width, 0, g_window_height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glUseProgram(0);
    glDisable(GL_TEXTURE_2D);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    
    int h = 15;
    glRasterPos2i(3, g_window_height-h);
    
    const char *c = str;
    int pos[4];
    
    glGetIntegerv(GL_CURRENT_RASTER_POSITION, pos);
    
    while (*c != 0 && pos[1] > 0)
    {
        glGetIntegerv(GL_CURRENT_RASTER_POSITION, pos);
        if (pos[0] > g_window_width-h || *c=='\n')
        {
            glRasterPos2i(3, pos[1]-h);
        }
        
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        ++c;
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
    
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(stderr, "%s\n",infoLog);
        g_compile_link_log += string("\n") + infoLog;
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
    
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(stderr,"%s\n",infoLog);
        g_compile_link_log += string("\n") + infoLog;
        free(infoLog);
    }
}

GLuint createShader(std::string file_name)
{
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint pixel_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    
    // obtain file sizes:
    int fsize1, fsize2, bytes_read;
    std::string vert_shader = file_name + ".vert";
    std::string frag_shader = file_name + ".frag";
    
    struct stat statBuf;
    stat(vert_shader.c_str(), &statBuf);
    fsize1 = statBuf.st_size;
    stat(frag_shader.c_str(), &statBuf);
    fsize2 = statBuf.st_size;
    
    FILE *f1 = fopen(vert_shader.c_str(), "r");
    FILE *f2 = fopen(frag_shader.c_str(), "r");
    if (!f1 || !f2)
        return 0;
    
    // allocate memory to contain the whole file:
    GLchar * shader_buffer[1];
    shader_buffer[0] = (GLchar*)malloc(std::max(fsize1, fsize2));
    
    // Read in the fragment shader
    bytes_read = fread ((void*)shader_buffer[0],1,fsize2,f2);
    glShaderSource(pixel_shader, 1, (const GLchar**)shader_buffer, &bytes_read);
    
    // Read in the vertex shader
    bytes_read = fread ((void*)shader_buffer[0],1,fsize1,f1);
    glShaderSource(vertex_shader, 1, (const GLchar**)shader_buffer, &bytes_read);
    
    free(shader_buffer[0]);
    
    GLint status;
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        g_compile_link_log += vert_shader + ": ";
        printShaderInfoLog(vertex_shader);
        return 0;
    }
    glCompileShader(pixel_shader);
    glGetShaderiv(pixel_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        g_compile_link_log += frag_shader + ": ";
        printShaderInfoLog(pixel_shader);
        return 0;
    }
    
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program,vertex_shader);
    glAttachShader(shader_program,pixel_shader);
    
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        g_compile_link_log += file_name + " link error: ";
        printProgramInfoLog(shader_program);
    }
    
    return shader_program;
}



// Draw a room aligned with the x,y, and z axes from min_xyz to max_xyz
void drawRoom(const Vec3f& min_xyz, const Vec3f& max_xyz) {
 }


#endif
