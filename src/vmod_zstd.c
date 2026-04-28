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

enum vfp_which { ENC, DEC };

struct vfp_zstd_settings {
    unsigned magic;
#define VFP_ZSTD_SETTINGS_MAGIC 0xc79b73f8

    enum vfp_which which;
};

struct vfp_zstd_priv {
    unsigned magic;
#define VFP_ZSTD_MAGIC 0xc79b73f7

    struct vfp_zstd_settings *settings;
};


static enum vfp_status v_matchproto_(vfp_init_f)
vfp_zstd_init(VRT_CTX, struct vfp_ctx *vc, struct vfp_entry *vfe) {
    const struct vfp_zstd_priv *priv;
    const struct vfp_zstd_settings *settings;

	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);
	CAST_OBJ_NOTNULL(priv, vfe->vfp->priv1, VFP_ZSTD_MAGIC);
	CAST_OBJ_NOTNULL(settings, priv->settings, VFP_ZSTD_SETTINGS_MAGIC);
	settings = priv->settings;

	VZSTD_LOG(SLT_Notice, NO_VXID, "initializing vfp_zstd");

	/*
	 * Ignore partial responses to range requests (as Vinyl does for
	 * gzip and gunzip).
	 */
	if (http_GetStatus(vc->resp) == 206) {
	    VZSTD_LOGf(SLT_Notice, NO_VXID, "skipping vfp_zstd for status code [%d]", http_GetStatus(vc->resp));
		return (VFP_NULL);
    }

    if (settings->which == ENC) {
        if (http_GetHdr(vc->resp, H_Content_Encoding, NULL)) {
	        VZSTD_LOG(SLT_Notice, NO_VXID, "skipping vfp_zstd for missing content-encoding resp header");
            return (VFP_NULL);
        }
        // FIXME: create encoder
    }

    if (settings->which == DEC) {
        if (!http_HdrIs(vc->resp, H_Content_Encoding, "zstd")) {
	        VZSTD_LOG(SLT_Notice, NO_VXID, "skipping vfp_zstd as content encoding is not zstd");
            return (VFP_NULL);
        }
        // FIXME: create decoder
    }

    // FIXME: Initialize params
    // FIXME: set vfe->priv1

    VZSTD_LOG(SLT_Notice, NO_VXID, "updating response headers");
	http_Unset(vc->resp, H_Content_Encoding);
	http_Unset(vc->resp, H_Content_Length);
	RFC2616_Weaken_Etag(vc->resp);
	vc->obj_flags |= OF_CHGCE;

    if (settings->which == ENC) {
        http_SetHeader(vc->resp, "Content-Encoding: zstd");
        RFC2616_Vary_AE(vc->resp);
    }

	return (VFP_OK);
}

static void v_matchproto_(vfp_fini_f)
vfp_zstd_fini(struct vfp_ctx *vc, struct vfp_entry *vfe) {
    const struct vfp_zstd_priv *priv;

	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);
	CAST_OBJ_NOTNULL(priv, vfe->vfp->priv1, VFP_ZSTD_MAGIC);

	if (vfe->priv1 != NULL) {
	    // FIXME: destroy priv1 content
        vfe->priv1 = NULL;
	}

	VZSTD_LOG(SLT_Notice, NO_VXID, "destroyed vfp_zstd");
}

static enum vfp_status v_matchproto_(vfp_pull_f)
vfp_zstd_pull(struct vfp_ctx *vc, struct vfp_entry *vfe, void *ptr, ssize_t *lenp) {
	const struct vfp_zstd_priv *priv;

	CHECK_OBJ_NOTNULL(vc, VFP_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(vfe, VFP_ENTRY_MAGIC);
	AN(vfe->vfp);
	CAST_OBJ_NOTNULL(priv, vfe->vfp->priv1, VFP_ZSTD_MAGIC);
	assert(priv->settings->which == ENC);
	AN(ptr);
	AN(lenp);

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