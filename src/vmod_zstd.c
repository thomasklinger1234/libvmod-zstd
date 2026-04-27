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
#define VZSTD_LOGf(tag, vxid, fmt, ...) VSL((tag), (vxid), "ZSTD: " fmt, __VA_ARGS__)
#define VZSTD_LOG(tag, vxid, fmt) VZSTD_LOGf((tag), (vxid), "%s", fmt)

static enum vfp_status v_matchproto_(vfp_init_f)
vfp_zstd_init(VRT_CTX, struct vfp_ctx *vc, struct vfp_entry *vfe) {
	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);

	VZSTD_LOG(SLT_Notice, NO_VXID, "initializing vfp_zstd");

	return (VFP_OK);
}

static void v_matchproto_(vfp_fini_f)
vfp_zstd_fini(struct vfp_ctx *vc, struct vfp_entry *vfe) {
	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);

	VZSTD_LOG(SLT_Notice, NO_VXID, "destroying vfp_zstd");
}

static enum vfp_status v_matchproto_(vfp_pull_f)
vfp_zstd_pull(struct vfp_ctx *vc, struct vfp_entry *vfe, void *ptr, ssize_t *lenp) {
	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);

    return (VFP_END);
}

static const struct vfp vfp_zstd = {
	.name = "zstd",
	.init = vfp_zstd_init,
	.fini = vfp_zstd_fini,
	.pull = vfp_zstd_pull,
	.priv1 = NULL,
};

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

    const char *err;

    switch(e) {
    case VCL_EVENT_LOAD:
        VZSTD_LOGf(SLT_Debug, NO_VXID, "adding vfp_zstd as [%s]", vfp_zstd.name);
        err = VRT_AddFilter(ctx, &vfp_zstd, NULL);
        if (err != NULL) {
            VZSTD_LOGf(SLT_Error, NO_VXID, "adding vfp_zstd [%s] failed with error [%s]", vfp_zstd.name, err);
            return(1);
        }
        return (0);
    case VCL_EVENT_DISCARD:
        VZSTD_LOGf(SLT_Debug, NO_VXID, "removing vfp_zstd [%s]", vfp_zstd.name);
        VRT_RemoveFilter(ctx, &vfp_zstd, NULL);
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