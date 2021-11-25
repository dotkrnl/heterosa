#ifndef _AUTOSA_PRINT_H
#define _AUTOSA_PRINT_H

#include <isl/printer.h>

#include "autosa_common.h"

/* Arrays */
__isl_give isl_printer *autosa_array_info_print_call_argument(
    __isl_take isl_printer *p, struct autosa_array_info *array);
__isl_give isl_printer *autosa_array_ref_group_print_prefix(
    struct autosa_array_ref_group *group, __isl_take isl_printer *p);
__isl_give isl_printer *autosa_array_ref_group_print_fifo_name(
    struct autosa_array_ref_group *group, __isl_take isl_printer *p);
__isl_give isl_printer *autosa_print_types(__isl_take isl_printer *p,
                                           struct autosa_types *types, struct autosa_prog *prog);
__isl_give isl_printer *autosa_print_local_declarations(
    __isl_take isl_printer *p, struct autosa_prog *prog);
__isl_give isl_printer *autosa_array_info_print_data_size(
    __isl_take isl_printer *p, struct autosa_array_info *array);
__isl_give isl_printer *autosa_array_info_print_size(
    __isl_take isl_printer *p, struct autosa_array_info *array);
__isl_give isl_printer *autosa_array_info_print_serialize_size(
    __isl_take isl_printer *p, struct autosa_array_info *array);    
__isl_give isl_printer *autosa_print_array_type(__isl_take isl_printer *p,
                                                struct autosa_array_info *array);
__isl_give isl_printer *autosa_print_array_type_with_lane(
    __isl_take isl_printer *p,
    struct autosa_array_info *array, int n_lane);
__isl_give isl_printer *autosa_array_info_print_declaration_argument(
    __isl_take isl_printer *p, struct autosa_array_info *array, int n_lane,
    const char *memory_space, int n_ref);
__isl_give isl_printer *autosa_module_array_info_print_call_argument(
    __isl_take isl_printer *p, struct polysa_array_info *array);
__isl_give isl_printer *autosa_print_var_initialization(
    __isl_take isl_printer *p, struct autosa_kernel_var *var);

/* Utils */
__isl_give isl_printer *print_str_new_line(__isl_take isl_printer *p, const char *str);
__isl_give isl_printer *autosa_print_macros(__isl_take isl_printer *p,
                                            __isl_keep isl_ast_node *node);

/* Kernel */
__isl_give isl_printer *print_kernel_arguments(__isl_take isl_printer *p,
                                               struct autosa_prog *prog, struct autosa_kernel *kernel,
                                               int types, struct hls_info *hls);
__isl_give isl_printer *print_kernel_header(__isl_take isl_printer *p,
                                            struct autosa_prog *prog, struct autosa_kernel *kernel, struct hls_info *hls);

/* HW modules */
void print_module_iterators(FILE *out, struct autosa_hw_module *module);
__isl_give isl_printer *print_module_arguments(
    __isl_take isl_printer *p,
    struct autosa_prog *prog,
    struct autosa_kernel *kernel,
    struct autosa_hw_module *module, int types,
    enum platform target,
    int inter, int arb, int boundary, int serialize);
__isl_give isl_printer *print_pe_dummy_module_arguments(
    __isl_take isl_printer *p,
    struct autosa_prog *prog,
    struct autosa_kernel *kernel,
    struct autosa_pe_dummy_module *pe_dummy_module,
    int types,
    enum platform target);
void print_top_gen_headers(
    struct autosa_prog *prog, struct autosa_hw_top_module *top, struct hls_info *hls);
__isl_give isl_printer *print_top_gen_arguments(__isl_take isl_printer *p,
                                                struct autosa_prog *prog, struct autosa_kernel *kernel, int types);
__isl_give isl_printer *autosa_kernel_print_module_call(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct autosa_prog *prog,
    enum platform target);
void print_func_iterators(FILE *out,
                          struct autosa_drain_merge_func *func);
__isl_give isl_printer *print_serialize_counter(
    __isl_take isl_printer *p, 
    struct autosa_hw_module *module);
__isl_give isl_printer *print_host_serialize_arguments(
    __isl_take isl_printer *p,
    struct autosa_kernel *kernel,
    struct autosa_array_ref_group *group,
    struct autosa_hw_module *module,
    int types,
    int hls);    

/* FIFOs */
__isl_give isl_printer *autosa_fifo_print_declaration_arguments(
    __isl_take isl_printer *p, struct autosa_array_ref_group *group, int n_lane,
    const char *suffix, enum platform target);
__isl_give isl_printer *autosa_fifo_print_call_argument(
    __isl_take isl_printer *p, struct autosa_array_ref_group *group,
    const char *suffix, enum platform target);
__isl_give isl_printer *autosa_kernel_print_fifo_decl(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct autosa_prog *prog, struct hls_info *hls);

/* Statements */
__isl_give isl_printer *autosa_kernel_print_domain(__isl_take isl_printer *p,
                                                   struct autosa_kernel_stmt *stmt);
__isl_give isl_printer *autosa_kernel_print_io(__isl_take isl_printer *p,
                                               struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_io_transfer(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls, const char *iterator_prefix);
__isl_give isl_printer *autosa_kernel_print_io_dram(__isl_take isl_printer *p,
                                                    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_inter_trans(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_intra_trans(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_intra_inter(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_inter_intra(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_state_handle(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_drain_merge(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt, struct hls_info *hls);
__isl_give isl_printer *autosa_kernel_print_host_serialize(
    __isl_take isl_printer *p,
    struct autosa_kernel_stmt *stmt,
    struct hls_info *hls);    

/* Xilinx-specific */
__isl_give isl_printer *print_fifo_type_xilinx(__isl_take isl_printer *p,
                                               struct autosa_array_ref_group *group, int n_lane);
__isl_give isl_printer *print_fifo_rw_xilinx(__isl_take isl_printer *p,
                                             const char *fifo_name, int read);

/* Intel-specific */
__isl_give isl_printer *print_fifo_type_intel(__isl_take isl_printer *p,
                                              struct autosa_array_ref_group *group, int n_lane);
__isl_give isl_printer *print_fifo_rw_intel(__isl_take isl_printer *p,
                                            const char *fifo_name, int read);

#endif
