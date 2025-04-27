#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "seeer.h"
#include "looker.h"
#include "debug_prints.h"


#define BLACK (pixel_t){.r = 0, .g = 0, .b = 0}
#define WHITE (pixel_t){.r = 255, .g = 255, .b = 255}

typedef struct {
    uint32_t x;
    uint32_t y;
} coord_t;

static inline uint16_t sum(pixel_t pixel);
static inline pixel_t* getPix(image_t img, uint32_t x, uint32_t y);
static uint8_t isCrossing(image_t img, pixel_t *pix, int pixelOffset);
static uint32_t getPoints(image_t img, coord_t **points, int pixelOffset);

int compare_ints(const void *a_ptr, const void *b_ptr) {
    const uint32_t a = *((uint32_t *) a_ptr);
    const uint32_t b = *((uint32_t *) b_ptr);

    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}


//TODO: Give pixelOffset a better name
void filter(image_t img, int pixelOffset) {

    DPRINTF("Getting points :)\n");
    coord_t *points;
    uint32_t pointCount = getPoints(img, &points, pixelOffset);

    // We need at least a 5x5 board  (4x4 Queens is impossible).
    if (pointCount < 16) {
        free(points);
        printf("No board detected, or the board is too small.\n");
        return ;
    }
    return;
    DPRINTF("Found %u points!!!\n", pointCount);


    //! The points aren't in the right order!!! shit!!!
    //! FUck!!! Try to sort them??? Maybe???
    //! Honestly you only need the top left point,
    //! but finding the entire board is necesarry for measuring its size.
    //! Maybe put like the points in bins or something based on their x and y.
    //! Like go through the points, see if you've seen that particular x or y
    //! value before, and store that or something. You only need as much
    //! of those bins as the amount of points, so that's nice I guess.

    // Find the median positive horizontal offset.
    int32_t *offsets = malloc(pointCount * sizeof(int32_t));
    int32_t offsets_i = 0;

    for (int32_t i = 1; i < pointCount; i++) {

        int32_t offset = points[i].x - points[i - 1].x;
        // We only care about the X offset for now.
        if (offset < 0) continue;
        offsets[offsets_i] = offset;
    }

    qsort(offsets, offsets_i, sizeof(int32_t), compare_ints);
    int32_t medianOffset = offsets[offsets_i / 2];
    DPRINTF("Nominal offset: %d\n", medianOffset);

    free(offsets);


    // Count the points, and remove the points that aren't in the board.
    // TODO: Expand this to find all offset chains,
    // TODO: and take the one that occurs the most.

    // Keep track of whether we've set the size.
    uint8_t setSize = 0;
    uint32_t size = 0;
    uint8_t prevCorrectOffset = 0;
    uint32_t filteredPointsIndex = 0;

    DPRINTF("Point 0 is [%4d, %4d]\n", points[0].x, points[0].y);

    for (uint32_t i = 0; i < pointCount - 1; i++) {
        int32_t offset = points[i + 1].x - points[i].x;
        uint8_t correctOffset = abs(offset - medianOffset) < MAX_OFFSET_ERROR;

        if (correctOffset || prevCorrectOffset) {
            points[filteredPointsIndex] = points[i];
            filteredPointsIndex++;
            DPRINTF("Added point [%4d, %4d]\n", points[i].x, points[i].y);
        }
        else DPRINTF("Not adding point [%4d, %4d]\n", points[i].x, points[i].y);

        prevCorrectOffset = correctOffset;

        // Start counting at the first occurrence of the offset.
        if (!setSize && correctOffset) size++;
        else if (!setSize && size && !correctOffset) {
            setSize = 1;
        }

    }

    //TODO: This is a bodge
    if (filteredPointsIndex == (size + 1) * (size + 1) - 1) {
        DPRINTF("Adding last point..\n");
        points[filteredPointsIndex++] = points[pointCount - 1];
    }

    // Add one because we're counting the crossings in between the cells.
    // Add one because we're counting distances between those crossings.
    size += 2;
    pointCount = filteredPointsIndex;

    DPRINTF("Board size is %d\n", size);

    if (pointCount != (size - 1) * (size - 1)) {
        printf("Points count (%u) and board size (%d) do not match up!!! Abort!!!\n",
            pointCount, size - 1
        );
    }

    coord_t topLeftPoint = points[0];
    printf("Top left point is [%d %d]\n", topLeftPoint.x, topLeftPoint.y);

    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++) {
            pixel_t *p =
                &img.pixels[topLeftPoint.x + topLeftPoint.y * img.width];
            p->r = 255;
            p->g = 0;
            p->b = 255;
        }
    }

    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++) {
            points[y * size + x] = (coord_t){
                topLeftPoint.x + x * medianOffset - medianOffset / 2,
                topLeftPoint.y + y * medianOffset - medianOffset / 2,
            };
        }
    }


    uint32_t colors_i = 0;
    pixel_t colors[size];

    for (uint32_t i = 0; i < pointCount; i++) {
        pixel_t *pixel = getPix(img,
            points[i].x - medianOffset/2,
            points[i].y - medianOffset/2
        );


        // Check if color already found.
        uint8_t colorFound = 0;
        for (uint32_t c = 0; c < colors_i; c++) {
            if (memcmp(colors + c, pixel, sizeof(pixel_t)) == 0) {
                colorFound = 1;
            }
        }
        if (colorFound) continue;


        if (colors_i >= size) {
            printf("FUCK there are more colors than queens.\n");
            break;
        }
        colors[colors_i++] = *pixel;
        pixel->r = 255;
    }

    if (colors_i != size) {
        printf("There are too few colors!!!\n");
    }


    for (uint32_t i = 0; i < colors_i; i++) {
        printf("Color %d: #%02x%02x%02x  #%02x%02x%02x  #%02x%02x%02x\n", i,
            colors[i].r, colors[i].g, colors[i].b,
            colors[i].b, colors[i].g, colors[i].r,
            colors[i].g, colors[i].r, colors[i].b
        );
    }


    free(points);
}

