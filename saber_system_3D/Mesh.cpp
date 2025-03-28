#include "Mesh.h"
#define MAX_NUM_MESHES 1000
//static mesh_t meshes[MAX_NUM_MESHES];
//static int mesh_count = 0;
std::vector<mesh_t*> meshes;

vec3_t cube_vertices[N_CUBE_VERTICES] =
{
	{-1, -1, -1},
	{-1,  1, -1},
	{ 1,  1, -1},
	{ 1, -1, -1},
	{ 1,  1,  1},
	{ 1,  -1,  1},
	{-1,  1,  1},
	{-1, -1,  1}
};

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{1,  2, 3,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{1,  3, 4,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE },
	// right
	{4,  3, 5,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{4,  5, 6,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE },
	// back
	{ 6,  5, 7,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{ 6,  7, 8,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE },
	// left
	{ 8,  7, 2,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{ 8,  2, 1,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE },
	// top
	{ 2,  7, 5,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{ 2,  5, 3,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE },
	// bottom
	{ 6,  8, 1,{ 0, 1 },{ 0, 0 }, { 1, 0 }, olc::WHITE },
	{ 6,  1, 4,{ 0, 1 },{ 1, 0 }, { 1, 1 }, olc::WHITE }
		// front
  

};

void free_mesh_textures()
{
	if (meshes.size() != 0)
	{
		meshes.clear();
	}
}

mesh_t* get_mesh(int index)
{
	return meshes[index];
}

int get_num_meshes()
{
	return meshes.size();
}

void load_texture_mesh(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t tranlation, vec3_t rotation)
{
	mesh_t* new_mesh = new mesh_t();
	load_obj_file_data(new_mesh, obj_filename);
	load_mesh_png_data(new_mesh, png_filename);
	new_mesh->scale = scale;
	new_mesh->translation = tranlation;
	new_mesh->rotation = rotation;

	meshes.push_back(new_mesh);
}



void load_color_mesh(const char* obj_filename, olc::Pixel p, vec3_t scale, vec3_t tranlation, vec3_t rotation)
{
	mesh_t* new_mesh = new mesh_t();
	load_obj_file_data(new_mesh, obj_filename);
	new_mesh->color = p;
	new_mesh->scale = scale;
	new_mesh->translation = tranlation;
	new_mesh->rotation = rotation;

	meshes.push_back(new_mesh);
}

void load_cube_mesh_data(int side, olc::Pixel color,const char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation)
{
	mesh_t* new_mesh = new mesh_t;
	
	for (int i = 0; i < N_CUBE_VERTICES; i++)
	{
		vec3_t cube_vertex = cube_vertices[i];
		new_mesh->vertices.push_back(cube_vertex);



	}
	
	switch (side)
	{
	case 1: //front
	{
		face_t cube_face0 = cube_faces[0];
		
		new_mesh->faces.push_back(cube_face0);
		face_t cube_face1 = cube_faces[1];
		//cube_face1.color = color;
		new_mesh->faces.push_back(cube_face1);
		new_mesh->color = color;
	}break;

	case 2: //right
	{
		face_t cube_face2 = cube_faces[2];
		//cube_face2.color = color;
		new_mesh->faces.push_back(cube_face2);
		face_t cube_face3 = cube_faces[3];
		//cube_face3.color = color;
		new_mesh->faces.push_back(cube_face3);
		new_mesh->color = color;

	}break;

	case 3: //left
	{
		face_t cube_face6 = cube_faces[6];
		//cube_face6.color = color;
		new_mesh->faces.push_back(cube_face6);
		face_t cube_face7 = cube_faces[7];
		//.color = color;
		new_mesh->faces.push_back(cube_face7);
		new_mesh->color = color;

	}break;

	case 4: //back
	{
		face_t cube_face4 = cube_faces[4];
		///cube_face4.color = color;
		new_mesh->faces.push_back(cube_face4);
		face_t cube_face5 = cube_faces[5];
		//cube_face5.color = color;
		new_mesh->faces.push_back(cube_face5);
		new_mesh->color = color;

	}break;

	case 5: //top
	{
		face_t cube_face8 = cube_faces[8];
		//cube_face8.color = color;
		new_mesh->faces.push_back(cube_face8);
		face_t cube_face9 = cube_faces[9];
		//cube_face9.color = color;
		new_mesh->faces.push_back(cube_face9);
		new_mesh->color = color;

	}break;

	case 6: //bottom
	{
		face_t cube_face10 = cube_faces[10];
		//cube_face10.color = color;
		new_mesh->faces.push_back(cube_face10);
		face_t cube_face11 = cube_faces[11];
		//cube_face11.color = color;
		new_mesh->faces.push_back(cube_face11);

	}break;

	default:
		for (int i = 0; i < N_CUBE_FACES; i++)
		{
			face_t cube_face = cube_faces[i];
			new_mesh->color = cube_face.color;
			new_mesh->faces.push_back(cube_face);
		}
	}
	load_mesh_png_data(new_mesh, png_filename);
	new_mesh->rotation = rotation;
	new_mesh->scale = scale;
	new_mesh->translation = translation;
	
	meshes.push_back(new_mesh);
}

void load_obj_file_data(mesh_t* mesh,const char* filename)
{
	

	FILE* file;
	file = fopen(filename, "r");
	char line[1024];
	std::vector<tex2_t> texcoords;
	
	
	while (fgets(line, 1024, file) != nullptr)
	{
		//vertex information
		if (strncmp(line, "v ", 2) == 0)
		{
			vec3_t vertex;
			sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			
			mesh->vertices.push_back(vertex);
		}
	
		//texture coord information
		if (strncmp(line, "vt ", 3) == 0)
		{
			tex2_t texcoord;
	
			sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
	
			texcoords.push_back(texcoord);
		}
	
	   //face information
		if (strncmp(line, "f ", 2) == 0)
		{
			

			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];
	
				face_t face;
				
				
				sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&vertex_indices[0], &texture_indices[0], &normal_indices[0],
					&vertex_indices[1], &texture_indices[1], &normal_indices[1], 
					&vertex_indices[2], &texture_indices[2], &normal_indices[2]
				);

				if (texture_indices[0] < 0)
				{
					if (strncmp(line, "f ", 2) == 0) {
						int vertex_indices[3];

						sscanf(
							line, "f %d %d %d",
							&vertex_indices[0],
							&vertex_indices[1],
							&vertex_indices[2]
						);

						face_t face = {
							.a = vertex_indices[0],
							.b = vertex_indices[1],
							.c = vertex_indices[2],
							.color = olc::CYAN
						};
						mesh->faces.push_back(face);
					}
				}
				else
				{
					face.a = vertex_indices[0];
					face.b = vertex_indices[1];
					face.c = vertex_indices[2];
					face.a_uv = texcoords[texture_indices[0] - 1];
					face.b_uv = texcoords[texture_indices[1] - 1];
					face.c_uv = texcoords[texture_indices[2] - 1];
					face.color = olc::CYAN;
					face.isVisible = true;
					mesh->faces.push_back(face);
				}
	
			
		}
	
	
	}
	
	texcoords.clear();
	mesh->rotation = { 0,0,0 };
	mesh->scale = { 1.0,1.0,1.0 };
	mesh->translation = { 0,0,0 };
}

void load_mesh_png_data(mesh_t* mesh, const char* filename)
{
	mesh->texture = new olc::Sprite(filename);
}
