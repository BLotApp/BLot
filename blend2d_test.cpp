#include <blend2d.h>
#include <cstdio>

int main() {
    BLImage img(256, 256, BL_FORMAT_PRGB32);
    BLContext ctx(img);

    // Fill background white
    ctx.setCompOp(BL_COMP_OP_SRC_COPY);
    ctx.fillAll();

    // Draw a red circle
    ctx.setCompOp(BL_COMP_OP_SRC_OVER);
    ctx.setFillStyle(BLRgba32(0xFF0000FF)); // Red, premultiplied
    ctx.fillEllipse(128, 128, 100, 100);

    ctx.end();

    BLResult result = img.writeToFile("blend2d_test.png");
    if (result == BL_SUCCESS) {
        printf("Successfully wrote blend2d_test.png\n");
        return 0;
    } else {
        printf("Failed to write blend2d_test.png (error code: %d)\n", result);
        return 1;
    }
} 