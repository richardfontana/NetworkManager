/* nmclienttest - test app for NetworkManager
 *
 * Dan Williams <dcbw@redhat.com>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * (C) Copyright 2004 Red Hat, Inc.
 */

#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>


char * get_active_device (DBusConnection *connection)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;
	char			*device_path;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						"/org/freedesktop/NetworkManager",
						"org.freedesktop.NetworkManager",
						"getActiveDevice");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return NULL;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return NULL;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return NULL;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char *string;
	string = dbus_message_iter_get_string (&iter);
	if (!string)
	{
		fprintf (stderr, "NetworkManager returned a NULL active device object path" );
		return NULL;
	}

	fprintf (stderr, "Active device: '%s'\n", string );

	dbus_message_unref (reply);
	dbus_message_unref (message);

	device_path = g_strdup (string);
	return (device_path);
}


void get_device_name (DBusConnection *connection, char *path)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager",
						"getName");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char *string;
	string = dbus_message_iter_get_string (&iter);
	if (!string)
	{
		fprintf (stderr, "NetworkManager returned a NULL active device object path" );
		return;
	}

	fprintf (stderr, "Active device name: '%s'\n", string );

	dbus_message_unref (reply);
	dbus_message_unref (message);
}


int get_object_signal_strength (DBusConnection *connection, char *path)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager.Devices",
						"getStrength");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return (0);
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	dbus_message_unref (message);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		return (0);
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		return (0);
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	int qual = dbus_message_iter_get_int32 (&iter);

	dbus_message_unref (reply);

	return (qual);
}


void get_nm_status (DBusConnection *connection)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						"/org/freedesktop/NetworkManager",
						"org.freedesktop.NetworkManager",
						"status");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char *string;
	string = dbus_message_iter_get_string (&iter);
	if (!string)
	{
		fprintf (stderr, "NetworkManager returned a NULL status" );
		return;
	}

	fprintf (stderr, "NM Status: '%s'\n", string );

	dbus_message_unref (reply);
	dbus_message_unref (message);
}

void get_device_active_network (DBusConnection *connection, char *path)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager",
						"getActiveNetwork");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		if (strstr (error.name, "NoActiveNetwork"))
			fprintf (stderr, "      This device is not associated with a wireless network\n");
		else
			fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char *string;
	string = dbus_message_iter_get_string (&iter);
	if (!string)
	{
		fprintf (stderr, "NetworkManager returned a NULL active device object path" );
		return;
	}

	fprintf (stderr, "Active device's Network: '%s' ", string );

	dbus_message_unref (reply);
	dbus_message_unref (message);

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						string,
						"org.freedesktop.NetworkManager",
						"getName");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	string = dbus_message_iter_get_string (&iter);
	if (!string)
	{
		fprintf (stderr, "NetworkManager returned a NULL active device object path" );
		return;
	}

	fprintf (stderr, " (%s)\n", string );

	dbus_message_unref (reply);
	dbus_message_unref (message);
}


int get_device_type (DBusConnection *connection, char *path)
{
	DBusMessage	*message;
	DBusMessage	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager",
						"getType");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return (-1);
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return (-1);
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return (-1);
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	int	type = dbus_message_iter_get_int32 (&iter);

	dbus_message_unref (reply);
	dbus_message_unref (message);

	return (type);
}


const char * get_network_name (DBusConnection *connection, const char *path)
{
	DBusMessage	*message2;
	DBusMessage	*reply2;
	DBusMessageIter iter2;
	DBusError		 error2;

	message2 = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager",
						"getName");
	if (message2 == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return (NULL);
	}

	dbus_error_init (&error2);
	reply2 = dbus_connection_send_with_reply_and_block (connection, message2, -1, &error2);
	dbus_message_unref (message2);
	if (dbus_error_is_set (&error2))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error2.name, error2.message);
		return (NULL);
	}

	if (reply2 == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		return (NULL);
	}

	/* now analyze reply */
	dbus_message_iter_init (reply2, &iter2);
	const char *string2 = dbus_message_iter_get_string (&iter2);
	if (!string2)
	{
		fprintf (stderr, "NetworkManager returned a NULL network name" );
		return (NULL);
	}

	dbus_message_unref (reply2);

	return (string2);
}


