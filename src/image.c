#include "../include/image.h"

static uint16_t read_u16(FILE *f, int *ok)
{
    unsigned char b[2];

    if (fread(b, 1, sizeof(b), f) != sizeof(b))
    {
        *ok = 0;
        return 0;
    }

    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}

static uint32_t read_u32(FILE *f, int *ok)
{
    unsigned char b[4];

    if (fread(b, 1, sizeof(b), f) != sizeof(b))
    {
        *ok = 0;
        return 0;
    }

    return (uint32_t)b[0]
         | ((uint32_t)b[1] << 8)
         | ((uint32_t)b[2] << 16)
         | ((uint32_t)b[3] << 24);
}

static int32_t read_i32(FILE *f, int *ok)
{
    return (int32_t)read_u32(f, ok);
}

static int check_size(size_t a, size_t b, size_t *result)
{
    if (a != 0 && b > SIZE_MAX / a)
        return 0;

    *result = a * b;
    return 1;
}

int load_bmp_image_double(const char *path,
                          double *destination,
                          size_t width,
                          size_t height)
{
    if (!path || !destination || width == 0 || height == 0)
    {
        printf("error : load_bmp_image_double :: init error -> returned 0\n");
        return 0;
    }

    FILE *f = fopen(path, "rb");
    if (!f)
    {
        printf("error : load_bmp_image_double :: fopen error 'file can't be opened' -> returned 0\n");
        return 0;
    }

    int ok = 1;

    /* BITMAPFILEHEADER */
    const uint16_t signature = read_u16(f, &ok);
    (void)read_u32(f, &ok); /* file size */
    (void)read_u16(f, &ok); /* reserved */
    (void)read_u16(f, &ok); /* reserved */
    const uint32_t pixels_offset = read_u32(f, &ok);

    if (!ok || signature != 0x4D42) /* "BM" */
    {
        fclose(f);
        printf("error : load_bmp_image_double :: image read error -> returned 0\n");
        return 0;
    }

    /* DIB header: expects at least a BITMAPINFOHEADER (40 bytes). */
    const uint32_t header_size = read_u32(f, &ok);
    if (!ok || header_size < 40)
    {
        fclose(f);
        printf("error : load_bmp_image_double :: size error -> returned 0\n");
        return 0;
    }

    const int32_t source_width_i = read_i32(f, &ok);
    const int32_t source_height_i = read_i32(f, &ok);
    const uint16_t planes = read_u16(f, &ok);
    const uint16_t bits_per_pixel = read_u16(f, &ok);
    const uint32_t compression = read_u32(f, &ok);

    (void)read_u32(f, &ok); /* image size */
    (void)read_i32(f, &ok); /* X resolution */
    (void)read_i32(f, &ok); /* Y resolution */
    (void)read_u32(f, &ok); /* colors */
    (void)read_u32(f, &ok); /* important colors */
    //printf("%d\n", compression);

    if (!ok || planes != 1 || source_width_i <= 0 || source_height_i == 0 ||
        (bits_per_pixel != 24 && bits_per_pixel != 32) || compression != 0)
    {
        fclose(f);
        printf("error : load_bmp_image_double :: parsing image error -> returned 0\n");
        return 0;
    }

    const size_t source_width = (size_t)source_width_i;
    const size_t source_height =
        (size_t)(source_height_i < 0 ? -source_height_i : source_height_i);

    /* BMP alignment: each row takes a multiple of 4 bytes. */
    const size_t bytes_per_pixel = bits_per_pixel / 8;
    size_t raw_row;

    if (!check_size(source_width, bytes_per_pixel, &raw_row))
    {
        fclose(f);
        printf("error : load_bmp_image_double :: size_verification_after_call(0) error -> returned 0\n");
        return 0;
    }

    const size_t row = (raw_row + 3u) & ~((size_t)3u);

    size_t pixels_size;
    if (!check_size(row, source_height, &pixels_size))
    {
        fclose(f);
        printf("error : load_bmp_image_double :: size_verification_after_call(1) error -> returned 0\n");
        return 0;
    }

    unsigned char *pixels = malloc(pixels_size);
    if (!pixels)
    {
        fclose(f);
        printf("error : load_bmp_image_double :: pixels error -> returned 0\n");
        return 0;
    }

    if (fseek(f, (long)pixels_offset, SEEK_SET) != 0 ||
        fread(pixels, 1, pixels_size, f) != pixels_size)
    {
        free(pixels);
        fclose(f);
        printf("error : load_bmp_image_double :: fread / fseek error -> returned 0\n");
        return 0;
    }

    fclose(f);

    /*
     * The BMP can be bottom-up (positive height) or top-down (negative).
     * We resize directly into destination, without an intermediate buffer.
     *
     * Resizing by MIN-POOLING: for each destination pixel, we look at the
     * whole matching block of source pixels and keep the darkest one.
     * Unlike nearest-neighbor (which skips pixels and can make a thin
     * stroke disappear depending on alignment), this guarantees that a
     * dark detail (grid line, thin stroke) in the source block always
     * shows up in the destination pixel.
     */
    for (size_t y = 0; y < height; ++y)
    {
        /* range of source rows covered by this destination row */
        size_t sy_start = (y * source_height) / height;
        size_t sy_end = ((y + 1) * source_height) / height;
        if (sy_end <= sy_start) sy_end = sy_start + 1;
        if (sy_end > source_height) sy_end = source_height;

        for (size_t x = 0; x < width; ++x)
        {
            /* range of source columns covered by this destination pixel */
            size_t sx_start = (x * source_width) / width;
            size_t sx_end = ((x + 1) * source_width) / width;
            if (sx_end <= sx_start) sx_end = sx_start + 1;
            if (sx_end > source_width) sx_end = source_width;

            double min_gray = 255.0;

            for (size_t sy = sy_start; sy < sy_end; ++sy)
            {
                const size_t source_row =
                    (source_height_i > 0) ? (source_height - 1 - sy) : sy;

                const unsigned char *row_ptr = pixels + source_row * row;

                for (size_t sx = sx_start; sx < sx_end; ++sx)
                {
                    const unsigned char *pixel = row_ptr + sx * bytes_per_pixel;

                    /* BMP stores BGR(A). */
                    const double b = (double)pixel[0];
                    const double g = (double)pixel[1];
                    const double r = (double)pixel[2];

                    /* Approximate standard luminance conversion. */
                    const double gray = 0.114 * b + 0.587 * g + 0.299 * r;

                    if (gray < min_gray)
                        min_gray = gray;
                }
            }

            destination[y * width + x] = min_gray / 255.0;
        }
    }

    free(pixels);
    return 1;
}