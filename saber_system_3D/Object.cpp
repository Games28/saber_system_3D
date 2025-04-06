#include "Object.h"

Object::Object(vec3_t scale, vec3_t translation, vec3_t rotation)
{
	this->scale = scale;
	this->translation = translation;
	this->rotation = rotation;
}

void Object::load_cube_mesh(int side, olc::Pixel color, const char* png_filename)
{
	mesh = load_cube_mesh_data(side, color, png_filename,scale,translation,rotation);
	int i = 0;
	
}

void Object::load_obj_mesh(const char* obj_filename, const char* png_filename)
{
	load_texture_mesh(obj_filename, png_filename, scale, translation, rotation);
}

void Object::process_graphics_pipline_stages(olc::PixelGameEngine* pge)
{
	mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);



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
		face_vertices[0] = mesh->vertices[mesh_face.a - 1];
		face_vertices[1] = mesh->vertices[mesh_face.b - 1];
		face_vertices[2] = mesh->vertices[mesh_face.c - 1];



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

			std::cout << "dot normal camera: " << dot_normal_camera << std::endl;
			//DrawString(30, 30, "dot normal camera:" + std::to_string(dot_normal_camera));

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


				projected_points[j].x *= (float)(pge->ScreenWidth() / 2);
				projected_points[j].y *= (float)(pge->ScreenHeight() / 2);




				//translating the projected points to the middle of the screen
				projected_points[j].x += (float)(pge->ScreenWidth() / 2);
				projected_points[j].y += (float)(pge->ScreenHeight() / 2);





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
			int i = 0;


		}


	}
}

void Object::Render(olc::PixelGameEngine* pge, DepthDrawer& depth_draw)
{
	int m = get_num_meshes();


	
	// currently working stuff
	int t_length = (int)trangles_to_render.size();


	






	for (int i = 0; i < t_length; i++)
	{
		triangle_t triangle = trangles_to_render[i];


		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{

			//testing
			draw_filled_triangle(depth_draw,
				pge,
				(int)triangle.points[0].x, (int)triangle.points[0].y, (int)triangle.points[0].z, (int)triangle.points[0].w,
				(int)triangle.points[1].x, (int)triangle.points[1].y, (int)triangle.points[1].z, (int)triangle.points[1].w,
				(int)triangle.points[2].x, (int)triangle.points[2].y, (int)triangle.points[2].z, (int)triangle.points[2].w,
				triangle.p
			);
		}

		if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
		{
			draw_textured_triangle(depth_draw,
				pge,
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
				pge,
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
			draw_rect(pge, (int)triangle.points[0].x, (int)triangle.points[0].y, 4, 4, olc::RED);
			draw_rect(pge, (int)triangle.points[1].x, (int)triangle.points[1].y, 4, 4, olc::RED);
			draw_rect(pge, (int)triangle.points[2].x, (int)triangle.points[2].y, 4, 4, olc::RED);
		}
	}

	trangles_to_render.clear();

	
}

void Object::Update(olc::PixelGameEngine* pge, float deltatime)
{
	




		process_graphics_pipline_stages(pge);

	
}
