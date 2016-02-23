/*
 * Quick display of Navdata in a tree
 *
 * code adapted from the following GTK tutorial :
 * http://scentric.net/tutorial/treeview-tutorial.html
 */

/* System includes */
#include <sys/time.h>
#include <gtk/gtk.h>
#include <mosquitto.h>
/* SDK includes */
#include <ardrone_api.h>
#include <navdata_common.h>

/*Mqtt and serialization includes*/

#include <binn.h>

/* Local declarations */
#include <ihm/ihm_raw_navdata.h>

//The MQTT Client
struct mosquitto *navmosq = NULL;


#define MAX_FIELDS (100)
GtkTreeStore  *treestore;
GtkTreeIter   sequence_number;
GtkTreeIter   navdataBlocks[NAVDATA_NUM_TAGS];
GtkTreeIter   navdataFields[NAVDATA_NUM_TAGS][MAX_FIELDS];

static void addfield(int tag,char * name,char*comment,int*counter)
{
	GtkTreeIter child;
  gtk_tree_store_append(treestore, &child, &navdataBlocks[tag]);
  gtk_tree_store_set(treestore, &child,COL_FIELD, name,COL_VALUE,"",COL_COMMENT,comment,-1);
  navdataFields[tag][*counter]=child;
  (*counter)++;
}

static void setfield(int tag,char * value,int*counter)
{
  GtkTreeIter child;
  child = navdataFields[tag][*counter];
  gtk_tree_store_set(treestore, &child,COL_VALUE, value,-1);
  (*counter)++;
}

void mqtt_message_callback(struct mosquitto *mosq, void *obj, 
    const struct mosquitto_message *message)
{
  // Note: nothing in the Mosquitto docs or examples suggests that we
  //  must free this message structure after processing it.
  printf ("Got message: %s on topic %s\n", (char *)message->payload, message->topic);
  if(!(strcmp(message->topic, "/ardrone/takeoff")))
  {
    printf ("Got takeoff message: %s \n", (char *)message->payload); 
    ardrone_tool_set_ui_pad_start(1);
  }
  else if(!(strcmp(message->topic, "/ardrone/land")))
  {
    printf ("Got land message: %s \n", (char *)message->payload);
    ardrone_tool_set_ui_pad_start(0);
  }
}

void mqtt_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
  printf("Subscribe succeeded\n");
}

void mqtt_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
  printf("MQTT Connected with code: %d\n", rc);
}


static GtkTreeModel *
create_and_fill_model (void)
{
	int cpt=0;

  treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /* Append a top level rows */

  gtk_tree_store_append(treestore, &sequence_number, /*parent element*/NULL);
  gtk_tree_store_set(treestore, &sequence_number,COL_FIELD, "Sequence number",-1);


	#define NAVDATA_OPTION_DEMO(STRUCTURE,NAME,TAG) { \
				gtk_tree_store_append(treestore, &navdataBlocks[TAG], NULL);\
				gtk_tree_store_set   (treestore, &navdataBlocks[TAG], COL_FIELD, #TAG, -1);  }
	#define NAVDATA_OPTION(STRUCTURE,NAME,TAG)      { \
				gtk_tree_store_append(treestore, &navdataBlocks[TAG], NULL); \
				gtk_tree_store_set   (treestore, &navdataBlocks[TAG], COL_FIELD, #TAG, -1);  }
	#define NAVDATA_OPTION_CKS(STRUCTURE,NAME,TAG)

	#include <navdata_keys.h>


  cpt=0;
  addfield(NAVDATA_DEMO_TAG,"ctrl_state","Control State",&cpt);
  addfield(NAVDATA_DEMO_TAG,"phi","deg - Phi",&cpt);
  addfield(NAVDATA_DEMO_TAG,"psi","deg - Psi",&cpt);
  addfield(NAVDATA_DEMO_TAG,"theta","deg - Theta",&cpt);

  cpt=0;
  addfield(NAVDATA_GAMES_TAG,"Double tap counter","times",&cpt);
  addfield(NAVDATA_GAMES_TAG,"Finish line counter","times",&cpt);


  return GTK_TREE_MODEL(treestore);
}



static GtkWidget *
create_view_and_model (void)
{
  GtkTreeViewColumn   *col;
  GtkCellRenderer     *renderer;
  GtkWidget           *view;
  GtkTreeModel        *model;

  view = gtk_tree_view_new();

  /* --- Column #1 --- */
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Navdata field");
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", COL_FIELD);
  //g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "weight-set", TRUE, NULL);
  //g_object_set(renderer, "foreground", "Red", "foreground-set", TRUE, NULL); /* make red */

  /* --- Column #2 --- */
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Value");
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", COL_VALUE);


  /* --- Column #3 --- */
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Comment");
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", COL_COMMENT);

  //gtk_tree_view_column_set_cell_data_func(col, renderer, age_cell_data_func, NULL, NULL);


  model = create_and_fill_model();
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
  g_object_unref(model); /* destroy model automatically with view */
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), GTK_SELECTION_NONE);

  return view;
}


