/*-
 * Copyright 2018 Aniket Pandey
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <atf-c.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#define ERROR (-1)
#define SERVER_PATH "server"

static int sockfd, sockfd2;
static int tr = 1;
static socklen_t len;
static struct pollfd fds[1];
static char regex[40];

/*
 * Assign local address to a server's socket
 */
static void
assign_address(struct sockaddr_un *server)
{
	memset(server, 0, sizeof(*server));
	server->sun_family = AF_UNIX;
	strcpy(server->sun_path, SERVER_PATH);
}


ATF_TC_WITH_CLEANUP(socket_success);
ATF_TC_HEAD(socket_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"socket(2) call");
}

ATF_TC_BODY(socket_success, tc)
{
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Check the presence of sockfd in audit record */
	snprintf(regex, 30, "socket.*return,success,%d", sockfd);
	check_audit(fds, regex, pipefd);
	close(sockfd);
}

ATF_TC_CLEANUP(socket_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(socket_failure);
ATF_TC_HEAD(socket_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"socket(2) call");
}

ATF_TC_BODY(socket_failure, tc)
{
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, socket(ERROR, SOCK_STREAM, 0));
	/* Check the presence of hex(-1) in audit record */
	snprintf(regex, 40, "socket.*0x%x.*return,failure", ERROR);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(socket_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(socketpair_success);
ATF_TC_HEAD(socketpair_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"socketpair(2) call");
}

ATF_TC_BODY(socketpair_success, tc)
{
	int sv[2];
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, socketpair(PF_UNIX, SOCK_STREAM, 0, sv));
	/* Check the presence of 0x0 in audit record */
	snprintf(regex, 40, "socketpair.*0x0.*return,success");
	check_audit(fds, regex, pipefd);

	/* Close all socket descriptors */
	close(sv[0]);
	close(sv[1]);
}

ATF_TC_CLEANUP(socketpair_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(socketpair_failure);
ATF_TC_HEAD(socketpair_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"socketpair(2) call");
}

ATF_TC_BODY(socketpair_failure, tc)
{
	int sv;
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, socketpair(ERROR, SOCK_STREAM, 0, &sv));
	/* Check the presence of hex(-1) in audit record */
	snprintf(regex, 40, "socketpair.*0x%x.*return,failure", ERROR);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(socketpair_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(setsockopt_success);
ATF_TC_HEAD(setsockopt_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"setsockopt(2) call");
}

ATF_TC_BODY(setsockopt_success, tc)
{
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Check the presence of sockfd in audit record */
	snprintf(regex, 30, "setsockopt.*0x%x.*return,success", sockfd);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, \
		sizeof(int)));
	check_audit(fds, regex, pipefd);
	close(sockfd);
}

ATF_TC_CLEANUP(setsockopt_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(setsockopt_failure);
ATF_TC_HEAD(setsockopt_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"setsockopt(2) call");
}

ATF_TC_BODY(setsockopt_failure, tc)
{
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, setsockopt(ERROR, SOL_SOCKET, SO_REUSEADDR, &tr, \
		sizeof(int)));
	/* Check the presence of hex(-1) in audit record */
	snprintf(regex, 40, "setsockopt.*0x%x.*return,failure", ERROR);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(setsockopt_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(bind_success);
ATF_TC_HEAD(bind_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"bind(2) call");
}

ATF_TC_BODY(bind_success, tc)
{
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Preliminary socket setup */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Check the presence of AF_UNIX address path in audit record */
	snprintf(regex, 30, "bind.*unix.*%s.*return,success", SERVER_PATH);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);
	close(sockfd);
}

ATF_TC_CLEANUP(bind_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(bind_failure);
ATF_TC_HEAD(bind_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"bind(2) call");
}

ATF_TC_BODY(bind_failure, tc)
{
	/* Preliminary socket setup */
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, bind(-1, (struct sockaddr *)&server, len));
	/* Check the presence of hex(-1) and AF_UNIX path in audit record */
	snprintf(regex, 40, "bind.*0x%x.*%s.*return,failure", ERROR, SERVER_PATH);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(bind_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(bindat_success);
ATF_TC_HEAD(bindat_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"bindat(2) call");
}

