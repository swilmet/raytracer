#include <stdlib.h>
#include <graphene.h>

typedef struct _CameraFrame CameraFrame;
typedef struct _ImagePlane ImagePlane;
typedef struct _Raytracer Raytracer;

struct _CameraFrame
{
	graphene_point3d_t eye;
	graphene_vec3_t u;
	graphene_vec3_t v;
	graphene_vec3_t w;
};

/* Uses a parallel projection. */
struct _ImagePlane
{
	float left;
	float right;
	float bottom;
	float top;

	/* TODO add pixbuf */

	/* To support perspective projection, add a distance field. */
};

struct _Raytracer
{
	CameraFrame camera;
	ImagePlane image;

	/* Objects */
	graphene_sphere_t sphere_red;
};

static void
init_camera_frame (CameraFrame *camera)
{
	graphene_point3d_init (&camera->eye, 50.f, 0.f, 50.f);
	graphene_vec3_init_from_vec3 (&camera->u, graphene_vec3_x_axis ());
	graphene_vec3_init_from_vec3 (&camera->v, graphene_vec3_z_axis ());
	graphene_vec3_negate (graphene_vec3_y_axis (), &camera->w);
}

static void
init_image_plane (ImagePlane *image)
{
	/* A square of 100x100. */
	image->left = -50.f;
	image->right = 50.f;
	image->bottom = -50.f;
	image->top = 50.f;
}

static void
init_objects (Raytracer *tracer)
{
	graphene_point3d_t center;

	graphene_point3d_init (&center, 20.f, 50.f, 20.f);
	graphene_sphere_init (&tracer->sphere_red, &center, 10.f);
}

static void
init (Raytracer *tracer)
{
	init_camera_frame (&tracer->camera);
	init_image_plane (&tracer->image);
	init_objects (tracer);
}

int
main (void)
{
	Raytracer tracer;

	init (&tracer);

	return EXIT_SUCCESS;
}