static GtkWidget *window = NULL;
static GtkWidget *view   = NULL;

gint navdata_ihm_raw_navdata_window_was_destroyed( GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	/* Explanations here : http://www.gtk.org/tutorial1.2/gtk_tut-2.html */
	window = NULL;
	view = NULL;
	return FALSE; /* FALSE means we want the window destryed. TRUE aborts the destroy process. */
}

int
navdata_ihm_raw_navdata_create_window ()
{
	if (!window)
	{
		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		if (!window) { return -1; }
		gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

		g_signal_connect(window, "delete_event", GTK_SIGNAL_FUNC(navdata_ihm_raw_navdata_window_was_destroyed), NULL); /* dirty */
		view = create_view_and_model();
		if (!view) { return -1; }

		gtk_container_add(GTK_CONTAINER(window), view);
	}

	gtk_widget_show_all(window);

	return 0;
}

static char buf[1024];
static char buf2[1024];

/* Do an ugly extern thingy because the packed navdata are normally not exposed by the SDK.
 * The unpacked version however does not contain the sequence number ...
 */
extern uint8_t navdata_buffer[NAVDATA_MAX_SIZE];

int
navdata_ihm_raw_navdata_update ( const navdata_unpacked_t* const navdata )
{
  const navdata_demo_t* nd = &navdata->navdata_demo;
  if(navmosq)
  {
    mosquitto_loop_start(navmosq);
    //serializing using binn library.

    //initialize obj
    binn* obj;
    obj = binn_object();
    
    //all fields taken from navdata_demo_t structure in navdata_common.h
    
    //HS_02082016
    //these are the fields that are required in the ROS Ardrone_autonomy Navdata.msg which we want to publish on the ROS side.
    //reference has been takjen from ardrone_navdata_file.c

    binn_object_set_uint32(obj, "vbat_flying_percentage", nd->vbat_flying_percentage);
    binn_object_set_uint32(obj, "ctrl_state", nd->ctrl_state);
    //    //HS_02082016 - Apparently tum_ardrone doesn't require mx,my,mz
    //binn_object_set_int32(obj, "mx", navdata->navdata_magneto.mx);
    //binn_object_set_int32(obj, "my", navdata->navdata_magneto.my);
    //binn_object_set_int32(obj, "mz", navdata->navdata_magneto.mz);
    //(signed int) pnd->navdata_magneto.mx,
    //(signed int) pnd->navdata_magneto.my,
    //(signed int) pnd->navdata_magneto.mz

    binn_object_set_int32(obj, "pressure", navdata->navdata_pressure_raw.Pression_meas);

    //Doesn't seem to be required by tum_ardrone
    //(signed int) pnd->navdata_pressure_raw.Temperature_meas
    //pnd->navdata_wind_speed.wind_speed,
    //pnd->navdata_wind_speed.wind_angle,
    //pnd->navdata_wind_speed.wind_compensation_phi,

    binn_object_set_float(obj, "theta", nd->theta);
    binn_object_set_float(obj, "phi", nd->phi);
    binn_object_set_float(obj, "psi", nd->psi);
    binn_object_set_uint32(obj, "altitude", nd->altitude);
    binn_object_set_float(obj, "vx", nd->vx);
    binn_object_set_float(obj, "vy", nd->vy);
    binn_object_set_float(obj, "vz", nd->vz);
    //quick grep in tum_ardrone says we don't need this: HS02082016
    //navdata->navdata_phys_measures.phys_accs[ACC_X],
    //navdata->navdata_phys_measures.phys_accs[ACC_Y],
    //navdata->navdata_phys_measures.phys_accs[ACC_Z]

    binn_object_set_uint32(obj, "motor1", navdata->navdata_pwm.motor1);
    binn_object_set_uint32(obj, "motor2", navdata->navdata_pwm.motor2);
    binn_object_set_uint32(obj, "motor3", navdata->navdata_pwm.motor3);
    binn_object_set_uint32(obj, "motor4", navdata->navdata_pwm.motor4);
    //(unsigned int) pnd->navdata_vision_detect.nb_detected 
    binn_object_set_uint32(obj, "tm", navdata->navdata_time.time);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    binn_object_set_uint32(obj, "time_sec", (uint32_t)tv.tv_sec);
    binn_object_set_uint32(obj, "time_usec", (uint32_t)tv.tv_usec);

    /* Apparently not required
       binn_object_set_uint32(obj, "timestamp", (uint32_t)time(NULL));
       binn_object_set_uint16(obj, "tag", nd->tag);
       binn_object_set_uint16(obj, "size", nd->size);
       binn_object_set_uint32(obj, "num_frames", nd->num_frames);
       binn_object_set_uint32(obj, "detection_camera_type", nd->detection_camera_type);
     */

    //publish data from the binn using mqtt

    int ret = mosquitto_publish (navmosq, NULL, "uas/uav1/navdata", binn_size(obj), binn_ptr(obj), 0, false);
    if (ret)
    {
      fprintf (stderr, "NavClient: Can't publish to Mosquitto server\n");
    }

    // release the buffer
    binn_free(obj);
  }
  else
  {
    printf("Error in MQTT Connection..");
  }

	int cpt;
	long int period;
	double frequence,lowpass_frequence;
	static double previous_frequence=0.0f;
	struct timeval current_time;
	static struct timeval previous_time;
	static unsigned long int lastRefreshTime=0;

	navdata_t* packed_navdata = (navdata_t*) &navdata_buffer[0];

	if (!window || !view) { return -1; }

	gettimeofday(&current_time,NULL);

	period = ((current_time.tv_sec-previous_time.tv_sec))*1000000+(current_time.tv_usec-previous_time.tv_usec);
	lastRefreshTime+=period;

	frequence = 1000000.0f / (double)period;
	lowpass_frequence = frequence * 0.005f + previous_frequence * 0.95f;

	if (lastRefreshTime > 100000 /*ms*/)
	{
		lastRefreshTime = 0;

		gdk_threads_enter(); //http://library.gnome.org/devel/gdk/stable/gdk-Threads.html

		snprintf(buf,sizeof(buf),"%d",packed_navdata->sequence);
		snprintf(buf2,sizeof(buf2),"%3.1f Hz",lowpass_frequence);
		gtk_tree_store_set(treestore, &sequence_number,COL_VALUE, buf,COL_COMMENT,buf2,-1);

		cpt=0;
		snprintf(buf,sizeof(buf),"%x",navdata->navdata_demo.ctrl_state);	setfield(NAVDATA_DEMO_TAG,buf,&cpt);
		snprintf(buf,sizeof(buf),"%3.1f",navdata->navdata_demo.phi/1000);	setfield(NAVDATA_DEMO_TAG,buf,&cpt);
		snprintf(buf,sizeof(buf),"%3.1f",navdata->navdata_demo.psi/1000);	setfield(NAVDATA_DEMO_TAG,buf,&cpt);
		snprintf(buf,sizeof(buf),"%3.1f",navdata->navdata_demo.theta/1000);	setfield(NAVDATA_DEMO_TAG,buf,&cpt);

    cpt=0;
    snprintf(buf,sizeof(buf),"%x",navdata->navdata_games.double_tap_counter);	setfield(NAVDATA_GAMES_TAG,buf,&cpt);
    snprintf(buf,sizeof(buf),"%x",navdata->navdata_games.finish_line_counter);	setfield(NAVDATA_GAMES_TAG,buf,&cpt);

    gtk_widget_draw(GTK_WIDGET(view), NULL);
    gdk_threads_leave(); //http://library.gnome.org/devel/gdk/stable/gdk-Threads.html

  }

  previous_time = current_time;
  previous_frequence = lowpass_frequence;
  return C_OK;
}

