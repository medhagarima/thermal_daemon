/*
 * thd_util.cpp: Common utility functions
 *
 * Copyright (C) 2026 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 or later as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 *
 * Author Name Andrew Gaul <andrew@gaul.org>
 *
 */

#include "thd_util.h"
#include "thermald.h"
#include <cerrno>
#include <cstdlib>
#include <climits>

bool starts_with(const std::string& s, const char *prefix)
{
    size_t len = strlen(prefix);
    return s.size() >= len && s.compare(0, len, prefix) == 0;
}


static const size_t THD_MAX_STR_CMP_LEN = 4096;

size_t thd_cmp_len(const char *param1, const char *param2) {
	size_t param1_len;
	size_t param2_len;

	if (!param1)
		param1 = "";
	if (!param2)
		param2 = "";

	param1_len = strnlen(param1, THD_MAX_STR_CMP_LEN);
	param2_len = strnlen(param2, THD_MAX_STR_CMP_LEN);

	return (param1_len > param2_len) ? param1_len : param2_len;
}

int thd_strcmp_n(const char *param1, const char *param2) {
	if (!param1)
		param1 = "";
	if (!param2)
		param2 = "";

	return strncmp(param1, param2, thd_cmp_len(param1, param2));
}

int thd_strcasecmp_n(const char *param1, const char *param2) {
	if (!param1)
		param1 = "";
	if (!param2)
		param2 = "";

	return strncasecmp(param1, param2, thd_cmp_len(param1, param2));
}

// Safe integer parsing with validation
int parse_int_value(const std::string &str, int *result, int min_val, int max_val) {
	if (str.empty() || !result) {
		return -1;
	}

	char *endptr;
	errno = 0;
	long val = strtol(str.c_str(), &endptr, 10);

	// Check for conversion errors
	if (errno == ERANGE || endptr == str.c_str()) {
		thd_log_warn("Invalid integer value: '%s'\n", str.c_str());
		return -1;
	}

	// Allow trailing whitespace but not other garbage
	while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n') {
		endptr++;
	}
	if (*endptr != '\0') {
		thd_log_warn("Invalid integer format: '%s'\n", str.c_str());
		return -1;
	}

	// Check range
	if (val < min_val || val > max_val) {
		thd_log_warn("Integer value %ld out of range [%d, %d]: '%s'\n",
				val, min_val, max_val, str.c_str());
		return -1;
	}

	*result = (int)val;
	return 0;
}

// Safe double parsing with validation
int parse_double_value(const std::string &str, double *result, double min_val, double max_val) {
	if (str.empty() || !result) {
		return -1;
	}

	char *endptr;
	errno = 0;
	double val = strtod(str.c_str(), &endptr);

	// Check for conversion errors
	if (errno == ERANGE || endptr == str.c_str()) {
		thd_log_warn("Invalid floating point value: '%s'\n", str.c_str());
		return -1;
	}

	// Allow trailing whitespace but not other garbage
	while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n') {
		endptr++;
	}
	if (*endptr != '\0') {
		thd_log_warn("Invalid floating point format: '%s'\n", str.c_str());
		return -1;
	}

	// Check range
	if (val < min_val || val > max_val) {
		thd_log_warn("Floating point value %f out of range [%f, %f]: '%s'\n",
				val, min_val, max_val, str.c_str());
		return -1;
	}

	*result = val;
	return 0;
}
