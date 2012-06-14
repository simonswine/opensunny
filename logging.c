/* tool to read power production data for SMA solar power convertors
   Copyright flonatel GmbH & Co. KG, 2012

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/*
 * Implements basic logging.
 */
#include "logging.h"

#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

logging_p logger;

static void logging_timestamp(logging_p self)
{
	struct timeval tv;
	gettimeofday(&tv, 0);

	struct tm localtime_res;
    struct tm * result_localtime = localtime_r(&tv.tv_sec, &localtime_res);

	char buf[64];
	size_t const slen = strftime(buf, 64, "%Y-%m-%dT%H:%M:%S", result_localtime);

	snprintf(buf + slen, 64 - slen - 1, ".%06ld:", tv.tv_usec);

	fprintf(self->logfile, "%s", buf);
}

logging_p logging_constructor(FILE * logfile)
{
	logging_p self = (logging_p)malloc(sizeof(logging_t));
	assert(self!=0);
	self->logfile = logfile;
	self->loglevel = ll_info;
	return self;
}

void logging_destructor(logging_p self)
{
	fflush(self->logfile);
	/* Do not close the logfile - this must be done by the caller.
	 * This class just uses the logfile.
	 * This makes it possible to use stdout or stderr for logging.
	 */
	free(self);
}

void logging_set_loglevel(logging_p self, loglevel_t loglevel)
{
	self->loglevel = loglevel;
}

void logging_generic(logging_p self, loglevel_t level,
		char const * format, ...)
{
	if(self->loglevel>level) return;

	va_list argp;
	va_start(argp, format);

	logging_timestamp(self);
	fprintf(self->logfile, "%s",level2type(level));
	fprintf(self->logfile, ":");
	vfprintf(self->logfile, format, argp);
	fprintf(self->logfile, "\n");
	fflush(self->logfile);
}

char const * level2type_array[] =
{
		"TRACE", "DEBUG", "VERBOSE", "INFO", "WARNING", "ERROR", "FATAL"
};

char const * const level2type(loglevel_t level)
{
	return level2type_array[level];
}

static void logging_global_logger_destructor()
{
	log_debug("Destroying global logger.");
	logging_destructor(logger);
}

void log_init()
{
	logger = logging_constructor(stderr);
	logging_set_loglevel(logger, ll_debug);
	log_debug("Created global logger.");
	atexit(logging_global_logger_destructor);
}
