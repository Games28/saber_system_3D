#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "depthdrawer.h"
#include "Vector.h"
#include "Mesh.h"
#include "Graphics.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "camera.h"
#include "clipping.h"


#define WINDOW_HEIGHT 300
#define WINDOW_WIDTH 400
#define M_PI 3.14159f
#define  N_POINTS 730
#define MAX_TRIANGLES 10000

//voxel testing stuff


class graphics3D : public olc::PixelGameEngine
{
public:
	graphics3D()
	{
		sAppName = "3d Graphics";
	}
public:
	
	std::vector<triangle_t> trangles_to_render;
	
	Render_Method render_method = RENDER_FILL_TRIANGLE_WIRE;
	Cull_Method cull_method = CULL_BACKFACE;
	lighting_method light_method = FLAT_SHADING;
	mat4_t proj_matrix;
	mat4_t view_matrix;
	mat4_t world_matrix;
	DepthDrawer depth_draw;
	float zoom_factor = 0.0f;
	int num_triangles_to_render = 0;
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	
	//voxel stuff
	static const int max_count = 5;
	
	
public:
	
	void process_graphics_pipline_stages(mesh_t* mesh, int index)
	{
			//creates matrix to be used with mesh calculation
		mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
		mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
		mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
		mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
		mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);


		//initialize the target looking at the positive z-axis
		//if (index == 1)
		//{
		//	vec3_t target = mesh->translation;
		//
		//	vec3_t up_direction = { 0,1,0 };
		//	//create view matrix
		//	view_matrix = mat4_look_at(get_camera_position(), target, up_direction);
		//
		//}
		vec3_t target = get_camera_lookat_target();

		vec3_t up_direction = { 0,1,0 };
		//create view matrix
		view_matrix = mat4_look_at(get_camera_position(), target, up_direction);
		//trangles_to_render.clear();

		int num_faces = (int)mesh->faces.size();

		//face = one triangle, 12 = one cube/box
		for (int i = 0; i < num_faces; i++)
		{

			//gets the 3d face point (3 in total) of the 3 point triangle
			face_t mesh_face = mesh->faces[i];
			vec3_t face_vertices[3];

			//gets the x,y,z coords for each face triangle point location. as a unit length (1)
			//  from the origin(center)
			face_vertices[0] = mesh->vertices[mesh_face.a];
			face_vertices[1] = mesh->vertices[mesh_face.b];
			face_vertices[2] = mesh->vertices[mesh_face.c];



			//creates an array of vector 3 to store new coord x,y,z info with new calculation of 
			// rotation and orientation of triangle face.
			vec4_t transformed_vertices[3];
			for (int j = 0; j < 3; j++)
			{
				vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

				//creating world matrix to combine scaling, rotation and translation

				world_matrix = mat4_identity();

				//frist scaling
				world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
				//seond rotation
				world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
				world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
				world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
				//third movement/translation
				world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

				//multiply the world matrix by the original vector
				transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

				//multiply the view matrix by the vector to transform the scene to camera space
				transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

				//save transformed vertex in the array of transformed vertices
				transformed_vertices[j] = transformed_vertex;
			}


			vec3_t face_normal = get_triangle_normal(transformed_vertices);



			if (cull_method == CULL_BACKFACE)
			{

				//find the vector between a point in the triangle (A) and the camera position
				vec3_t camera_ray = vec3_sub(vec3_new(0, 0, 0), vec3_from_vec4(transformed_vertices[0]));

				//Calculate how aligned the camera ray is with the face normal (using dot product)
				float dot_normal_camera = vec3_dot(face_normal, camera_ray);

				if (dot_normal_camera < 0)
				{
					continue;
				}

			}

			//create a polygon from the original transformed triangle to be clipped;
			polygon_t polygon = create_polygon_from_triangle(
				vec3_from_vec4(transformed_vertices[0]),
				vec3_from_vec4(transformed_vertices[1]),
				vec3_from_vec4(transformed_vertices[2]),
				mesh_face.a_uv,
				mesh_face.b_uv,
				mesh_face.c_uv
			);


			//clip the polygon and returns a new polygon with potential new vertices
			clip_polygon(&polygon);

			//break the clipped polygon apart back into individual triangles
			triangle_t trangles_after_clipping[10];
			int num_triangles_after_clipping = 0;

			triangles_from_polygon(&polygon, trangles_after_clipping, &num_triangles_after_clipping);

			//loop all assembled triangles after clipping

			for (int t = 0; t < num_triangles_after_clipping; t++)
			{
				triangle_t triangle_after_clipping = trangles_after_clipping[t];


				vec4_t projected_points[3];
				//loop all three vertices to preform projection
				for (int j = 0; j < 3; j++)
				{
					// project the current vertex

					projected_points[j] = mat4_mul_vec4(proj_matrix, triangle_after_clipping.points[j]);

					if (projected_points[j].w != 0) {
						projected_points[j].x /= projected_points[j].w;
						projected_points[j].y /= projected_points[j].w;
						projected_points[j].z /= projected_points[j].w;
					}

					//invert the y values to account for flipped screen y corrdinate
					projected_points[j].y *= -1;


					projected_points[j].x *= (float)(ScreenWidth() / 2);
					projected_points[j].y *= (float)(ScreenHeight() / 2);




					//translating the projected points to the middle of the screen
					projected_points[j].x += (float)(ScreenWidth() / 2);
					projected_points[j].y += (float)(ScreenHeight() / 2);





				}





				float light_intensity_factor = -vec3_dot(face_normal, get_Light_direction());


				olc::Pixel triangle_color = light_apply_intensity(mesh->color, light_intensity_factor);

				triangle_t projected_triangle =
				{
					{
					 {projected_points[0].x, projected_points[0].y, projected_points[0].z,projected_points[0].w},
					 {projected_points[1].x, projected_points[1].y, projected_points[1].z,projected_points[1].w},
					 {projected_points[2].x, projected_points[2].y, projected_points[2].z,projected_points[2].w}
					},
					{
					 {triangle_after_clipping.texcoords[0].u,triangle_after_clipping.texcoords[0].v},
					 {triangle_after_clipping.texcoords[1].u,triangle_after_clipping.texcoords[1].v},
					 {triangle_after_clipping.texcoords[2].u,triangle_after_clipping.texcoords[2].v}
					},
					triangle_color,
					mesh->texture

				};
				//currently working

				trangles_to_render.push_back(projected_triangle);



			}


		}
	}
	bool check_full_mesh_visiblity(mesh_t* mesh, int mesh_index)
	{
		

		return true;
	}
