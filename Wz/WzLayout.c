#if 0

#define MAX_NUM_CHUNKS 1024
#define CHUNK_SIZE     16
#define TOTAL_SLOTS    (MAX_NUM_CHUNKS * CHUNK_SIZE)

// ---------------------------------------------------------------------------
// ISPC soa<N> turns this AOS struct into SOA automatically.
// soa<16> means every 16 consecutive elements are stored field-by-field,
// matching our CHUNK_SIZE so one chunk = one SOA block.
// ---------------------------------------------------------------------------
struct WzWidgetDataNew {
    float min_w, max_w;
    float min_h, max_h;

    float absolute_x, absolute_y, absolute_w, absolute_h;
    float relative_x, relative_y;
    float available_w, available_h;

    unsigned int8 layout;
    unsigned int8 shrink_wrap;
    unsigned int8 flex;

    unsigned int chunk_index;
    bool horizontal;
};

struct ChunkData {
    unsigned int parent_chunk;
    unsigned int parent_index_inside_chunk;
    unsigned int children_count;
};

// soa<CHUNK_SIZE>: each chunk's 16 slots are stored SOA-style.
// Indexed as:  chunks[chunk_idx].min_w[slot]
extern uniform soa<CHUNK_SIZE> WzWidgetDataNew chunks[MAX_NUM_CHUNKS];
extern uniform ChunkData                       chunks_data[MAX_NUM_CHUNKS];
extern uniform unsigned int                    chunks_count;

// ---------------------------------------------------------------------------
// Pass 1 (reverse): bottom-up min-size propagation
// ---------------------------------------------------------------------------
static void pass_bottomup() {
    for (uniform int i = MAX_NUM_CHUNKS - 1; i > 0; --i) {
        uniform int pidx = chunks_data[i].parent_index_inside_chunk;
        uniform int pchunk = chunks_data[i].parent_chunk;

        uniform float min_w_accum = 0.f;
        uniform float min_h_accum = 0.f;
        uniform float biggest_w = 0.f;
        uniform float biggest_h = 0.f;

        foreach(j = 0 ... CHUNK_SIZE) {
            // soa<16> field access: chunks[i].min_w[j] — no gather, contiguous
            varying float cmin_w = chunks[i].min_w[j];
            varying float cmin_h = chunks[i].min_h[j];

            min_w_accum += reduce_add(cmin_w);
            min_h_accum += reduce_add(cmin_h);
            biggest_w = max(biggest_w, reduce_max(cmin_w));
            biggest_h = max(biggest_h, reduce_max(cmin_h));
        }

        uniform float parent_min_w = chunks[pchunk].min_w[pidx];
        uniform float parent_min_h = chunks[pchunk].min_h[pidx];
        uniform bool  horiz = chunks[pchunk].horizontal[pidx];

        min_w_accum = min_w_accum > parent_min_w ? min_w_accum : parent_min_w;
        min_h_accum = min_h_accum > parent_min_h ? min_h_accum : parent_min_h;

        chunks[pchunk].min_w[pidx] = horiz ? min_w_accum : biggest_w;
        chunks[pchunk].min_h[pidx] = !horiz ? min_h_accum : biggest_h;
    }
}

// ---------------------------------------------------------------------------
// Pass 2 (forward): top-down available-space distribution
// ---------------------------------------------------------------------------
static void pass_topdown() {
    for (uniform int i = 1; i < MAX_NUM_CHUNKS; ++i) {
        uniform int pidx = chunks_data[i].parent_index_inside_chunk;
        uniform int pchunk = chunks_data[i].parent_chunk;

        uniform float parent_avail_w = chunks[pchunk].available_w[pidx];
        uniform float parent_avail_h = chunks[pchunk].available_h[pidx];
        uniform float flex_total = 0.f;

        // Sub-pass A: give each child its min, accumulate flex
        foreach(j = 0 ... CHUNK_SIZE) {
            varying float cmin_w = chunks[i].min_w[j];
            varying float cmin_h = chunks[i].min_h[j];
            varying float cflex = (float)chunks[i].flex[j];

            parent_avail_w -= reduce_add(cmin_w);
            parent_avail_h -= reduce_add(cmin_h);
            flex_total += reduce_add(cflex);

            chunks[i].available_w[j] += cmin_w;
            chunks[i].available_h[j] += cmin_h;
        }

        parent_avail_w = max(0.f, parent_avail_w);
        parent_avail_h = max(0.f, parent_avail_h);

        uniform float w_per_flex = (flex_total > 0.f) ? parent_avail_w / flex_total : 0.f;
        uniform float h_per_flex = (flex_total > 0.f) ? parent_avail_h / flex_total : 0.f;

        // Sub-pass B: distribute flex bonus
        foreach(j = 0 ... CHUNK_SIZE) {
            varying float f = (float)chunks[i].flex[j];
            varying float avail_w = chunks[i].available_w[j];
            varying float avail_h = chunks[i].available_h[j];

            chunks[i].available_w[j] = avail_w + max(0.f, min(f * w_per_flex, chunks[i].max_w[j] - avail_w));
            chunks[i].available_h[j] = avail_h + max(0.f, min(f * h_per_flex, chunks[i].max_h[j] - avail_h));
        }
    }
}

// ---------------------------------------------------------------------------
// Pass 3: materialise absolute sizes — fully parallel
// ---------------------------------------------------------------------------
static void pass_absolute() {
    for (uniform int i = 0; i < MAX_NUM_CHUNKS; ++i) {
        foreach(j = 0 ... CHUNK_SIZE) {
            chunks[i].absolute_w[j] = chunks[i].available_w[j];
            chunks[i].absolute_h[j] = chunks[i].available_h[j];
        }
    }
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
static void zero_chunks() {
    for (uniform int i = 0; i < MAX_NUM_CHUNKS; ++i) {
        foreach(j = 0 ... CHUNK_SIZE) {
            chunks[i].min_w[j] = 0.f;  chunks[i].max_w[j] = 0.f;
            chunks[i].min_h[j] = 0.f;  chunks[i].max_h[j] = 0.f;
            chunks[i].absolute_x[j] = 0.f;  chunks[i].absolute_y[j] = 0.f;
            chunks[i].absolute_w[j] = 0.f;  chunks[i].absolute_h[j] = 0.f;
            chunks[i].relative_x[j] = 0.f;  chunks[i].relative_y[j] = 0.f;
            chunks[i].available_w[j] = 0.f;  chunks[i].available_h[j] = 0.f;
            chunks[i].layout[j] = 0;    chunks[i].shrink_wrap[j] = 0;
            chunks[i].flex[j] = 0;    chunks[i].chunk_index[j] = 0;
            chunks[i].horizontal[j] = false;
        }

        chunks_data[i].children_count = 0;
    }
}

export void wz_layout_new() {
    zero_chunks();

    chunks_count = 1;

    // Root widget: chunk 0, slot 0
    chunks[0].max_w[0] = 100.f;  chunks[0].max_h[0] = 100.f;
    chunks[0].min_w[0] = 100.f;  chunks[0].min_h[0] = 100.f;
    chunks[0].available_w[0] = 100.f;  chunks[0].available_h[0] = 100.f;

    pass_bottomup();
    pass_topdown();
    pass_absolute();
}

#endif