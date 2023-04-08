#include <iostream>
#include "decodevideo.hpp"
#include "fastpixelmap.hpp"

/*
*   Workshop 3
*   by Kai Pendleton
*
*   Problem: Mapping pixels in a source image to the closest color available within a predefined palette
*   Goal: An algorithm that can quickly convert RGB images to pal8
*
*/

using namespace std;

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Color palette by John A. Watlington at alumni.media.mit.edu/~wad/color/palette.html
Color colorValues[16] = {
                                //Black
                                {0, 0, 0},
                                //Dark Gray
                                {87, 87, 87},
                                //Red
                                {173, 35, 35},
                                //Blue
                                {42, 75, 215},
                                //Green
                                {29, 105, 20},
                                //Brown
                                {129, 74, 25},
                                //Purple
                                {129, 38, 192},
                                //Light Gray
                                {160, 160, 160},
                                //Light Green
                                {129, 197, 122},
                                //Light Blue
                                {157, 175, 255},
                                //Cyan
                                {41, 208, 208},
                                //Orange
                                {255, 146, 51},
                                //Yellow
                                {255, 238, 51},
                                //Tan
                                {233, 222, 187},
                                //Pink
                                {255, 205, 243},
                                //White
                                {255, 255, 255}
};

BGRAPixel expandedPalette[256];

void initializeExpandedColors() {

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            uint8_t red, green, blue;
            red = (int)(0.33 * colorValues[i].red + 0.67 * colorValues[j].red);
            green = (int)(0.33 * colorValues[i].green + 0.67 * colorValues[j].green);
            blue = (int)(0.33 * colorValues[i].blue + 0.67 * colorValues[j].blue);
            expandedPalette[16 * i + j] = {blue, green, red};
        }
    }
    return;
}


int main()
{
    BGRAPixel palette[16];
    for (int i = 0; i < 16; i++) {
        palette[i].blue = colorValues[i].blue;
        palette[i].green = colorValues[i].green;
        palette[i].red = colorValues[i].red;
    }
    initializeExpandedColors();
    //writePPM("palette", 4, 4, (uint8_t*) palette, false);
    //writePPM("expandedPalette", 16, 16, (uint8_t*) expandedPalette, false);

    int width = 320;
    int height = 240;

    VideoDecoder decoder(width, height, "RickRoll.mkv");
    //decoder.printVideoInfo();
    decoder.seekFrame(0);
    uint8_t* image;
    FastPixelMap pixelMapper((uint8_t*)expandedPalette, 256);
    for (int i = 0; i < 1000; i++) {
        image = decoder.readFrame();

        //cout << i << endl;

        uint8_t *pal8Image;
        if ( i == 800 ) {

            writePPM("test.ppm", width, height, image, true);

            pal8Image = pixelMapper.convertImage(image, width, height, true);
            writePal8PPM("paletteTest.ppm", width, height, pal8Image, (uint8_t*) expandedPalette);
            delete[] pal8Image;

//            pal8Image = pixelMapper.fullSearchConvertImage(image, width, height, true);
//            writePal8PPM("fsPaletteTest.ppm", width, height, pal8Image, (uint8_t*) expandedPalette);
//            delete[] pal8Image;

        }

    }

    return 0;
}

/*
    Results:


    500 frames of 320x240 mp4 video

    Algorithm                                                      Time(s)         Time per Frame(s)
    Full-Search (naive)                                             68.744          .137
    MPS + No PDS                                                    5.980           .01196
    MPS + Partial PDS (Green+Blue, check, then red)                 5.996           .01199
    MPS + Full PDS                                                  5.926           .01185
    MPS + Full PDS + TIE                                            5.927           .01185
    No Pixel Mapping (Just decoding/scaling video with FFMPEG)      .643            .00129
    Performance Gain compared to Full-Search (Decoding/scaling time removed):       12.888x



    500 frames of 320x240 mkv video (Never Gonna Give You Up by Rick Astley)

    Algorithm                                                      Time(s)         Time per Frame(s)
    Full-Search (naive)                                            73.322          .1466
    MPS + Full PDS + TIE                                           11.089          .0222
    No Pixel Mapping (Just decoding/scaling video with FFMPEG)     3.200           .0064
    Performance Gain compared to Full Search (Decoding/scaling time removed):      8.889x


    Authors' reported speed-up: ~20x depending on image

    Possible Reasons for difference in speed: My best guess is that the additional operations used
    for calculating the index of the palette colors and image pixels significantly increases the overhead of my
    algorithm. It may be possible to rewrite the algorithm to continually track the offset of the image pointer
    and palette color without additional operations. This could potentially save multiple operations per pixel
    and per color checked.
    Another possible cause of the slowdown is the overhead of the class structure that I used. The authors'
    used C for their implementation, so they presumably had very little overhead.


    Sources:

    - Main source:
    Hu, Yu-Chen & Su, B.-H. (2008). Accelerated pixel mapping scheme for colour image quantisation.
    Imaging Science Journal, The. 56. 68-78. 10.1179/174313107X214231.

    - Original Paper describing the MPS algorithm
    Ra, & Kim, J.-K. (1993). A fast mean-distance-ordered partial codebook search algorithm for image vector quantization.
    IEEE Transactions on Circuits and Systems. 2, Analog and Digital Signal Processing, 40(9), 576–579.
    https://doi.org/10.1109/82.257335


    - Related Paper describing the trade-offs of the partial distance search technique
    L. Fissore, P. Laface, P. Massafra and F. Ravera, "Analysis and improvement of the partial distance search algorithm,"
    1993 IEEE International Conference on Acoustics, Speech, and Signal Processing, Minneapolis, MN, USA, 1993, pp. 315-318
    vol.2, doi: 10.1109/ICASSP.1993.319300.

*/