public:

	bool OnUserCreate() override
	{
		
		init_light(vec3_new(0, 0, 1));

		float aspectx = (float)ScreenWidth() / (float)ScreenHeight();
		float aspecty = (float)ScreenHeight() / (float)ScreenWidth();
		float fovy = M_PI / 3.0f; // the same as 180/3 or 60degs
		float fovx = (atan(fovy / 2) * aspectx) * 2.0f;
		float znear = 0.5f;
		float zfar = 20.0f;


		proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

		//initialize frustum planes with a point and a normal
		init_frustum_planes(fovx,fovy, znear, zfar);
		
		//voxel testing code/////////////////////////////////////////////////////////////////////////////
		
	
		load_color_mesh("cube.obj", olc::CYAN, vec3_new(1.0f, 1.0f, 1.0f), vec3_new(0, 0, +8), vec3_new(0, 0, 0));
		
		load_color_mesh("cube.obj", olc::GREY, vec3_new(1.0f, 0.2f, 1.0f),
			vec3_new(get_camera_position().x, get_camera_position().y, get_camera_position().z + 4), vec3_new(0, 0, 0));
	



		///////////////////////////////////////////////////////////////////////////////////////////////////
		depth_draw.Init(this);
		
		return true;
	}

	void input(float deltatime)
	{
		//rendering type
		if (GetKey(olc::F1).bPressed) { render_method = RENDER_WIRE_VERTEX; };
		if (GetKey(olc::F2).bPressed) { render_method = RENDER_WIRE; };
		if (GetKey(olc::F3).bPressed) { render_method = RENDER_FILL_TRIANGLE; };
		if (GetKey(olc::F4).bPressed) { render_method = RENDER_FILL_TRIANGLE_WIRE; };
		if (GetKey(olc::F5).bPressed) { render_method = RENDER_TEXTURED; };
		if (GetKey(olc::F6).bPressed) { render_method = RENDER_TEXTURED_WIRE; };
		if (GetKey(olc::C).bPressed) { cull_method = CULL_BACKFACE; };
		if (GetKey(olc::D).bPressed) { cull_method = CULL_NONE; };
		//shading type
		if (GetKey(olc::NP0).bPressed) { light_method = NO_LIGHTING; };
		if (GetKey(olc::NP1).bPressed) { light_method = FLAT_SHADING; };

		//camera controls
		
		if (GetKey(olc::A).bHeld) { rotate_camera_yaw(-1.0f * deltatime); };
		if (GetKey(olc::D).bHeld) { rotate_camera_yaw(+1.0f * deltatime); };

		//fps look up and down
		//if (GetKey(olc::UP).bHeld) { rotate_camera_pitch(+3.0 * deltatime); };
		//if (GetKey(olc::DOWN).bHeld) { rotate_camera_pitch(-3.0 * deltatime); };

		//object movement controls
		if (GetKey(olc::I).bHeld) {
			offsetY = -0.05f;
		}
		if (GetKey(olc::K).bHeld) {
			offsetY = +0.05f;
		}
		if (GetKey(olc::J).bHeld) {
			offsetX = -0.05f;
		}
		if (GetKey(olc::L).bHeld) {
			offsetX = +0.05f;
		}
		if (GetKey(olc::I).bReleased) {
			offsetY = 0.0f;
		}
		if (GetKey(olc::K).bReleased) {
			offsetY = 0.0f;
		}
		if (GetKey(olc::J).bReleased) {
			offsetX = 0.0f;
		}
		if (GetKey(olc::L).bReleased) {
			offsetX = 0.0f;
		}

		if(GetKey(olc::Q).bHeld) { update_camera_position(vec3_new(-3.0f * deltatime,0.0f, 0.0f)); };
		if (GetKey(olc::E).bHeld) { update_camera_position(vec3_new(+3.0f * deltatime, 0.0f, 0.0f)); };
		if (GetKey(olc::UP).bHeld) { update_camera_position(vec3_new(0.0f,+3.0f * deltatime,0.0f)); };
		if (GetKey(olc::DOWN).bHeld) { update_camera_position(vec3_new(0.0f, -3.0f * deltatime, 0.0f)); };
		if (GetKey(olc::W).bHeld) 
		{ 
			set_new_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0f * deltatime));
			set_new_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
		};
		if (GetKey(olc::S).bHeld) 
		{ 
			set_new_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0f * deltatime));
			set_new_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
		};
	}


	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		input(fElapsedTime);


		
		num_triangles_to_render = 0;
		
		


		//update and render visible mesh_faces;
		for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++)
		{
			mesh_t* mesh = get_mesh(mesh_index);
			

			
			if (mesh_index == 1)
			{
				mesh->rotation.x += offsetX * fElapsedTime;
				mesh->rotation.y += offsetY * fElapsedTime;
				
				
				mesh->translation = vec3_new(get_camera_position().x , get_camera_position().y, get_camera_position().z + 4);
				
			 }
			
			//mesh->rotation.x += 0.5f * fElapsedTime;
		  // mesh->rotation.y += 2.0f * fElapsedTime;
		    //mesh->rotation.z += 0.5f * fElapsedTime;
		//mesh.scale.x += 0.02f* fElapsedTime;
		//mesh.scale.y += 0.02f* fElapsedTime;
		//mesh.translation.x += 0.01f;

		

		
			process_graphics_pipline_stages(mesh, mesh_index);
			
		}
			// currently working stuff
			int t_length = (int)trangles_to_render.size();

			for (int i = 0; i < t_length; i++)
			{
				triangle_t triangle = trangles_to_render[i];


				if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
				{
					
					//testing
					draw_triangle_fill(depth_draw,
						this,
						(int)triangle.points[0].x, (int)triangle.points[0].y, (int)triangle.points[0].z, (int)triangle.points[0].w,
						(int)triangle.points[1].x, (int)triangle.points[1].y, (int)triangle.points[1].z, (int)triangle.points[1].w,
						(int)triangle.points[2].x, (int)triangle.points[2].y, (int)triangle.points[2].z, (int)triangle.points[2].w,
						triangle.p
					);
				}

				if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
				{
					draw_textured_triangle(depth_draw,
						this,
						(int)triangle.points[0].x, (int)triangle.points[0].y, (int)triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
						(int)triangle.points[1].x, (int)triangle.points[1].y, (int)triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
						(int)triangle.points[2].x, (int)triangle.points[2].y, (int)triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
						triangle.texture
					);
				}


				if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE ||
					render_method == RENDER_TEXTURED_WIRE)
				{
					draw_triangle(
						this,
						(int)triangle.points[0].x,
						(int)triangle.points[0].y,
						(int)triangle.points[1].x,
						(int)triangle.points[1].y,
						(int)triangle.points[2].x,
						(int)triangle.points[2].y,
						triangle.p
					);
				}

				if (render_method == RENDER_WIRE_VERTEX)
				{
					draw_rect(this, (int)triangle.points[0].x, (int)triangle.points[0].y, 4, 4, olc::RED);
					draw_rect(this, (int)triangle.points[1].x, (int)triangle.points[1].y, 4, 4, olc::RED);
					draw_rect(this, (int)triangle.points[2].x, (int)triangle.points[2].y, 4, 4, olc::RED);
				}
			}
		
		trangles_to_render.clear();
		
		depth_draw.Reset();

		return true;
	}

	bool OnUserDestroy() override
	{
		
		free_mesh_textures();
		return true;
	}
};


int main()
{

	graphics3D demo;
	if (demo.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 2, 2))
	{
		demo.Start();
	}
}