int navdata_ihm_raw_navdata_init ( void*v ) {

  mosquitto_lib_init();

  navmosq = mosquitto_new ("NavClient", true, NULL);
  if (!navmosq)
  {
    fprintf (stderr, "Navclient: Can't initialize Mosquitto library\n");		
  }	
  
  mosquitto_subscribe_callback_set (navmosq, mqtt_subscribe_callback);
  mosquitto_message_callback_set (navmosq, mqtt_message_callback);
  mosquitto_connect_callback_set (navmosq, mqtt_connect_callback);

  mosquitto_username_pw_set (navmosq, "admin", "admin");

  int ret = mosquitto_connect_async (navmosq, "localhost", 1883, 0);

  if (ret)
  {
    fprintf (stderr, "Navclient: Can't connect to Mosquitto server\n");
  }
  ret = mosquitto_subscribe(navmosq, NULL, "/ardrone/takeoff", 0);
  ret = mosquitto_subscribe(navmosq, NULL, "/ardrone/land", 0);
  if (ret)
  {
    fprintf (stderr, "Navclient: Can't publish to Mosquitto server\n");
  }
 
  return C_OK;
}
int navdata_ihm_raw_navdata_release () {
  mosquitto_disconnect (navmosq);
  mosquitto_destroy (navmosq);
  mosquitto_lib_cleanup();
  return C_OK;
}

