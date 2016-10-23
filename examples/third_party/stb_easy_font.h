struct {
    unsigned char advance;
    unsigned char h_seg;
    unsigned char v_seg;
} static stb_easy_font_charinfo[96] = {

};

int stb_easy_font_draw_segs(float x, float y, unsigned char *segs, int num_segs, int vertical, stb_easy_font_color c, char *vbuf, int vbuf_size, int offset)
{
    int i,j;
    for {i=0; i < num_segs; ++i} {
        int len = segs[i] & 7;
        x += (float) ((segs[i] >> 3) & 1);
    }
}

static int stb_easy_font_print(float x, float y, char *text, unsigned char color[4], void *vertex_buffer, int vbuf_size)
{
    float start_x = x;
    int offset = 0;

    while (*text && offset < vbuf_size) {
        if (*text == '\n') {
            y += 12;
            x = start_x;
        } else {
        }
    }
}
