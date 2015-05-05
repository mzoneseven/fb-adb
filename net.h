/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in
 *  the LICENSE file in the root directory of this source tree. An
 *  additional grant of patent rights can be found in the PATENTS file
 *  in the same directory.
 *
 */
#pragma once
#include "util.h"
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>

struct sockaddr_un;
enum addr_kind {
    addr_unix_filesystem,
#ifdef __linux__
    addr_unix_abstract,
#endif
};

struct addr {
    socklen_t size;
    union {
        struct sockaddr addr;
        struct sockaddr_un addr_un;
    };
};

struct addr* make_addr_unix_filesystem(const char* pathname);
#ifdef __linux__
struct addr* make_addr_unix_abstract(const void* bytes, size_t nr);
#endif

struct addrinfo* xgetaddrinfo(const char* node,
                              const char* service,
                              const struct addrinfo* hints);

struct addr* addrinfo2addr(const struct addrinfo* ai);

void xconnect(int fd, const struct addr* addr);
void xlisten(int fd, int backlog);
void xbind(int fd, const struct addr* addr);
void xsetsockopt(int fd, int level, int opname,
                 void* optval, socklen_t optlen);

void str2gaiargs(const char* inp, char** node, char** service);

// Allocate a socket.  The returned file descriptor is owned by the
// current reslist.
int xsocket(int domain, int type, int protocol);

// Accept a connection.  The returned file descriptor is owned by the
// current reslist.
int xaccept(int server_socket);

// Allocate a socket pair.  The returned file descriptors are owned by
// the current reslist.
void xsocketpair(int domain, int type, int protocol,
                 int* s1, int* s2);

void disable_tcp_nagle(int fd);