ATF_TC_BODY(bindat_success, tc)
{
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Preliminary socket setup */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Check the presence of socket descriptor in audit record */
	snprintf(regex, 30, "bindat.*0x%x.*return,success", sockfd);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, bindat(AT_FDCWD, sockfd, \
		(struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);
	close(sockfd);
}

ATF_TC_CLEANUP(bindat_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(bindat_failure);
ATF_TC_HEAD(bindat_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"bindat(2) call");
}

ATF_TC_BODY(bindat_failure, tc)
{
	/* Preliminary socket setup */
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, bindat(AT_FDCWD, -1, (struct sockaddr *)&server, len));
	/* Check the presence of hex(-1) in audit record */
	snprintf(regex, 40, "bindat.*0x%x.*return,failure", ERROR);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(bindat_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(listen_success);
ATF_TC_HEAD(listen_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"listen(2) call");
}

ATF_TC_BODY(listen_success, tc)
{
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Preliminary socket setup */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	/* Check the presence of socket descriptor in the audit record */
	snprintf(regex, 30, "listen.*0x%x.*return,success", sockfd);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, listen(sockfd, 1));
	check_audit(fds, regex, pipefd);
	close(sockfd);
}

ATF_TC_CLEANUP(listen_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(listen_failure);
ATF_TC_HEAD(listen_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"listen(2) call");
}

ATF_TC_BODY(listen_failure, tc)
{
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, listen(ERROR, 1));
	/* Check the presence of hex(-1) in audit record */
	snprintf(regex, 40, "listen.*0x%x.*return,failure", ERROR);
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(listen_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(connect_success);
ATF_TC_HEAD(connect_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"connect(2) call");
}

