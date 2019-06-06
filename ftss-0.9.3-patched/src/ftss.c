/*
 * Copyright 2009 FableTech
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "apr_shm.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_time.h"

#include "httpd.h"
#include "scoreboard.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif



static const char statuschr[] = { '.','S','_','R','W','K','L','D','C','G','I' };


int main(int argc, char *argv[])
{
	apr_status_t rv;
	apr_pool_t *pool;
	apr_shm_t *shm;
	void *p;
	size_t size;
	global_score *global;
	process_score *parent;
	worker_score *server;
	int i, j;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s /path/to/scoreboard\n", argv[0]);
		return EXIT_FAILURE;
	}

	apr_initialize();

	if ((rv = apr_pool_create(&pool, NULL)) != APR_SUCCESS) {
		fprintf(stderr, "%s: apr_pool_create() failed: %d\n",
			argv[0], rv);
		return EXIT_FAILURE;
	}

	
	if ((rv = apr_shm_attach(&shm, argv[1], pool)) != APR_SUCCESS) {
		fprintf(stderr, "%s: apr_shm_attach() failed: %d\n",
			argv[0], rv);
		return EXIT_FAILURE;
	}

	if (!(p = apr_shm_baseaddr_get(shm))) {
		fprintf(stderr, "%s: apr_shm_baseaddr_get() failed.\n",
			argv[0]);
		return EXIT_FAILURE;
	}

	if ((size = apr_shm_size_get(shm)) < sizeof(*global)) {
		fprintf(stderr, "%s: Scoreboard too small.\n",
			argv[0]);
		return EXIT_FAILURE;
	}
	//printf("p=%p, size = %u\n", p, (unsigned int)size);

	global = p;
	p += APR_ALIGN_DEFAULT(sizeof(*global));

	if (size != (
			APR_ALIGN_DEFAULT(sizeof(*global))
			+ APR_ALIGN_DEFAULT(sizeof(*parent))
				* global->server_limit
			+ APR_ALIGN_DEFAULT(sizeof(*server))
				* global->server_limit
				* global->thread_limit
#ifdef SCOREBOARD_BALANCERS
			+ sizeof(lb_score)
				* global->lb_limit
#endif
	)) {
		fprintf(stderr, "%s: Wrong scoreboard size.\n",
			argv[0]);
		return EXIT_FAILURE;
	}

/*
	printf("global->server_limit = %d\n", global->server_limit);
	printf("global->thread_limit = %d\n", global->thread_limit);
	printf("global->sb_type = %d\n", global->sb_type);
	printf("global->running_generation = %d\n", global->running_generation);
	printf("global->restart_time = %lld\n", global->restart_time);
#ifdef SCOREBOARD_BALANCERS
	printf("global->lb_limit = %d\n", global->lb_limit);
#endif
*/

	parent = p;
	p += APR_ALIGN_DEFAULT(sizeof(*parent)) * global->server_limit;

/*
	for (i=0; i<global->server_limit; i++) {
		if (!parent->pid) continue;
		printf("parent pid = %d\n", parent->pid);
	}
*/

	for (i=0; i<global->server_limit; i++) {
		if (!parent[i].pid) {
			p += APR_ALIGN_DEFAULT(sizeof(*server)) * global->thread_limit;
			continue;
		}
		for (j=0; j<global->thread_limit; j++) {
			server = p;
			p += APR_ALIGN_DEFAULT(sizeof(*server));
			if (server->status == SERVER_DEAD) continue;
			if (server->status >= SERVER_NUM_STATUS) continue;
			//printf("status = %d\n", server->status);
			//printf("->vhost = [%s]\n", server->vhost);
			//printf("->request = [%s]\n", server->request);
			printf("%d", parent[i].pid);
			printf("\t%c", statuschr[server->status]);
			printf("\t%.32s", server->client);
			printf("\t%.32s", server->vhost);
			printf("\t%.64s", server->request);
			printf("\n");
		}
	}

	if ((rv = apr_shm_detach(shm)) != APR_SUCCESS) {
		fprintf(stderr, "%s: apr_shm_detach() failed: %d\n",
			argv[0], rv);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
