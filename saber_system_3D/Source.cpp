#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//#include "depthdrawer.h"
//#include "Vector.h"
//#include "Mesh.h"
#include "Graphics.h"
//#include "matrix.h"
//#include "light.h"
#include "texture.h"
//#include "triangle.h"
#include "camera.h"
//#include "clipping.h"
//#include "aabb3.h"
#include "Object.h"
#include "aabb3.h"


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


	std::vector<Object*> objectlist;
	
	DepthDrawer depth_draw;

	Render_Method render_method = RENDER_FILL_TRIANGLE;
	Cull_Method cull_method = CULL_BACKFACE;
	lighting_method light_method = FLAT_SHADING;
	mat4_t proj_matrix;
	mat4_t view_matrix;
	mat4_t world_matrix;
	
	float zoom_factor = 0.0f;
	int num_triangles_to_render = 0;
	float offsetX = 0.0f;
	float offsetY = 0.0f;

	//collsion detection

	

	//aabb3 collsion test
	vec3_t cam_prev = { 0 }, cam_curr = { 0 }, cam_acc = { 0 }, cam_vel = { 0 };

	//gravity force
	vec3_t gravity = { 0, -9.8, 0 };
	
	//voxel stuff
	static const int max_count = 5;
	
	
public:
	
	void process_graphics_pipline_stages(Object* obj, int index)
	{
			//creates matrix to be used with mesh calculation
		mat4_t scale_matrix;
		mat4_t translation_matrix;
		mat4_t rotation_matrix_x;
		mat4_t rotation_matrix_y;
		mat4_t rotation_matrix_z;
		vec3_t target;
		
		if (obj->ispickedup == true)
		{
			
			target = obj->translation;
			scale_matrix = mat4_make_scale(obj->mesh->scale.x, obj->mesh->scale.y, obj->mesh->scale.z);
			translation_matrix = mat4_make_translation(obj->mesh->translation.x, obj->mesh->translation.y, obj->mesh->translation.z);
			rotation_matrix_x = mat4_make_rotation_x(obj->mesh->rotation.x);
			rotation_matrix_y = mat4_make_rotation_y(obj->mesh->rotation.y);
			rotation_matrix_z = mat4_make_rotation_z(obj->mesh->rotation.z);
		}
		else
		{
			target = get_camera_lookat_target();
			scale_matrix = mat4_make_scale(obj->mesh->scale.x, obj->mesh->scale.y, obj->mesh->scale.z);
			translation_matrix = mat4_make_translation(obj->mesh->translation.x, obj->mesh->translation.y, obj->mesh->translation.z);
			rotation_matrix_x = mat4_make_rotation_x(obj->mesh->rotation.x);
			rotation_matrix_y = mat4_make_rotation_y(obj->mesh->rotation.y);
			rotation_matrix_z = mat4_make_rotation_z(obj->mesh->rotation.z);
		}
		vec3_t up_direction = { 0,1,0 };
		//create view matrix
		view_matrix = mat4_look_at(get_camera_position(), target, up_direction);
		//trangles_to_render.clear();
		
		int num_faces = (int)obj->mesh->faces.size();
		
		//face = one triangle, 12 = one cube/box
		for (int i = 0; i < num_faces; i++)
		{
		
			//gets the 3d face point (3 in total) of the 3 point triangle
			face_t mesh_face = obj->mesh->faces[i];
			vec3_t face_vertices[3];
		
			//gets the x,y,z coords for each face triangle point location. as a unit length (1)
			//  from the origin(center)
			face_vertices[0] = obj->mesh->vertices[mesh_face.a - 1];
			face_vertices[1] = obj->mesh->vertices[mesh_face.b - 1];
			face_vertices[2] = obj->mesh->vertices[mesh_face.c - 1];
		
		
		
			//creates an array of vector 3 to store new coord x,y,z info with new calculation of 
			// rotation and orientation of triangle face.
			vec4_t transformed_vertices[3];
			for (int j = 0; j < 3; j++)
			{
				vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

				//creating world matrix to combine scaling, rotation and translation

				world_matrix = mat4_identity();
				if (obj->ispickedup)
				{
					world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);

					//third movement/translation
					world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

					//seond rotation
					world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
					world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
					world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);

				}
				else
				{

					//frist scaling
					world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
					//seond rotation
					world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
					world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
					world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
					//third movement/translation
					world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
					//frist scaling
					
				}
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
		
		
					projected_points[j].x *= (float)(ScreenWidth() / 2);
					projected_points[j].y *= (float)(ScreenHeight() / 2);
		
		
		
		
					//translating the projected points to the middle of the screen
					projected_points[j].x += (float)(ScreenWidth() / 2);
					projected_points[j].y += (float)(ScreenHeight() / 2);
		
		
		
		
		
				}
		
		
		        //rigidbody stuff
				
		
		
				float light_intensity_factor = -vec3_dot(face_normal, get_Light_direction());
		
		
				olc::Pixel triangle_color = light_apply_intensity(obj->mesh->color, light_intensity_factor);
		
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
					obj->mesh->texture
		
				};
				//currently working
		
				objectlist[0]->trangles_to_render.push_back(projected_triangle);
		
		
		
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

		init_camera();
		int tilecount = 10;
		enum TriangleSides
		{
			DEFUALT,
			FRONT,
			RIGHT,
			LEFT,
			BACK,
			TOP,
			BOTTOM
		};
		init_light(vec3_new(0, 0, 1));

		float aspectx = (float)ScreenWidth() / (float)ScreenHeight();
		float aspecty = (float)ScreenHeight() / (float)ScreenWidth();
		float fovy = M_PI / 3.0f; // the same as 180/3 or 60degs
		float fovx = (atan(fovy / 2) * aspectx) * 2.0f;
		float znear = 0.1f;
		float zfar = 100.0f;
		

		proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

		//initialize frustum planes with a point and a normal
		init_frustum_planes(fovx,fovy, znear, zfar);
		
		//voxel testing code/////////////////////////////////////////////////////////////////////////////
		
		//does not work
		//load_texture_mesh("assets/snowspeeder.obj", "assets/Textspeeder.png", vec3_new(1.0f, 1.0f, 1.0f), vec3_new(0.0f, 0.0f, + 4), vec3_new(0, 0, 0));
			
		//does work 
		//load_color_mesh("assets/mountains.obj", olc::CYAN, vec3_new(1.0f, 1.0f, 1.0f), vec3_new(-5, -14, +7), vec3_new(0, 0, 0));
		
		//does work
		//load_texture_mesh("assets/drone.obj", "assets/drone.png", vec3_new(1.0f, 1.0f, 1.0f),
			//vec3_new(0.0f, 0.0f, + 4), vec3_new(0, 0, 0));


		///////////////////////////////////////////////////////////////////////////////////////////////////

		// raycaster rework
		 //west walls /right side

		Object* player = new Object(vec3_new(1.0f, 1.0f, 1.0f),
			vec3_new(get_camera_position().x, get_camera_position().y, get_camera_position().z),
			vec3_new(0.0f, 0.0f, 0.0f));
		player->load_cube_mesh(DEFUALT, olc::CYAN, "./assets/blank.png");
		objectlist.push_back(player);

		Object* obj = new Object(vec3_new(1, 1, 0.2), vec3_new(0, 0, +5), vec3_new(0, 0, 0));
		obj->load_cube_mesh(FRONT, olc::CYAN, "./assets/r2idletest.png");

		objectlist.push_back(obj);
		//load_cube_mesh_data(FRONT, olc::CYAN, "./assets/r2idletest.png", vec3_new(1, 1, 0.2), vec3_new(0, 0, +5), vec3_new(0, 0, 0));
		
		for (int z = 0; z < tilecount * 2; z += 2)
		{
			Object* obj = new Object(vec3_new(1, 1, 1), vec3_new(-7, 0, -2 + z), vec3_new(0, 0, 0));
			obj->load_cube_mesh( RIGHT, olc::CYAN, "./assets/bluestone.png");
			objectlist.push_back(obj);
		}
		
		//east walls/ left side
		for (int z = 0; z < tilecount * 2; z += 2)
		{
			Object* obj = new Object(vec3_new(1, 1, 1), vec3_new(+13, 0, -2 + z), vec3_new(0, 0, 0));
			obj->load_cube_mesh(LEFT, olc::BLUE, "./assets/bluestone.png");
			objectlist.push_back(obj);
		}
		
		//back wall / front side
		for (int x = 0; x < tilecount * 2; x += 2)
		{
			Object* obj = new Object(vec3_new(1, 1, 1), vec3_new(-7 + x, 0, +10), vec3_new(0, 0, 0));
			obj->load_cube_mesh( FRONT, olc::GREEN, "./assets/bluestone.png");
			objectlist.push_back(obj);
		}
		
		
		//front wall / back side
		for (int x = 0; x < tilecount * 2; x += 2)
		{
			Object* obj = new Object(vec3_new(1, 1, 1), vec3_new(-7 + x, 0, -2), vec3_new(0, 0, 0));
			obj->load_cube_mesh( BACK, olc::DARK_CYAN, "./assets/bluestone.png");
			objectlist.push_back(obj);
		}
		
		//floor
		for (int x = -5; x < tilecount * 2; x += 2)
		{
		
			for (int z = 0; z < tilecount * 2; z += 2)
			{
				Object* obj = new Object(vec3_new(1, 1, 1), vec3_new(x, -2, -2 + z), vec3_new(0, 0, 0));
				obj->load_cube_mesh( TOP, olc::GREY, "./assets/colorstone.png");
				objectlist.push_back(obj);
			}
		
		}

		//ceiling

	   //for (int x = -5; x < tilecount * 2; x += 2)
	   //{
	   //
	   //    for (int z = 0; z < tilecount * 2; z += 2)
	   //    {
	   //        load_cube_mesh_data( BOTTOM, 0xff00ffff, "./assets/wood.png", vec3_new(1, 1, 1), vec3_new(x, +2, -2 + z), vec3_new(0, 0, 0));
	   //    }
	   //
	   //}

		depth_draw.Init(this);
		
		return true;
	}

	void input(float deltatime)
	{
		cam_prev = get_camera_direction();
		vec3_t up_direction = { 0,1,0 };
		
		//rendering type
		if (GetKey(olc::F1).bPressed) { render_method = RENDER_WIRE_VERTEX; };
		if (GetKey(olc::F2).bPressed) { render_method = RENDER_WIRE; };
		if (GetKey(olc::F3).bPressed) { render_method = RENDER_FILL_TRIANGLE; };
		if (GetKey(olc::F4).bPressed) { render_method = RENDER_FILL_TRIANGLE_WIRE; };
		if (GetKey(olc::F5).bPressed) { render_method = RENDER_TEXTURED; };
		if (GetKey(olc::F6).bPressed) { render_method = RENDER_TEXTURED_WIRE; };
		if (GetKey(olc::C).bPressed) { cull_method = CULL_BACKFACE; };
		if (GetKey(olc::V).bPressed) { cull_method = CULL_NONE; };
		//shading typecd
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
			offsetY = -2.0f;
		}
		if (GetKey(olc::K).bHeld) {
			offsetY = +2.0f;
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

		if(GetKey(olc::Q).bHeld) 
		{
			
			vec3_t stafe = vec3_cross(up_direction, get_camera_direction());

			stafe = vec3_mul(stafe, 3.0f * deltatime);
			set_new_camera_position(vec3_sub(get_camera_position(), stafe));
			
		};
		if (GetKey(olc::E).bHeld)
		{
			vec3_t stafe = vec3_cross(up_direction, get_camera_direction());
			stafe = vec3_mul(stafe,  3.0f * deltatime);
			set_new_camera_position(vec3_add(get_camera_position(), stafe));
		};
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

		if (GetKey(olc::SPACE).bHeld) {
			objectlist[1]->ispickedup = true;
		}

		if (GetKey(olc::SPACE).bReleased) {
			objectlist[1]->ispickedup = false;
		}
		

		cam_curr = get_camera_direction();
	}


	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		input(fElapsedTime);


		
		num_triangles_to_render = 0;
		
		int m = get_num_meshes();

		//for (auto obj : objectlist)
		//{
		//	obj->Update(this, fElapsedTime);
		//}
	
		
		//update and render visible mesh_faces;
		for (int mesh_index = 0; mesh_index < objectlist.size(); mesh_index++)
		{
			Object* obj = objectlist[mesh_index];
			//mesh_t* mesh = objectlist[mesh_index]->mesh;
			
			if (mesh_index == 0)
			{
				obj->mesh->translation = vec3_new(get_camera_position().x, get_camera_position().y, get_camera_position().z);
			}
			
			if (mesh_index == 1)
			{

				float distx = obj->mesh->translation.x - get_camera_position().x;
				float disty = obj->mesh->translation.y - get_camera_position().y;
				float distz = obj->mesh->translation.z - get_camera_position().z;
				
				float angleplayertoobj = atan2f(distx, distz);
				
				obj->mesh->rotation.x += offsetX * fElapsedTime;
				obj->mesh->rotation.y = angleplayertoobj;
				if (obj->ispickedup == true)
				{
					obj->mesh->translation = vec3_new(get_camera_position().x, get_camera_position().y, get_camera_position().z + 4);
					obj->mesh->rotation = vec3_new(0, angleplayertoobj, 0);
				}
				else
				{
					

					
					obj->mesh->rotation = vec3_new(0, angleplayertoobj, 0);
				}
				
				
				//
				
			 }
			
			
			//mesh->rotation.x += 0.5f * fElapsedTime;
		  // mesh->rotation.y += 2.0f * fElapsedTime;
		    //mesh->rotation.z += 0.5f * fElapsedTime;
		//mesh.scale.x += 0.02f* fElapsedTime;
		//mesh.scale.y += 0.02f* fElapsedTime;
		//mesh.translation.x += 0.01f;
		
		
		
		
			process_graphics_pipline_stages(obj,mesh_index);
			
		}
			// currently working stuff
			int t_length = (int)objectlist[0]->trangles_to_render.size();


			//collision test
			cam_acc = vec3_add(cam_acc, gravity);

			cam_vel = vec3_sub(cam_curr, cam_prev);

			cam_prev = cam_curr;


			for (auto obj : objectlist)
			{
				obj->Render(this, depth_draw);
			}



			//for (int i = 0; i < t_length; i++)
			//{
			//	triangle_t triangle = objectlist[0]->trangles_to_render[i];
			//
			//
			//	if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
			//	{
			//		
			//		//testing
			//		draw_filled_triangle(depth_draw,
			//			this,
			//			(int)triangle.points[0].x, (int)triangle.points[0].y, (int)triangle.points[0].z, (int)triangle.points[0].w,
			//			(int)triangle.points[1].x, (int)triangle.points[1].y, (int)triangle.points[1].z, (int)triangle.points[1].w,
			//			(int)triangle.points[2].x, (int)triangle.points[2].y, (int)triangle.points[2].z, (int)triangle.points[2].w,
			//			triangle.p
			//		);
			//	}
			//
			//	if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
			//	{
			//		draw_textured_triangle(depth_draw,
			//			this,
			//			(int)triangle.points[0].x, (int)triangle.points[0].y, (int)triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
			//			(int)triangle.points[1].x, (int)triangle.points[1].y, (int)triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
			//			(int)triangle.points[2].x, (int)triangle.points[2].y, (int)triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
			//			triangle.texture
			//		);
			//	}
			//
			//
			//	if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE ||
			//		render_method == RENDER_TEXTURED_WIRE)
			//	{
			//		draw_triangle(
			//			this,
			//			(int)triangle.points[0].x,
			//			(int)triangle.points[0].y,
			//			(int)triangle.points[1].x,
			//			(int)triangle.points[1].y,
			//			(int)triangle.points[2].x,
			//			(int)triangle.points[2].y,
			//			triangle.p
			//		);
			//	}
			//
			//	if (render_method == RENDER_WIRE_VERTEX)
			//	{
			//		draw_rect(this, (int)triangle.points[0].x, (int)triangle.points[0].y, 4, 4, olc::RED);
			//		draw_rect(this, (int)triangle.points[1].x, (int)triangle.points[1].y, 4, 4, olc::RED);
			//		draw_rect(this, (int)triangle.points[2].x, (int)triangle.points[2].y, 4, 4, olc::RED);
			//	}
			//}
			//
			//objectlist[0]->trangles_to_render.clear();
			//trangles_to_render.clear();
		 	
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