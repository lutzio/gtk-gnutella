
/* Misc functions */

#include "gnutella.h"

#include <sys/stat.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

gchar *ip_to_gchar(guint32 ip)
{
	struct in_addr ia;
	ia.s_addr = g_htonl(ip);
	return inet_ntoa(ia);
}


gchar *ip_port_to_gchar(guint32 ip, guint16 port)
{
	static gchar a[128];
	struct in_addr ia;
	ia.s_addr = g_htonl(ip);
	g_snprintf(a, sizeof(a), "%s:%u", inet_ntoa(ia), port);
	return a;
}

guint32 gchar_to_ip(gchar *str)
{
	/* Returns 0 if str is not a valid IP */

	struct in_addr ia;
	gint r;
	r = inet_aton(str, &ia);
	if (r) return g_ntohl(ia.s_addr);
	return 0;
}

guint32 host_to_ip(gchar *host)
{
	struct hostent *he = gethostbyname(host);
	if (he) return g_ntohl(*(guint32 *) (he->h_addr_list[0]));
	else herror("gethostbyname()");
	return 0;
}

/* Check wether path is a directory */

gboolean is_directory(gchar *path)
{
	struct stat st;
	if (stat(path, &st) == -1) return FALSE;
	return S_ISDIR(st.st_mode);
}

/* Returns a number of bytes in a more readable form */

gchar *short_size(guint32 size)
{
	static gchar b[256];

	if (size < 1024) g_snprintf(b, sizeof(b), "%u Bytes", size);
	else if (size < 1048576) g_snprintf(b, sizeof(b), "%.1fKB", (float) size / 1024.0);
	else if (size < 1073741824) g_snprintf(b, sizeof(b), "%.1fMB", (float) size / 1048576.0);
	else g_snprintf(b, sizeof(b), "%.1fGB", (float) size / 1073741824.0);

	return b;
}

/* Returns the ip:port of a node */

gchar *node_ip(struct gnutella_node *n)
{
	return ip_port_to_gchar(n->ip, n->port);
}

/* Dumps a gnutella message (debug) */

void message_dump(struct gnutella_node *n)
{
	gint32 size, ip, index, count, total;
	gint16 port, speed;

	printf("Node %s: ",    node_ip(n));
	printf("Func 0x%.2x ", n->header.function);
	printf("TTL = %d ",    n->header.ttl);
	printf("hops = %d ",   n->header.hops);

	READ_GUINT32_LE(n->header.size, size);

	printf(" data = %u", size);

	if (n->header.function == GTA_MSG_SEARCH)
	{
		READ_GUINT16_LE(n->data, speed);
		printf(" Speed = %d Query = '%s'", speed, n->data + 2);
	}
	else if (n->header.function == GTA_MSG_INIT_RESPONSE)
	{
		READ_GUINT16_LE(n->data, port);
		READ_GUINT32_BE(n->data + 2, ip);
		READ_GUINT32_LE(n->data + 6, count);
		READ_GUINT32_LE(n->data + 10, total);

		printf(" Host = %s Port = %d Count = %d Total = %d", ip_to_gchar(ip), port, count, total);
	}
	else if (n->header.function == GTA_MSG_PUSH_REQUEST)
	{
		READ_GUINT32_BE(n->data + 20, ip);
		READ_GUINT32_LE(n->data + 16, index);
		READ_GUINT32_LE(n->data + 24, port);

		printf(" Index = %d Host = %s Port = %d ", index, ip_to_gchar(ip), port);
	}

	printf("\n");
}

/* vi: set ts=3: */

