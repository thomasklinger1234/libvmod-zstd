#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "cache/cache.h"
#include "cache/cache_filter.h"
#include "vcl.h"

#include "vcc_zstd_if.h"

#include <zstd.h>

#define VZSTD_FAIL(ctx, fmt, ...) VRT_fail((ctx), "ZSTD: " fmt, __VA_ARGS__)
#define VZSTD_LOGf(tag, ctx, fmt, ...) VSL((tag), ((ctx) == NULL || (ctx)->sp == NULL) ? NO_VXID : (ctx)->sp->vxid, "ZSTD: " fmt, __VA_ARGS__)
#define VZSTD_LOG(tag, ctx, fmt) VZSTD_LOGf((tag), (ctx), "%s", fmt)

VCL_STRING
vmod_version(VRT_CTX) {
    (void)ctx;

    return (ZSTD_VERSION_STRING);
}

int
vmod_event(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
    ASSERT_CLI();
    CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
    AN(priv);

    switch(e) {
    case VCL_EVENT_LOAD:
        return (0);
    case VCL_EVENT_DISCARD:
        return (0);
    case VCL_EVENT_WARM:
        return (0);
    case VCL_EVENT_COLD:
        return (0);
    default:
        VZSTD_FAIL(ctx, "unknown VCL event [%d]", e);
    }

    return (UNLIKELY(0));
}