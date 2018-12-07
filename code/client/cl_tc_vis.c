#include "client.h"
#include "../qcommon/cm_local.h"
#include "../qcommon/cm_patch.h"

static qhandle_t bboxShader;
static qhandle_t bboxShader_nocull;

static qboolean trigger[MAX_SUBMODELS];

static cvar_t *trigger_draw;
static cvar_t *clips_draw;


static void parse_triggers(char *entities);
static void R_DrawBBox(vec3_t origin, vec3_t mins, vec3_t maxs, vec4_t color);

void tc_vis_init(void) {
	bboxShader = re.RegisterShader("bbox");
	bboxShader_nocull = re.RegisterShader("bbox_nocull");

	trigger_draw = Cvar_Get("bxdfe_triggers_draw", "0", CVAR_ARCHIVE);
	clips_draw = Cvar_Get("bxdfe_clips_draw", "0", CVAR_ARCHIVE);

	memset(&trigger, 0, sizeof(trigger));
	parse_triggers(cm.entityString);
}

void tc_vis_render(void) {
	if (trigger_draw->integer) {
		for (int i = 0; i < cm.numSubModels; i++) {
			if (trigger[i]) {
				vec4_t color = { 0, 128, 0, 255 };
				R_DrawBBox(vec3_origin, cm.cmodels[i].mins, cm.cmodels[i].maxs, color);
			}
		}
	}
	
	if (clips_draw->integer) {
		for (int i = 0; i < cm.numBrushes; i++) {
			cbrush_t *brush = &cm.brushes[i];
			if (brush->contents & CONTENTS_PLAYERCLIP) {
				vec4_t color = { 255, 0, 0, 255 };
				R_DrawBBox(vec3_origin, brush->bounds[0], brush->bounds[1], color);
			}
		}
		for (int i = 0; i < cm.numSurfaces; i++) {
			cPatch_t *patch = cm.surfaces[i];
			if (patch && patch->contents & CONTENTS_PLAYERCLIP) {
				vec4_t color = { 255, 0, 0, 255 };
				R_DrawBBox(vec3_origin, patch->pc->bounds[0], patch->pc->bounds[1], color);
			}
		}
	}
}

// ripped from breadsticks
static void parse_triggers(char *entities) {
	for (;; ) {
		qboolean is_trigger = qfalse;
		int model = -1;

		char *token = COM_Parse(&entities);
		if (!entities)
			break;

		if (token[0] != '{')
			Com_Error(ERR_DROP, "mape is borked\n");

		for (;; ) {
			token = COM_Parse(&entities);

			if (token[0] == '}')
				break;

			if (!Q_stricmp(token, "model")) {
				token = COM_Parse(&entities);
				if (token[0] == '*')
					model = atoi(token + 1);
			}

			if (!Q_stricmp(token, "classname")) {
				token = COM_Parse(&entities);
				is_trigger = !!Q_stristr(token, "trigger");
			}
		}

		if (is_trigger && model > 0) {
			trigger[model] = qtrue;
		}
	}
}

// ripped from breadsticks
static void R_DrawBBox(vec3_t origin, vec3_t mins, vec3_t maxs, vec4_t color) {
	int i;
	float extx, exty, extz;
	polyVert_t verts[4];
	vec3_t corners[8];

	// get the extents (size)
	extx = maxs[0] - mins[0];
	exty = maxs[1] - mins[1];
	extz = maxs[2] - mins[2];

	// set the polygon's texture coordinates
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;

	// set the polygon's vertex colors
	for (i = 0; i < 4; i++) {
		//memcpy( verts[i].modulate, color, sizeof(verts[i].modulate) );
		verts[i].modulate[0] = color[0];
		verts[i].modulate[1] = color[1];
		verts[i].modulate[2] = color[2];
		verts[i].modulate[3] = color[3];
	}

	VectorAdd(origin, maxs, corners[3]);

	VectorCopy(corners[3], corners[2]);
	corners[2][0] -= extx;

	VectorCopy(corners[2], corners[1]);
	corners[1][1] -= exty;

	VectorCopy(corners[1], corners[0]);
	corners[0][0] += extx;

	for (i = 0; i < 4; i++) {
		VectorCopy(corners[i], corners[i + 4]);
		corners[i + 4][2] -= extz;
	}

	// top
	VectorCopy(corners[0], verts[0].xyz);
	VectorCopy(corners[1], verts[1].xyz);
	VectorCopy(corners[2], verts[2].xyz);
	VectorCopy(corners[3], verts[3].xyz);
	re.AddPolyToScene(bboxShader, 4, verts, 1);

	// bottom
	VectorCopy(corners[7], verts[0].xyz);
	VectorCopy(corners[6], verts[1].xyz);
	VectorCopy(corners[5], verts[2].xyz);
	VectorCopy(corners[4], verts[3].xyz);
	re.AddPolyToScene(bboxShader, 4, verts, 1);

	// top side
	VectorCopy(corners[3], verts[0].xyz);
	VectorCopy(corners[2], verts[1].xyz);
	VectorCopy(corners[6], verts[2].xyz);
	VectorCopy(corners[7], verts[3].xyz);
	re.AddPolyToScene(bboxShader_nocull, 4, verts, 1);

	// left side
	VectorCopy(corners[2], verts[0].xyz);
	VectorCopy(corners[1], verts[1].xyz);
	VectorCopy(corners[5], verts[2].xyz);
	VectorCopy(corners[6], verts[3].xyz);
	re.AddPolyToScene(bboxShader_nocull, 4, verts, 1);

	// right side
	VectorCopy(corners[0], verts[0].xyz);
	VectorCopy(corners[3], verts[1].xyz);
	VectorCopy(corners[7], verts[2].xyz);
	VectorCopy(corners[4], verts[3].xyz);
	re.AddPolyToScene(bboxShader_nocull, 4, verts, 1);

	// bottom side
	VectorCopy(corners[1], verts[0].xyz);
	VectorCopy(corners[0], verts[1].xyz);
	VectorCopy(corners[4], verts[2].xyz);
	VectorCopy(corners[5], verts[3].xyz);
	re.AddPolyToScene(bboxShader_nocull, 4, verts, 1);
}