#ifndef _AUTOSA_SCHEDULE_TREE_H
#define _AUTOSA_SCHEDULE_TREE_H

#include <isl/schedule_node.h>

__isl_give isl_schedule_node *autosa_tree_move_down_to_depth(
    __isl_take isl_schedule_node *node, int depth,
    __isl_keep isl_union_set *core);
__isl_give isl_schedule_node *autosa_tree_move_down_to_array(
    __isl_take isl_schedule_node *node, __isl_keep isl_union_set *core);
__isl_give isl_schedule_node *autosa_tree_move_up_to_array(
    __isl_take isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_tree_move_down_to_local(
    __isl_take isl_schedule_node *node, __isl_keep isl_union_set *core);
__isl_give isl_schedule_node *autosa_tree_move_down_to_kernel(
    __isl_take isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_tree_move_up_to_kernel(
    __isl_take isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_tree_move_down_to_pe(
    __isl_take isl_schedule_node *node, __isl_keep isl_union_set *core);
__isl_give isl_schedule_node *autosa_tree_move_up_to_pe(
    __isl_take isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_tree_move_down_to_mark(
    __isl_take isl_schedule_node *node, __isl_keep isl_union_set *core,
    const char *mark);
__isl_give isl_schedule_node *autosa_tree_move_up_to_mark(
    __isl_take isl_schedule_node *node, const char *mark);
__isl_give isl_schedule_node *autosa_tree_move_down_to_io_mark(
    __isl_take isl_schedule_node *node, __isl_keep isl_union_set *core,
    int io_level);

int autosa_tree_node_is_kernel(__isl_keep isl_schedule_node *node);

__isl_give isl_schedule_node *autosa_tree_insert_local_before_array(
    __isl_take isl_schedule_node *node);

/* Band node */
__isl_give isl_multi_val *construct_band_tile_sizes(
    __isl_keep isl_schedule_node *node, int *tile_size);
struct autosa_node_band_prop *extract_node_band_prop(
    __isl_keep isl_schedule_node *node);
struct autosa_node_band_prop *autosa_node_band_prop_free(
    __isl_take struct autosa_node_band_prop *prop);
isl_bool is_permutable_node(__isl_keep isl_schedule_node *node);
isl_bool is_dep_uniform(__isl_keep isl_basic_map *bmap, void *user);
isl_bool is_dep_uniform_wrap(__isl_keep isl_map *map, void *user);
isl_bool uniform_dep_check(__isl_keep isl_schedule *schedule,
                           struct ppcg_scop *scop);
__isl_give isl_vec *get_dep_dis_at_node(__isl_keep isl_basic_map *dep,
                                        __isl_keep isl_schedule_node *band);
__isl_give isl_schedule_node *loop_interchange_at_node(
    __isl_take isl_schedule_node *node, isl_size level1, isl_size level2);
__isl_give isl_schedule_node *get_outermost_permutable_node(
    __isl_keep isl_schedule *schedule);
__isl_give isl_schedule_node *get_innermost_permutable_node(
    __isl_keep isl_schedule *schedule);
__isl_give isl_schedule_node *tile_band(__isl_take isl_schedule_node *node,
                                        __isl_take isl_multi_val *sizes);
__isl_give isl_schedule_node *autosa_tile_band(
    __isl_take isl_schedule_node *node, __isl_keep int *sizes);
__isl_give isl_schedule_node *autosa_node_band_tile_loop(
    __isl_take isl_schedule_node *node, int tile_size, int pos);
__isl_give isl_schedule_node *clear_pe_opt_prop(
    __isl_take isl_schedule_node *node, void *user);
__isl_give isl_schedule_node *restore_node_band_prop(
    __isl_take isl_schedule_node *node,
    __isl_take struct autosa_node_band_prop *prop);
__isl_give isl_schedule_node *autosa_node_interchange(
    __isl_take isl_schedule_node *node);
isl_bool no_permutable_node(__isl_keep isl_schedule_node *node, void *user);
isl_bool isl_schedule_node_is_io_mark(__isl_keep isl_schedule_node *node,
                                      int io_level);
int is_node_under_simd(__isl_keep isl_schedule_node *node);
int is_node_under_latency(__isl_keep isl_schedule_node *node);
int *extract_band_upper_bounds(__isl_keep isl_schedule_node *node);
__isl_give isl_union_set *set_schedule_eq(__isl_keep isl_schedule_node *node,
                                          __isl_keep isl_id_list *names);
isl_bool is_flow_dep_carried_by_array_part_loops(
    __isl_keep isl_schedule *schedule, struct autosa_array_ref_group *group,
    struct autosa_kernel *kernel);
__isl_give isl_schedule_node *reorder_band_by_dep_dis(
    __isl_take isl_schedule_node *node);
__isl_give isl_schedule_node *sched_pos_setup(
    __isl_take isl_schedule_node *node);
int get_band_single_schedule_val(__isl_keep isl_schedule_node *node);
int get_last_sched_dim_val(__isl_keep isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_atomic_ancestors(
    __isl_take isl_schedule_node *node);
int is_dep_carried_by_node(__isl_keep isl_basic_map *dep,
                           __isl_keep isl_schedule_node *node);
__isl_give isl_schedule_node *autosa_node_sink_to_depth(
    __isl_take isl_schedule_node *node, int depth);
__isl_give isl_schedule_node *autosa_node_sink_to_mark(
    __isl_take isl_schedule_node *node, const char *name);
int is_marked(__isl_keep isl_schedule_node *node, const char *name);

/* Schedule */
__isl_give isl_schedule *compute_schedule(struct autosa_gen *gen);
__isl_give isl_schedule *get_schedule(struct autosa_gen *gen);
__isl_give isl_schedule *merge_outer_bands(__isl_give isl_schedule *schedule,
                                           struct autosa_gen *gen);

#endif
