#include <stdlib.h>
#include <graphene.h>
#include <gtk/gtk.h>

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

	GdkPixbuf *pixbuf;

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

	/* The view direction is the y axis. */
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

	image->pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 100, 100);

	/* Apply a white background color. */
	gdk_pixbuf_fill (image->pixbuf, 0xffffffff);
}

static void
init_objects (Raytracer *tracer)
{
	graphene_point3d_t center;

	/* A red sphere of radius 10 centered on (20, 50, 20). */
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
main (int    argc,
      char **argv)
{
	GtkWidget *window;
	GtkWidget *image;
	Raytracer tracer;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 120, 120);
	g_signal_connect (window, "destroy", gtk_main_quit, NULL);

	init (&tracer);

	image = gtk_image_new_from_pixbuf (tracer.image.pixbuf);
	gtk_container_add (GTK_CONTAINER (window), image);

	gtk_widget_show_all (window);
	gtk_main ();

	/* Be Valgrind-friendly */
	g_object_unref (tracer.image.pixbuf);

	return EXIT_SUCCESS;
}