ATF_TC_BODY(connect_success, tc)
{
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Server Socket: Assign address and listen for connection */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Non-blocking server socket */
	ATF_REQUIRE(fcntl(sockfd, F_SETFL, O_NONBLOCK) != -1);
	/* Bind to the specified address and wait for connection */
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	ATF_REQUIRE_EQ(0, listen(sockfd, 1));

	/* Set up "blocking" client socket */
	ATF_REQUIRE((sockfd2 = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);

	/* Audit record must contain AF_UNIX address path & sockfd2 */
	snprintf(regex, 40, "connect.*0x%x.*%s.*success", sockfd2, SERVER_PATH);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, connect(sockfd2, (struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);

	/* Close all socket descriptors */
	close(sockfd);
	close(sockfd2);
}

ATF_TC_CLEANUP(connect_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(connect_failure);
ATF_TC_HEAD(connect_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"connect(2) call");
}

ATF_TC_BODY(connect_failure, tc)
{
	/* Preliminary socket setup */
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Audit record must contain AF_UNIX address path & Hex(-1) */
	snprintf(regex, 40, "connect.*0x%x.*%s.*failure", ERROR, SERVER_PATH);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, connect(ERROR, (struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(connect_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(connectat_success);
ATF_TC_HEAD(connectat_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"connectat(2) call");
}

ATF_TC_BODY(connectat_success, tc)
{
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Server Socket: Assign address and listen for connection */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Non-blocking server socket */
	ATF_REQUIRE(fcntl(sockfd, F_SETFL, O_NONBLOCK) != -1);
	/* Bind to the specified address and wait for connection */
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	ATF_REQUIRE_EQ(0, listen(sockfd, 1));

	/* Set up "blocking" client socket */
	ATF_REQUIRE((sockfd2 = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);

	/* Audit record must contain sockfd2 */
	snprintf(regex, 30, "connectat.*0x%x.*return,success", sockfd2);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, connectat(AT_FDCWD, sockfd2, \
		(struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);

	/* Close all socket descriptors */
	close(sockfd);
	close(sockfd2);
}

ATF_TC_CLEANUP(connectat_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(connectat_failure);
ATF_TC_HEAD(connectat_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"connectat(2) call");
}

ATF_TC_BODY(connectat_failure, tc)
{
	/* Preliminary socket setup */
	struct sockaddr_un server;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Audit record must contain Hex(-1) */
	snprintf(regex, 30, "connectat.*0x%x.*return,failure", ERROR);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, connectat(AT_FDCWD, ERROR, \
		(struct sockaddr *)&server, len));
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(connectat_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(accept_success);
ATF_TC_HEAD(accept_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"accept(2) call");
}

ATF_TC_BODY(accept_success, tc)
{
	int clientfd;
	struct sockaddr_un server, client;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Server Socket: Assign address and listen for connection */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Non-blocking server socket */
	ATF_REQUIRE(fcntl(sockfd, F_SETFL, O_NONBLOCK) != -1);
	/* Bind to the specified address and wait for connection */
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	ATF_REQUIRE_EQ(0, listen(sockfd, 1));

	/* Set up "blocking" client socket */
	ATF_REQUIRE((sockfd2 = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	ATF_REQUIRE_EQ(0, connect(sockfd2, (struct sockaddr *)&server, len));

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE((clientfd = accept(sockfd, \
		(struct sockaddr *)&client, &len)) != -1);

	/* Audit record must contain clientfd & sockfd2 */
	snprintf(regex, 40, \
		"accept.*0x%x.*return,success,%d", sockfd, clientfd);
	check_audit(fds, regex, pipefd);

	/* Close all socket descriptors */
	close(sockfd);
	close(sockfd2);
	close(clientfd);
}

ATF_TC_CLEANUP(accept_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(accept_failure);
ATF_TC_HEAD(accept_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"accept(2) call");
}

ATF_TC_BODY(accept_failure, tc)
{
	/* Preliminary socket setup */
	struct sockaddr_un client;
	len = sizeof(struct sockaddr_un);

	/* Audit record must contain Hex(-1) */
	snprintf(regex, 30, "accept.*0x%x.*return,failure", ERROR);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, accept(ERROR, (struct sockaddr *)&client, &len));
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(accept_failure, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(shutdown_success);
ATF_TC_HEAD(shutdown_success, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of a successful "
					"shutdown(2) call");
}

ATF_TC_BODY(shutdown_success, tc)
{
	int clientfd;
	struct sockaddr_un server, client;
	assign_address(&server);
	len = sizeof(struct sockaddr_un);

	/* Server Socket: Assign address and listen for connection */
	ATF_REQUIRE((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	/* Non-blocking server socket */
	ATF_REQUIRE(fcntl(sockfd, F_SETFL, O_NONBLOCK) != -1);
	/* Bind to the specified address and wait for connection */
	ATF_REQUIRE_EQ(0, bind(sockfd, (struct sockaddr *)&server, len));
	ATF_REQUIRE_EQ(0, listen(sockfd, 1));

	/* Set up "blocking" client and connect with non-blocking server*/
	ATF_REQUIRE((sockfd2 = socket(PF_UNIX, SOCK_STREAM, 0)) != -1);
	ATF_REQUIRE_EQ(0, connect(sockfd2, (struct sockaddr *)&server, len));
	ATF_REQUIRE((clientfd = accept(sockfd, \
		(struct sockaddr *)&client, &len)) != -1);

	/* Audit record must contain clientfd & sockfd2 */
	snprintf(regex, 40, "shutdown.*0x%x.*return,success", clientfd);

	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(0, shutdown(clientfd, SHUT_RDWR));
	check_audit(fds, regex, pipefd);

	/* Close all socket descriptors */
	close(sockfd);
	close(sockfd2);
	close(clientfd);
}

ATF_TC_CLEANUP(shutdown_success, tc)
{
	cleanup();
}


ATF_TC_WITH_CLEANUP(shutdown_failure);
ATF_TC_HEAD(shutdown_failure, tc)
{
	atf_tc_set_md_var(tc, "descr", "Tests the audit of an unsuccessful "
					"shutdown(2) call");
}

ATF_TC_BODY(shutdown_failure, tc)
{
	/* Audit record must contain Hex(-1) */
	snprintf(regex, 30, "shutdown.*0x%x.*return,failure", ERROR);
	FILE *pipefd = setup(fds, "nt");
	ATF_REQUIRE_EQ(-1, shutdown(ERROR, SHUT_RDWR));
	check_audit(fds, regex, pipefd);
}

ATF_TC_CLEANUP(shutdown_failure, tc)
{
	cleanup();
}


ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, socket_success);
	ATF_TP_ADD_TC(tp, socket_failure);
	ATF_TP_ADD_TC(tp, socketpair_success);
	ATF_TP_ADD_TC(tp, socketpair_failure);
	ATF_TP_ADD_TC(tp, setsockopt_success);
	ATF_TP_ADD_TC(tp, setsockopt_failure);

	ATF_TP_ADD_TC(tp, bind_success);
	ATF_TP_ADD_TC(tp, bind_failure);
	ATF_TP_ADD_TC(tp, bindat_success);
	ATF_TP_ADD_TC(tp, bindat_failure);
	ATF_TP_ADD_TC(tp, listen_success);
	ATF_TP_ADD_TC(tp, listen_failure);

	ATF_TP_ADD_TC(tp, connect_success);
	ATF_TP_ADD_TC(tp, connect_failure);
	ATF_TP_ADD_TC(tp, connectat_success);
	ATF_TP_ADD_TC(tp, connectat_failure);
	ATF_TP_ADD_TC(tp, accept_success);
	ATF_TP_ADD_TC(tp, accept_failure);

	ATF_TP_ADD_TC(tp, shutdown_success);
	ATF_TP_ADD_TC(tp, shutdown_failure);

	return (atf_no_error());
}
