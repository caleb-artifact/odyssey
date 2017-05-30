
/*
 * machinarium.
 *
 * cooperative multitasking engine.
*/

#include <machinarium.h>
#include <machinarium_private.h>

MACHINE_API int
machine_bind(machine_io_t obj, struct sockaddr *sa)
{
	mm_io_t *io = obj;
	mm_errno_set(0);
	if (io->connected) {
		mm_errno_set(EINPROGRESS);
		return -1;
	}
	int rc;
	rc = mm_io_socket(io, sa);
	if (rc == -1)
		goto error;
	rc = mm_socket_set_reuseaddr(io->fd, 1);
	if (rc == -1) {
		mm_errno_set(errno);
		goto error;
	}
	rc = mm_socket_bind(io->fd, sa);
	if (rc == -1) {
		mm_errno_set(errno);
		goto error;
	}
	rc = machine_io_attach(io);
	if (rc == -1)
		goto error;
	return 0;
error:
	if (io->fd != -1) {
		close(io->fd);
		io->fd = -1;
	}
	io->handle.fd = -1;
	return -1;
}
