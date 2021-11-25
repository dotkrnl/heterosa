#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include <isl/id.h>
#include <isl/space.h>
#include <isl/schedule.h>
#include <isl/schedule_node.h>

#include "ppcg_options.h"

#ifdef __cplusplus
extern "C"
{
#endif

	__isl_give isl_set *parametrization(__isl_take isl_space *space,
																			int len, int first, __isl_keep isl_id_list *names);

	__isl_give isl_schedule *ppcg_compute_non_grouping_schedule(
			__isl_take isl_schedule_constraints *sc, struct ppcg_options *options);
	__isl_give isl_schedule *ppcg_compute_schedule(
			__isl_take isl_schedule_constraints *sc,
			__isl_keep isl_schedule *schedule, struct ppcg_options *options);

	__isl_give isl_schedule *ppcg_get_schedule(isl_ctx *ctx,
																						 struct ppcg_options *options,
																						 __isl_give isl_schedule *(*compute)(void *user), void *user);

	__isl_give isl_schedule_node *ppcg_set_schedule_node_type(
			__isl_take isl_schedule_node *node, enum isl_ast_loop_type type);

#ifdef __cplusplus
}
#endif

#endif
