/*
 * $Id$
 *
 * Copyright (c) 2010, Jeroen Asselman
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

/**
 * @ingroup lib
 * @file
 *
 * Win32 cross-compiling utility routines.
 *
 * @author Jeroen Asselman
 * @date 2010
 */

#ifndef _mingw32_h_
#define _mingw32_h_
#ifdef MINGW32

#define WINVER 0x0501
#include <ws2tcpip.h>
#include <winsock2.h>

#define ECONNRESET WSAECONNRESET
#define ECONNREFUSED WSAECONNREFUSED
#define ECONNABORTED WSAECONNABORTED
#define ENETUNREACH WSAENETUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#define ETIMEDOUT WSAETIMEDOUT
#define EINPROGRESS WSAEINPROGRESS
#define ENOTCONN WSAENOTCONN
#define ENOBUFS WSAENOBUFS
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define ENETRESET WSAENETRESET
#define ENETDOWN WSAENETDOWN
#define EHOSTDOWN WSAEHOSTDOWN
#define ENOPROTOOPT WSAENOPROTOOPT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define EDQUOT WSAEDQUOT

#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH

#define S_IXGRP  _S_IEXEC
#define S_IWGRP  _S_IWRITE
#define S_IRGRP  _S_IREAD

#define S_IRWXG _S_IREAD
#define S_IRWXO _S_IREAD

#define S_IROTH 0
#define S_IWOTH 0
#define S_ISUID 0
#define S_ISGID 0

#define MAP_PRIVATE 0	/* FIXME */

#define PROT_READ PAGE_READONLY		/* FIXME */
#define PROT_NONE PAGE_READWRITE

#define O_NONBLOCK 0

/* Should fix the usage of all of the following */
#define F_DUPFD  0
#define F_GETFD  1
#define F_SETFD  2
#define F_GETFL  3
#define F_SETFL  4
#define F_GETLK  5
#define F_SETLK  6
#define F_SETLKW 7

#define F_RDLCK  0
#define F_WRLCK  1
#define F_UNLCK  2

#define S_IFLNK 0120000 /* Symbolic link */

/* winsock doesn't feature poll(), so there is a version implemented
 * in terms of select() in mingw.c. The following definitions
 * are copied from linux man pages. A poll() macro is defined to
 * call the version in mingw.c.
 */
#define POLLIN      0x0001    /* There is data to read */
#define POLLPRI     0x0002    /* There is urgent data to read */
#define POLLOUT     0x0004    /* Writing now will not block */
#define POLLERR     0x0008    /* Error condition */
#define POLLHUP     0x0010    /* Hung up */
#define POLLNVAL    0x0020    /* Invalid request: fd not open */

#define getppid()	1
#define fcntl(fd, cmd, ...) (-1)
#define ffs __builtin_ffs

#define socket mingw_socket
#define bind mingw_bind
#define	writev mingw_s_writev
#define getsockopt mingw_getsockopt
#define setsockopt mingw_setsockopt
#define connect mingw_connect
#define listen mingw_listen
#define accept mingw_accept
#define shutdown mingw_shutdown

#define getaddrinfo mingw_getaddrinfo
#define freeaddrinfo mingw_freeaddrinfo

#define open mingw_open
#define read mingw_read
#define readv mingw_s_readv
#define write mingw_write
#define sendto mingw_sendto

#define sockaddr_un sockaddr_in

typedef SOCKET socket_fd_t;
typedef WSABUF iovec_t;

struct pollfd {
    SOCKET fd;        /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

struct msghdr
  {
    void *msg_name;             /* Address to send to/receive from.  */
    socklen_t msg_namelen;      /* Length of address data.  */

    iovec_t *msg_iov;      /* Vector of data to send/receive into.  */
    size_t msg_iovlen;          /* Number of elements in the vector.  */

    void *msg_control;          /* Ancillary data (eg BSD filedesc passing). */
    size_t msg_controllen;      /* Ancillary data buffer length.
                                   !! The type should be socklen_t but the
                                   definition of the kernel is incompatible
                                   with this.  */

    int msg_flags;              /* Flags on received message.  */
  };



static inline char *
iovec_base(const iovec_t* iovec) {
	return iovec->buf;
}

static inline size_t 
iovec_len(const iovec_t* iovec) {
	return iovec->len;
}

static inline char *
iovec_set_base(iovec_t* iovec, char *base) {
	return iovec->buf = base;
}

static inline size_t 
iovec_set_len(iovec_t* iovec, size_t len) {
	return iovec->len = len;
}

const char* mingw_strerror(int errnum);
int mingw_open(const char *pathname, int flags, ...);
int mingw_rename(const char *oldpath, const char *newpath);

ssize_t mingw_read(int fd, void *buf, size_t count);
ssize_t mingw_write(int fd, const void *buf, size_t count);


/*** Socket functions ***/
int mingw_getaddrinfo(const char *node, const char *service,
                      const struct addrinfo *hints,
                      struct addrinfo **res);

void mingw_freeaddrinfo(struct addrinfo *res);

	   
socket_fd_t mingw_socket(int domain, int type, int protocol);
int         mingw_bind(socket_fd_t sockfd, const struct sockaddr *addr, socklen_t addrlen);
socket_fd_t mingw_connect(socket_fd_t sockfd, const struct sockaddr *addr,
	          socklen_t addrlen);
int         mingw_listen(socket_fd_t sockfd, int backlog);
socket_fd_t mingw_accept(socket_fd_t sockfd, struct sockaddr *addr, 
              socklen_t *addrlen);
int         mingw_shutdown(socket_fd_t sockfd, int how);
int         mingw_getsockopt(int sockfd, int level, int optname, 
              void *optval, socklen_t *optlen);
int         mingw_setsockopt(socket_fd_t sockfd, int level, int optname, 
              const void *optval, socklen_t optlen);
ssize_t     mingw_sendto(socket_fd_t sockfd, const void *buf, size_t len, int flags,
              const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t     s_write(socket_fd_t fd, const void *buf, size_t count);
ssize_t     s_read(socket_fd_t fd, void *buf, size_t count);
size_t      mingw_s_readv(socket_fd_t fd, const iovec_t *iov, int iovcnt);
ssize_t     recvmsg (socket_fd_t s, struct msghdr *hdr, int flags);

int         s_close(socket_fd_t fd);
ssize_t     mingw_s_writev(socket_fd_t fd, const iovec_t *iov, int iovcnt);

#define rename(oldpath, newpath) mingw_rename(oldpath, newpath)
#define g_strerror(errnum) mingw_strerror(errnum)

#endif
#endif /* _mingw32_h_ */

/* vi: set ts=4 sw=4 cindent: */