/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Peter Vrabec  <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <math.h>

#include <cve_nvd.h>

#include "oscap-tool.h"

static bool getopt_cve(int argc, char **argv, struct oscap_action *action);
static int app_cve_validate(const struct oscap_action *action);
static int app_cve_find(const struct oscap_action *action);

static struct oscap_module* CVE_SUBMODULES[];

struct oscap_module OSCAP_CVE_MODULE = {
    .name = "cve",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Common Vulnerabilities and Exposures",
    .submodules = CVE_SUBMODULES
};

static struct oscap_module CVE_VALIDATE_MODULE = {
    .name = "validate",
    .parent = &OSCAP_CVE_MODULE,
    .summary = "Validate CVE NVD feed",
    .usage = "nvd-feed.xml",
    .help = "Validate CVE NVD feed.",
    .opt_parser = getopt_cve,
    .func = app_cve_validate
};

static struct oscap_module CVE_FIND_MODULE = {
    .name = "find",
    .parent = &OSCAP_CVE_MODULE,
    .summary = "Find particular CVE in CVE NVD feed",
    .usage = "CVE nvd-feed.xml",
    .help = "Find particular CVE in CVE NVD feed.",
    .opt_parser = getopt_cve,
    .func = app_cve_find
};

static struct oscap_module* CVE_SUBMODULES[] = {
    &CVE_VALIDATE_MODULE,
    &CVE_FIND_MODULE,
    NULL
};

static int app_cve_validate(const struct oscap_action *action)
{
	int ret;
        int result;
	char *doc_version = "2.0";

        ret=oscap_validate_document(action->cve_action->file, action->doctype, doc_version, reporter, (void*) action);
        if (ret==-1) {
                result=OSCAP_ERROR;
                goto cleanup;
        }
        else if (ret==1) {
                result=OSCAP_FAIL;
        }
        else
                result=OSCAP_OK;

        if (result==OSCAP_FAIL)
                validation_failed(action->cve_action->file, action->doctype, doc_version);

cleanup:
        if (oscap_err())
                fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

        free(action->cve_action);
        return result;
}

static int app_cve_find(const struct oscap_action *action)
{
        return OSCAP_OK;
}

bool getopt_cve(int argc, char **argv, struct oscap_action *action)
{
        if( (action->module == &CVE_VALIDATE_MODULE)) {
                if( argc != 4 ) {
                        oscap_module_usage(action->module, stderr, "Wrong number of parameteres.\n");
                        return false;
                }
                action->doctype = OSCAP_DOCUMENT_CVE_FEED;
                action->cve_action = malloc(sizeof(struct cve_action));
                action->cve_action->file=argv[3];
        }

	return true;
}

