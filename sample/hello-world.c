/*
  This example program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#ifndef _WIN32
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

// #include <event2/d_global.h>

int
dg_append(char **rt, char *src1, int c1_len, char *src2, int c2_len)
{
	int sum = c1_len + c2_len;
	if (src1 != NULL && src2 != NULL) {
		(*rt) = (char *)malloc(sizeof(char) * sum);
		if (rt == NULL) {
			printf("dg_append>> malloc fail");
			return 0;
		}

		for (int i = 0; i < c1_len; i++) {
			(*rt)[i] = src1[i];
		}
		for (int i = 0; i < c2_len; i++) {
			int offset = i + c1_len;
			(*rt)[offset] = src2[i];
		}
	} else {
		return 0;
	}

	return sum;
}
void
dg_append_free(char *ptr)
{
	free(ptr);
}

int
dg_read_all(char *path, char **rt)
{
	int unit = 4096;
	int count_unit = 1;
	int size_rt = 0;
	int cur_capacit = unit * count_unit;
	FILE *fl = fopen(path, "r");
	if (fl != NULL) {
		printf("file open succese\n");
		(*rt) = malloc(sizeof(char) * cur_capacit);
		char ch;
		while (ch = getc(fl)) {
			if (ch != EOF) {
				if (size_rt < cur_capacit) {
					(*rt)[size_rt] = ch;
				} else {
					count_unit++;
					cur_capacit = unit * count_unit;
					char *check = realloc((*rt), cur_capacit);
					if (check == NULL) {
						if ((*rt) != NULL) {
							free((*rt));
							return 0;
						}
						return 0;
					}
					rt[size_rt] = ch;
				}
				size_rt++;
			} else {
				break;
			}
		}
		fclose(fl);
	} else {
		printf("dg_read_all>>open file failed");
		(*rt) = NULL;
		return 0;
	}

	return size_rt;
}

void
dg_read_all_free(char *ptr)
{
	free(ptr);
}

// Content-Length:132
// 该文本不能正常显示在浏览器中,需要加响应头,文件编码方式
static const char Header[] = "HTTP/1.1 200 OK\nContent-Type: text/html;charset=utf-8\nConnection: close\n\n";
static const char Rear[] = "\n";
static const char MESSAGE[] = "HTTP/1.1 200 OK\nContent-Type: text/html; "
							  "charset = utf-8\n\nHello, World!\n\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t,
	struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

int
main(int argc, char **argv)
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin = {0};
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr *)&sin,
		sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}


static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_READ);
	char path[11] = "index.html";
	char *fl_cnt = NULL;
	int fl_sz = dg_read_all(path, &fl_cnt);
	char *pkg1 = NULL;
	int pack_sz =
		dg_append(&pkg1, (char *)Header, strlen(Header), fl_cnt, fl_sz);
	char *package = NULL;
	if (pkg1 != 0) {
		pack_sz = dg_append(
			&package, (char *)pkg1, pack_sz, (char *)Rear, strlen(Rear));
		dg_append_free(pkg1);
	}

	// char* ptr=malloc()

	bufferevent_write(bev, package, pack_sz);
	if (fl_cnt != 0 && pack_sz != 0) {
		dg_read_all_free(fl_cnt);
		dg_append_free(package);
	}
}


static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		// bufferevent_free(bev);
	}
}

static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno)); /*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = {2, 0};

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}
