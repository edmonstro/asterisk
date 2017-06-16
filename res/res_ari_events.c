/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2012 - 2013, Digium, Inc.
 *
 * David M. Lee, II <dlee@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/res_ari_resource.c.mustache
 */

/*! \file
 *
 * \brief WebSocket resource
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_ari</depend>
	<depend type="module">res_ari_model</depend>
	<depend type="module">res_stasis</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_REGISTER_FILE()

#include "asterisk/app.h"
#include "asterisk/module.h"
#include "asterisk/stasis_app.h"
#include "ari/resource_events.h"
#if defined(AST_DEVMODE)
#include "ari/ari_model_validators.h"
#endif
#include "asterisk/http_websocket.h"

#define MAX_VALS 128

static int ast_ari_events_event_websocket_ws_attempted_cb(struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *headers, const char *session_id)
{
	struct ast_ari_events_event_websocket_args args = {};
	int res = 0;
	RAII_VAR(struct ast_ari_response *, response, NULL, ast_free);
	struct ast_variable *i;

	response = ast_calloc(1, sizeof(*response));
	if (!response) {
		ast_log(LOG_ERROR, "Failed to create response.\n");
		goto fin;
	}

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "app") == 0) {
			/* Parse comma separated list */
			char *vals[MAX_VALS];
			size_t j;

			args.app_parse = ast_strdup(i->value);
			if (!args.app_parse) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (strlen(args.app_parse) == 0) {
				/* ast_app_separate_args can't handle "" */
				args.app_count = 1;
				vals[0] = args.app_parse;
			} else {
				args.app_count = ast_app_separate_args(
					args.app_parse, ',', vals,
					ARRAY_LEN(vals));
			}

			if (args.app_count == 0) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (args.app_count >= MAX_VALS) {
				ast_ari_response_error(response, 400,
					"Bad Request",
					"Too many values for app");
				goto fin;
			}

			args.app = ast_malloc(sizeof(*args.app) * args.app_count);
			if (!args.app) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			for (j = 0; j < args.app_count; ++j) {
				args.app[j] = (vals[j]);
			}
		} else
		if (strcmp(i->name, "subscribeAll") == 0) {
			args.subscribe_all = ast_true(i->value);
		} else
		{}
	}

	res = ast_ari_websocket_events_event_websocket_attempted(ser, headers, &args, session_id);

fin: __attribute__((unused))
	if (!response) {
		ast_http_error(ser, 500, "Server Error", "Memory allocation error");
		res = -1;
	} else if (response->response_code != 0) {
		/* Param parsing failure */
		RAII_VAR(char *, msg, NULL, ast_json_free);
		if (response->message) {
			msg = ast_json_dump_string(response->message);
		} else {
			ast_log(LOG_ERROR, "Missing response message\n");
		}

		if (msg) {
			ast_http_error(ser, response->response_code, response->response_text, msg);
		}
		res = -1;
	}
	ast_free(args.app_parse);
	ast_free(args.app);
	return res;
}

static void ast_ari_events_event_websocket_ws_established_cb(struct ast_websocket *ws_session,
	struct ast_variable *get_params, struct ast_variable *headers)
{
	struct ast_ari_events_event_websocket_args args = {};
	RAII_VAR(struct ast_ari_response *, response, NULL, ast_free);
	struct ast_variable *i;
	RAII_VAR(struct ast_websocket *, s, ws_session, ast_websocket_unref);
	RAII_VAR(struct ast_ari_websocket_session *, session, NULL, ao2_cleanup);

	response = ast_calloc(1, sizeof(*response));
	if (!response) {
		ast_log(LOG_ERROR, "Failed to create response.\n");
		goto fin;
	}

#if defined(AST_DEVMODE)
	session = ast_ari_websocket_session_create(ws_session,
		ast_ari_validate_message_fn());
#else
	session = ast_ari_websocket_session_create(ws_session, NULL);
#endif
	if (!session) {
		ast_log(LOG_ERROR, "Failed to create ARI session\n");
		goto fin;
	}

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "app") == 0) {
			/* Parse comma separated list */
			char *vals[MAX_VALS];
			size_t j;

			args.app_parse = ast_strdup(i->value);
			if (!args.app_parse) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (strlen(args.app_parse) == 0) {
				/* ast_app_separate_args can't handle "" */
				args.app_count = 1;
				vals[0] = args.app_parse;
			} else {
				args.app_count = ast_app_separate_args(
					args.app_parse, ',', vals,
					ARRAY_LEN(vals));
			}

			if (args.app_count == 0) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (args.app_count >= MAX_VALS) {
				ast_ari_response_error(response, 400,
					"Bad Request",
					"Too many values for app");
				goto fin;
			}

			args.app = ast_malloc(sizeof(*args.app) * args.app_count);
			if (!args.app) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			for (j = 0; j < args.app_count; ++j) {
				args.app[j] = (vals[j]);
			}
		} else
		if (strcmp(i->name, "subscribeAll") == 0) {
			args.subscribe_all = ast_true(i->value);
		} else
		{}
	}

	ast_ari_websocket_events_event_websocket_established(session, headers, &args);

