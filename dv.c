/* dv.c
 *
 * Copyright (C) 2003 Daniel Thom <dath@ficker.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ##########################################################################
 *
 * dv (or dath view) is a small image viewer meant as a free and simpler
 * replacement for xv.  It does not do any image manipulation as there are
 * better programs (gimp) available for that purpose.
 *
 * I originally wrote dv using gdk and imlib in 1999.  I finally got tired of
 * the busy "idle" eating up CPU time to do event processing, so I decided
 * a rewrite was in order after not finding any obvious (and better) event
 * handling mechanism for gdk.  This rewrite is the result of two nights of
 * insomnia by someone with no working knowledge of gtk...Be warned!
 *
 */


#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define VERSION		0.0.1

/*
 * Prototypes
 */
static gboolean delete_event(GtkWidget *, GdkEvent *, gpointer);
static void destroy(GtkWidget *, gpointer);
static void key_press_event(GtkWidget *, gpointer);
GtkWidget * dv_load_image(GtkWindow *, char *);
void dv_unload_image(GtkWindow *, GtkWidget *);


/* Some global variables we should remove...They could be stuffed in structures
 * and passed around no? */
int numfiles = 0;
GtkWidget *window;
GtkWidget *gimage;
char **gargv;


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		g_printerr("Usage: dv file [file...]\n");
		exit(1);
	}

	numfiles = argc;	/* Global file count */
	gargv = argv;		/* Global reference to argv */

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/* No user shrink, no user grow, window resize automatically */
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, TRUE);

	g_signal_connect(G_OBJECT(window), "delete_event",
			 G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",
			 G_CALLBACK(destroy), NULL);
	g_signal_connect(G_OBJECT(window), "key_press_event",
			 G_CALLBACK(key_press_event), NULL);

	/* Load initial image without a keypress */
	gimage = dv_load_image((GtkWindow *)window, argv[1]);
	
	gtk_widget_show(window);

	/* Start event processing */
	gtk_main();

	/* Should not make it here */
	return(0);
}


/*
 * On a "delete event", destroy the window.
 */
static gboolean
delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	/* Destroy the window */
	return(FALSE);
}


/*
 * If someone destroys us, let them!
 */
static void
destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}


/*
 * Handle a key press...
 */
static void
key_press_event(GtkWidget *widget, gpointer data)
{
	static int file = 1;
	GdkEventKey *ev = (GdkEventKey *) data;

	switch (ev->keyval) {
		case GDK_Tab:		/* Load Next File */
		case GDK_Right:
			if (file < numfiles - 1) {
				file++;
				dv_unload_image((GtkWindow *)window, gimage);
				gimage = NULL;
				gimage = dv_load_image((GtkWindow *)window, gargv[file]);
			}
			break;
		case GDK_Left:		/* Load Previous File */
			if (file > 1) {
				file--;
				dv_unload_image((GtkWindow *)window, gimage);
				gimage = NULL;
				gimage = dv_load_image((GtkWindow *)window, gargv[file]);
			}
			break;
		case GDK_q:		/* Quit Application */
		case GDK_Q:
			gtk_main_quit();
			break;
		default:
			break;
	}
}


/*
 * Load the specified image (and make it visible in the main window) and
 * return the widget that was created.
 */
GtkWidget *
dv_load_image(GtkWindow *window, char *file)
{
	GtkWidget *image;
#ifdef DEBUG
	GtkRequisition size;
#endif /* DEBUG */

	/*
	gchar load_string[1024];

	if (strlen(file) > 1014) {
		return(NULL);
	}

	g_snprintf(load_string, 1024, "Loading %s", file);
	gtk_window_set_title(window, load_string);
	*/

	image = gtk_image_new_from_file(file);
	if (image) {
#ifdef DEBUG
		gtk_widget_size_request(image, &size);
		g_print("%dx%d\n", size.width, size.height);
#endif /* DEBUG */

		gtk_container_add(GTK_CONTAINER(window), image);
		gtk_window_set_title(window, file);
		gtk_widget_show(image);
	}
	return(image);
}


/*
 * Remove the specified widget (image) from the main window, which I
 * *BELIEVE* will free it as well...
 */
void
dv_unload_image(GtkWindow *window, GtkWidget *image)
{
	if (image != NULL) {
		gtk_container_remove(GTK_CONTAINER(window), image);
		/* gtk_widget_destroy() complains if it is called after
		 * removing the image from the container.  I *ASSUME* this
		 * means it has been freed already. */
/*		gtk_widget_destroy(image); */
	}
}
