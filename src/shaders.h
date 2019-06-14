#ifndef SHADERS_H
#define SHADERS_H



/*
 * VERTEX SHADERS
 */

const GLchar* VS_default = 
  "#version 120\n"
  // attribute is same as in for vertex shader
  "attribute vec4 a_position;\n"
  "attribute vec2 a_texCoords;\n"
  // varying is same as out for vertex shader
  "varying vec3 v_color;\n"
  "varying vec2 v_texCoords;\n"
  // uniforms meaning values same for every vert
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = projection * view * model * vec4(a_position.xyz, 1.0);\n"
  "  v_color = vec3(1.0, 1.0, 1.0);\n"
  "  v_texCoords = a_texCoords;\n"
  "}\n";

const GLchar* VS_colors = 
  "#version 120\n"
  // attribute is same as in for vertex shader
  "attribute vec4 a_position;\n"
  "attribute vec3 a_color;\n"
  // varying is same as out for vertex shader
  "varying vec3 v_color;\n"
  // uniforms meaning values same for every vert
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(a_position.xyz, 1.0);\n"
  "  v_color = a_color;\n"
  "}\n";

const GLchar* VS_transformations = 
  "#version 120\n"
  // attribute is same as in for vertex shader
  "attribute vec4 a_position;\n"
  "attribute vec3 a_color;\n"
  // varying is same as out for vertex shader
  "varying vec3 v_color;\n"
  // uniforms meaning values same for every vert
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = projection * view * model * vec4(a_position.xyz, 1.0);\n"
  "  v_color = a_color;\n"
  "}\n";

const GLchar* VS_texture = 
  "#version 120\n"
  // attribute is same as in for vertex shader
  "attribute vec4 a_position;\n"
  "attribute vec3 a_color;\n"
  "attribute vec2 a_texCoords;\n"
  // varying is same as out for vertex shader
  "varying vec3 v_color;\n"
  "varying vec2 v_texCoords;\n"
  // uniforms meaning values same for every vert
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = projection * view * model * vec4(a_position.xyz, 1.0);\n"
  "  v_color = a_color;\n"
  "  v_texCoords = a_texCoords;\n"
  "}\n";

const GLchar* VS_planes = 
  "#version 120\n"
  // attribute is same as in for vertex shader
  "attribute vec4 a_position;\n"
  "attribute vec3 a_color;\n"
  "attribute vec2 a_texCoords;\n"
  // varying is same as out for vertex shader
  "varying vec3 v_color;\n"
  "varying vec2 v_texCoords;\n"
  // uniforms meaning values same for every vert
  "uniform float planeIndex;\n"
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = projection * view * model * vec4(a_position.xyz, 1.0);\n"
  "  gl_Position += (planeIndex * 0.3);\n"
  "  v_color = a_color;\n"
  "  v_texCoords = a_texCoords;\n"
  "}\n";



/*
 * FRAGMENT SHADERS
 */

const GLchar* FS_default = 
  // TODO(mc): why does this create problems?
  //"precision mediump float;\n"
  // varying is same as in for fragment shader
  "varying vec3 v_color;\n"
  "varying vec2 v_texCoords;\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = vec4(v_color.xyz, 1.0);\n"
  "}\n";

const GLchar* FS_texture = 
  // TODO(mc): why does this create problems?
  //"precision mediump float;\n"
  // varying is same as in for fragment shader
  "varying vec3 v_color;\n"
  "varying vec2 v_texCoords;\n"
  "uniform sampler2D tex;\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = vec4(v_color.xyz, 1.0);\n"
  //"  gl_FragColor = texture2D(tex, v_texCoords);\n"
  "}\n";

#endif  // SHADERS_H