void get_device_networks (DBusConnection *connection, const char *path)
{
	DBusMessage 	*message;
	DBusMessage 	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						path,
						"org.freedesktop.NetworkManager",
						"getNetworks");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char **networks;
	int	num_networks;

	if (!dbus_message_iter_get_string_array (&iter, &networks, &num_networks))
	{
		fprintf (stderr, "NetworkManager returned no device list" );
		return;
	}

	dbus_message_unref (reply);
	dbus_message_unref (message);

	int i;
	fprintf( stderr, "      Networks:\n" );
	for (i = 0; i < num_networks; i++)
	{
		const char *name = get_network_name (connection, networks[i]);

		fprintf( stderr, "         %s (%s)  Strength: %d%%\n", networks[i], name,
				get_object_signal_strength (connection, networks[i]) );
		dbus_free (name);
	}

	dbus_free_string_array (networks);
}


void get_devices (DBusConnection *connection)
{
	DBusMessage 	*message;
	DBusMessage 	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						"/org/freedesktop/NetworkManager",
						"org.freedesktop.NetworkManager",
						"getDevices");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		return;
	}

	if (reply == NULL)
	{
		fprintf( stderr, "dbus reply message was NULL\n" );
		dbus_message_unref (message);
		return;
	}

	/* now analyze reply */
	dbus_message_iter_init (reply, &iter);
	char **devices;
	int	num_devices;

	if (!dbus_message_iter_get_string_array (&iter, &devices, &num_devices))
	{
		fprintf (stderr, "NetworkManager returned no device list" );
		return;
	}

	dbus_message_unref (reply);
	dbus_message_unref (message);

	int i;
	fprintf( stderr, "Devices:\n" );
	for (i = 0; i < num_devices; i++)
	{
		int	 type;

		fprintf (stderr, "   %s", devices[i]);
		if ((type = get_device_type (connection, devices[i])) == 2)
		{
			fprintf (stderr, "   Strength: %d%%\n", get_object_signal_strength (connection, devices[i]));
			fprintf (stderr, "      Device type: '%d'\n", type );
			get_device_active_network (connection, devices[i]);
			get_device_networks (connection, devices[i]);
			fprintf (stderr, "\n");
		}
		else
		{
			fprintf (stderr, "\n      Device type: '%d'\n", type );
			fprintf (stderr, "\n");
		}
	}

	dbus_free_string_array (devices);
}


void set_device_network (DBusConnection *connection, const char *path, const char *network)
{
	DBusMessage 	*message;
	DBusMessage 	*reply;
	DBusMessageIter iter;
	DBusError		 error;

	message = dbus_message_new_method_call ("org.freedesktop.NetworkManager",
						"/org/freedesktop/NetworkManager",
						"org.freedesktop.NetworkManager",
						"setActiveDevice");
	if (message == NULL)
	{
		fprintf (stderr, "Couldn't allocate the dbus message\n");
		return;
	}

	dbus_message_append_args (message, DBUS_TYPE_STRING, path,
							DBUS_TYPE_STRING, network, DBUS_TYPE_INVALID);

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
	if (dbus_error_is_set (&error))
	{
		fprintf (stderr, "%s raised:\n %s\n\n", error.name, error.message);
		dbus_message_unref (message);
		dbus_error_free (&error);
		return;
	}
	else
		fprintf (stderr, "Success!!\n");
}


int main( int argc, char *argv[] )
{
	DBusConnection *connection;
	DBusError		error;

	g_type_init ();

	dbus_error_init (&error);
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (connection == NULL)
	{
		fprintf (stderr, "Error connecting to system bus: %s\n", error.message);
		dbus_error_free (&error);
		return 1;
	}

	char *path;

	get_nm_status (connection);
	path = get_active_device (connection);
	get_device_name (connection, path);
	get_devices (connection);
	if ((argc == 2) && (get_device_type (connection, path) == 2))
	{
		fprintf (stderr, "Attempting to force AP '%s' for device '%s'\n", argv[1], path);
		set_device_network (connection, path, argv[1]);
	}

	g_free (path);

	return 0;
}