fin: __attribute__((unused))
	if (response && response->response_code != 0) {
		/* Param parsing failure */
		RAII_VAR(char *, msg, NULL, ast_json_free);
		if (response->message) {
			msg = ast_json_dump_string(response->message);
		} else {
			ast_log(LOG_ERROR, "Missing response message\n");
		}
		if (msg) {
			ast_websocket_write(ws_session,
				AST_WEBSOCKET_OPCODE_TEXT, msg,	strlen(msg));
		}
	}
	ast_free(args.app_parse);
	ast_free(args.app);
}
int ast_ari_events_user_event_parse_body(
	struct ast_json *body,
	struct ast_ari_events_user_event_args *args)
{
	struct ast_json *field;
	/* Parse query parameters out of it */
	field = ast_json_object_get(body, "application");
	if (field) {
		args->application = ast_json_string_get(field);
	}
	field = ast_json_object_get(body, "source");
	if (field) {
		/* If they were silly enough to both pass in a query param and a
		 * JSON body, free up the query value.
		 */
		ast_free(args->source);
		if (ast_json_typeof(field) == AST_JSON_ARRAY) {
			/* Multiple param passed as array */
			size_t i;
			args->source_count = ast_json_array_size(field);
			args->source = ast_malloc(sizeof(*args->source) * args->source_count);

			if (!args->source) {
				return -1;
			}

			for (i = 0; i < args->source_count; ++i) {
				args->source[i] = ast_json_string_get(ast_json_array_get(field, i));
			}
		} else {
			/* Multiple param passed as single value */
			args->source_count = 1;
			args->source = ast_malloc(sizeof(*args->source) * args->source_count);
			if (!args->source) {
				return -1;
			}
			args->source[0] = ast_json_string_get(field);
		}
	}
	return 0;
}

/*!
 * \brief Parameter parsing callback for /events/user/{eventName}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_events_user_event_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_json *body, struct ast_ari_response *response)
{
	struct ast_ari_events_user_event_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "application") == 0) {
			args.application = (i->value);
		} else
		if (strcmp(i->name, "source") == 0) {
			/* Parse comma separated list */
			char *vals[MAX_VALS];
			size_t j;

			args.source_parse = ast_strdup(i->value);
			if (!args.source_parse) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (strlen(args.source_parse) == 0) {
				/* ast_app_separate_args can't handle "" */
				args.source_count = 1;
				vals[0] = args.source_parse;
			} else {
				args.source_count = ast_app_separate_args(
					args.source_parse, ',', vals,
					ARRAY_LEN(vals));
			}

			if (args.source_count == 0) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (args.source_count >= MAX_VALS) {
				ast_ari_response_error(response, 400,
					"Bad Request",
					"Too many values for source");
				goto fin;
			}

			args.source = ast_malloc(sizeof(*args.source) * args.source_count);
			if (!args.source) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			for (j = 0; j < args.source_count; ++j) {
				args.source[j] = (vals[j]);
			}
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "eventName") == 0) {
			args.event_name = (i->value);
		} else
		{}
	}
	args.variables = body;
	ast_ari_events_user_event(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Application does not exist. */
	case 422: /* Event source not found. */
	case 400: /* Invalid even tsource URI or userevent data. */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /events/user/{eventName}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /events/user/{eventName}\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	ast_free(args.source_parse);
	ast_free(args.source);
	return;
}

/*! \brief REST handler for /api-docs/events.json */
static struct stasis_rest_handlers events_user_eventName = {
	.path_segment = "eventName",
	.is_wildcard = 1,
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_events_user_event_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/events.json */
static struct stasis_rest_handlers events_user = {
	.path_segment = "user",
	.callbacks = {
	},
	.num_children = 1,
	.children = { &events_user_eventName, }
};
/*! \brief REST handler for /api-docs/events.json */
static struct stasis_rest_handlers events = {
	.path_segment = "events",
	.callbacks = {
	},
	.num_children = 1,
	.children = { &events_user, }
};

static int unload_module(void)
{
	ast_ari_remove_handler(&events);
	ao2_cleanup(events.ws_server);
	events.ws_server = NULL;
	ast_ari_websocket_events_event_websocket_dtor();
	stasis_app_unref();
	return 0;
}

static int load_module(void)
{
	int res = 0;

	CHECK_ARI_MODULE_LOADED();

	/* This is scoped to not conflict with CHECK_ARI_MODULE_LOADED */
	{
		struct ast_websocket_protocol *protocol;

		if (ast_ari_websocket_events_event_websocket_init() == -1) {
			return AST_MODULE_LOAD_DECLINE;
		}

		events.ws_server = ast_websocket_server_create();
		if (!events.ws_server) {
			ast_ari_websocket_events_event_websocket_dtor();
			return AST_MODULE_LOAD_DECLINE;
		}

		protocol = ast_websocket_sub_protocol_alloc("ari");
		if (!protocol) {
			ao2_ref(events.ws_server, -1);
			events.ws_server = NULL;
			ast_ari_websocket_events_event_websocket_dtor();
			return AST_MODULE_LOAD_DECLINE;
		}
		protocol->session_attempted = ast_ari_events_event_websocket_ws_attempted_cb;
		protocol->session_established = ast_ari_events_event_websocket_ws_established_cb;
		res |= ast_websocket_server_add_protocol2(events.ws_server, protocol);
	}

	stasis_app_ref();
	res |= ast_ari_add_handler(&events);
	if (res) {
		unload_module();
		return AST_MODULE_LOAD_DECLINE;
	}

	return AST_MODULE_LOAD_SUCCESS;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "RESTful API module - WebSocket resource",
	.support_level = AST_MODULE_SUPPORT_CORE,
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_ari,res_stasis",
);
