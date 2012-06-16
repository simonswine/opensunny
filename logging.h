/*
 *  OpenSunny -- OpenSource communication with SMA Readers
 *
 *  Copyright (C) 2012 Christian Simon <simon@swine.de>
 *  Copyright (C) 2012 flonatel GmbH & Co. KG
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OPENSUNNY_LOGGING_H
#define OPENSUNNY_LOGGING_H

#include <stdio.h>

enum loglevel_enum {
	ll_trace, ll_debug, ll_verbose, ll_info, ll_warning, ll_error,
	ll_fatal
};
typedef enum loglevel_enum loglevel_t;

struct logging_struct
{
	/* The file where to output logging. */
	FILE * logfile;
	/* Loglevel: Everything greater or equal this will be logged. */
	loglevel_t loglevel;
};

typedef struct logging_struct logging_t;
typedef logging_t * logging_p;

logging_p logging_constructor(FILE * logfile);
void logging_set_loglevel(logging_p self, loglevel_t loglevel);
void logging_generic(logging_p self, loglevel_t level, char const * format, ...);
void logging_hex(logging_p self, loglevel_t level, char const * const desc,
		void const * const data, unsigned long const len,
		unsigned long const offset);
void logging_destructor(logging_p self);

/* The global log instance. */
extern logging_p logger;

/* Helper / Utilities */
#define log_trace(...) logging_generic(logger, ll_trace, __VA_ARGS__)
#define log_debug(...) logging_generic(logger, ll_debug, __VA_ARGS__)
#define log_verbose(...) logging_generic(logger, ll_verbose, __VA_ARGS__)
#define log_info(...) logging_generic(logger, ll_info, __VA_ARGS__)
#define log_warning(...) logging_generic(logger, ll_warning, __VA_ARGS__)
#define log_error(...) logging_generic(logger, ll_error, __VA_ARGS__)
#define log_fatal(...) logging_generic(logger, ll_fatal, __VA_ARGS__)

#define hlog_trace(dEsC, dAtA, lEn, oFfSeT) \
	logging_hex(logger, ll_trace, dEsC, dAtA, lEn, oFfSeT)
#define hlog_debug(dEsC, dAtA, lEn, oFfSeT) \
	logging_hex(logger, ll_debug, dEsC, dAtA, lEn, oFfSeT)

char const * const level2type(loglevel_t level);

/* This must be called before the first log. */
void log_init();

#endif /* OPENSUNNY_LOGGING_H*/
