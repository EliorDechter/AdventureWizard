#ifndef WZ_LAYOUT
#define WZ_LAYOUT

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define WZL_LOG(...) (void)0;

#define WZ_FLEX_FIT_LOOSE 0
#define WZ_FLEX_FIT_TIGHT 1

#define WZ_LAYOUT_NONE 0
#define WZ_LAYOUT_HORIZONTAL 1
#define WZ_LAYOUT_VERTICAL 2

#define	MAIN_AXIS_SIZE_TYPE_MIN  0
#define	MAIN_AXIS_SIZE_TYPE_MAX  1

#define WZ_UINT_MAX 4294967295

#define WZ_LOG_MESSAGE_MAX_SIZE 256

//typedef enum {
//	WzAlignVCenter = 1 << 0,
//	WzAlignHCenter = 1 << 1,
//} WzAlignment;

typedef struct wzl_str
{
	char* str;
	unsigned int len;
} wzl_str;

typedef struct WzlRect
{
	//unsigned int index;
	int x, y;
	unsigned int w, h;
} WzLayoutRect;

enum
{
	CROSS_AXIS_ALIGNMENT_START,
	CROSS_AXIS_ALIGNMENT_END,
	CROSS_AXIS_ALIGNMENT_CENTER,
	CROSS_AXIS_ALIGNMENT_STRETCH,
	CROSS_AXIS_ALIGNMENT_BASELINE,
	CROSS_AXIS_ALIGNMENT_TOTAL,
};

typedef struct WzWidgetDescriptor
{
	const char* file;
	unsigned int line;
	unsigned int constraint_min_w, constraint_min_h, constraint_max_w, constraint_max_h;
	unsigned int layout;
	unsigned int pad_left, pad_right, pad_top, pad_bottom;
	unsigned int gap;
	unsigned int* children;
	unsigned int children_count;
	unsigned int flex_factor;
	unsigned char free_from_parent;
	unsigned char flex_fit;
	unsigned char main_axis_size_type;
	unsigned int alignment;
	unsigned int cross_axis_alignment;
	int x, y;
} WzWidgetDescriptor;

typedef struct WzDebugInfo
{
	unsigned int stage;
	unsigned int index;
	unsigned int constraint_min_w, constraint_min_h, constraint_max_w, constraint_max_h;
	unsigned int x, y, w, h;
} WzDebugInfo;

typedef struct WzLogMessage
{
	char str[WZ_LOG_MESSAGE_MAX_SIZE];
} WzLogMessage;

void wz_do_layout(unsigned int index,
	const WzWidgetDescriptor* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int *failed);

#endif