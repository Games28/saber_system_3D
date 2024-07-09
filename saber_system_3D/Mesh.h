#ifndef MESH_H
#define MESH_H
#include "Vector.h"
#include "triangle.h"
#include <string>
#include <vector>

//keep for voxel implmentation
#define N_CUBE_VERTICES 8

extern vec3_t cube_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES (6 * 2)
extern face_t cube_faces[N_CUBE_FACES];
/////////////////////////////



typedef struct
{
std::vector<vec3_t> vertices;
std::vector<face_t> faces;
 olc::Sprite* texture;
 olc::Pixel color;
	vec3_t rotation;
	vec3_t scale;
	vec3_t translation;
	int index;
}mesh_t;

void free_mesh_textures();
mesh_t* get_mesh(int index);
int get_num_meshes();
void load_texture_mesh(const char* obj_filename,const char* png_filename,vec3_t scale, vec3_t tranlation, vec3_t rotation);
void load_color_mesh(const char* obj_filename, olc::Pixel p,  vec3_t scale, vec3_t tranlation, vec3_t rotation);
void load_cube_mesh_data();
void load_obj_file_data(mesh_t* mesh,const char* filename);
void load_mesh_png_data(mesh_t* mesh, const char* filename);
#endif // !MESH_H


