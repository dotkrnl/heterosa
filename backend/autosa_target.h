#ifndef _AUTOSA_TARGET_H
#define _AUTOSA_TARGET_H

#include <pet.h>

#include "ppcg.h"
#include "ppcg_options.h"

#ifdef __cplusplus
extern "C" {
#endif

int generate_autosa_target(isl_ctx *ctx, struct ppcg_options *options,
                           const char *input);

#ifdef __cplusplus
}
#endif

#endif