uint32_t getPoints(image_t img, coord_t **points, int pixelOffset) {
    const uint32_t offset = 20;

    coord_t *coords = malloc(32 * sizeof(coord_t));
    uint32_t coords_n = 32;
    uint32_t coords_i = 0;

    for (uint32_t y = offset; y < img.height - offset; y++) {
        for (uint32_t x = offset; x < img.width - offset; x++) {

            pixel_t *pix = getPix(img, x, y);

            // Did we find a discrepancy?
            if (isCrossing(img, pix, pixelOffset)) {
                if (coords_i == coords_n) {
                    coords_n += 32;
                    coords = realloc(coords, coords_n * sizeof(coord_t));
                }
                coords[coords_i] = (coord_t){x, y};
                coords_i++;
            }
            else if (pix->g == 255) pix->g = 254;
        }
    }

    if (coords_i == 0) {
        free(coords);
        *points = NULL;
        return 0;
    }

    coords = realloc(coords, coords_i * sizeof(coord_t));
    *points = coords;

    return coords_i;
}


// Finds the crossing and also fucks up the green channel of the image.
uint8_t isCrossing(image_t img, pixel_t *pix, int pixelOffset) {
    if (
           (pix - 1)->g == 255
        || (pix - img.width)->g == 255
        || (pix - 1 - img.width)->g == 255
    ) {
        if (pix->g == 255) pix->g = 254;
        return 0;
    }

    uint8_t conv[3][3] = {
        {1, 0, 1},
        {0, 0, 0},
        {1, 0, 1}
    };

    // Loop until we find a discrepancy
    for (int cy = -1; cy <= 1; cy++) {
        for (int cx = -1; cx <= 1; cx++) {

            int32_t offset = pixelOffset * cx + (pixelOffset * cy * img.width);
            pixel_t *pixPtr = pix + offset;

            pixel_t testPix = *pixPtr;

            if (!!sum(testPix) != conv[cy + 1][cx + 1]) {
                return 0;
            }

        }
    }

    pix->g = 255;
    return 1;
}


static inline uint16_t sum(pixel_t pixel) {
    return pixel.r + pixel.g + pixel.b;
}


static inline pixel_t* getPix(image_t img, uint32_t x, uint32_t y) {
    return img.pixels + (x + y * img.width);
}
