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

/* Compute the ray for the pixel at position (i,j). */
static void
compute_ray (Raytracer      *tracer,
	     int             pixel_i,
	     int             pixel_j,
	     graphene_ray_t *ray)
{
	ImagePlane *image = &tracer->image;
	int width;
	int height;
	float u;
	float v;
	graphene_vec3_t u_vec3;
	graphene_vec3_t v_vec3;
	graphene_vec3_t uu;
	graphene_vec3_t vv;
	graphene_vec3_t eye;
	graphene_vec3_t origin_vec3;
	graphene_point3d_t origin;
	graphene_vec3_t direction;

	width = gdk_pixbuf_get_width (image->pixbuf);
	height = gdk_pixbuf_get_height (image->pixbuf);

	g_assert (0 <= pixel_i && pixel_i < width);
	g_assert (0 <= pixel_j && pixel_j < height);

	u = image->left + (image->right - image->left) * (pixel_i + 0.5) / width;
	v = image->top + (image->bottom - image->top) * (pixel_j + 0.5) / height;

	g_assert (image->left < u && u < image->right);
	g_assert (image->bottom < v && v < image->top);

	graphene_vec3_init (&u_vec3, u, u, u);
	graphene_vec3_init (&v_vec3, v, v, v);

	graphene_vec3_multiply (&tracer->camera.u, &u_vec3, &uu);
	graphene_vec3_multiply (&tracer->camera.v, &v_vec3, &vv);

	graphene_point3d_to_vec3 (&tracer->camera.eye, &eye);
	graphene_vec3_add (&eye, &uu, &origin_vec3);
	graphene_vec3_add (&origin_vec3, &vv, &origin_vec3);
	graphene_point3d_init_from_vec3 (&origin, &origin_vec3);

	graphene_vec3_negate (&tracer->camera.w, &direction);

	graphene_ray_init (ray, &origin, &direction);
}

static void
put_pixel (GdkPixbuf *pixbuf,
	   int        pixel_i,
	   int        pixel_j,
	   guchar     red,
	   guchar     green,
	   guchar     blue)
{
	int width;
	int height;
	int rowstride;
	int n_channels;
	guchar *pixels;
	guchar *p;

	width = gdk_pixbuf_get_width (pixbuf);
	height = gdk_pixbuf_get_height (pixbuf);

	g_assert (0 <= pixel_i && pixel_i < width);
	g_assert (0 <= pixel_j && pixel_j < height);

	rowstride = gdk_pixbuf_get_rowstride (pixbuf);

	n_channels = gdk_pixbuf_get_n_channels (pixbuf);
	g_assert_cmpint (n_channels, ==, 3);

	pixels = gdk_pixbuf_get_pixels (pixbuf);

	p = pixels + pixel_j * rowstride + pixel_i * n_channels;
	p[0] = red;
	p[1] = green;
	p[2] = blue;
}

static bool
intersect_object (Raytracer      *tracer,
		  graphene_ray_t *ray)
{
	return true;
}

static void
launch (Raytracer *tracer)
{
	int width;
	int height;
	int pixel_i;

	width = gdk_pixbuf_get_width (tracer->image.pixbuf);
	height = gdk_pixbuf_get_height (tracer->image.pixbuf);

	for (pixel_i = 0; pixel_i < width; pixel_i++)
	{
		int pixel_j;

		for (pixel_j = 0; pixel_j < height; pixel_j++)
		{
			graphene_ray_t ray;

			compute_ray (tracer, pixel_i, pixel_j, &ray);

			if (intersect_object (tracer, &ray))
			{
				put_pixel (tracer->image.pixbuf,
					   pixel_i, pixel_j,
					   0xff, 0x00, 0x00);
			}
		}
	}
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

	launch (&tracer);

	gtk_main ();

	/* Be Valgrind-friendly */
	g_object_unref (tracer.image.pixbuf);

	return EXIT_SUCCESS;
}
