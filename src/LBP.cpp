#include "LBP.hpp"

const int powers[3][3] = {
        { 128, 64, 32 },
        { 1, 0, 16 },
        { 2, 4, 8 }
};

void treatPixel(const Mat_<uchar>& src, Mat_<uchar>& dest, const uint x, const uint y, const int *neigh, const size_t neighSize) {
    uint value = 0;

    for (uint i = 0; i < neighSize; i++) {
        const auto w = *(neigh++);
        const auto z = *(neigh++);
        const auto a = x + w;
        const auto b = y + z;

        if ((int)src(b, a) <= (int)src(y, x)) {
            value += powers[1 + z][1 + w];
        }
    }

    dest(y, x) = value;
};

void LBP(const Mat_<uchar>& src, Mat_<uchar>& dest) {
    const uint maxW = src.cols - 1;
    const uint maxH = src.rows - 1;

    // following code is decomposed to avoid the if condition
    // on every pixels for better performances reasons

    const int topLeftNeigh[3][2] = { { 1, 0 }, { 0, 1 }, { 1, 1 } };
    const int topRightNeigh[3][2] = { { -1, 0 }, { -1, 1 }, { 0, 1 } };
    const int botRightNeigh[3][2] = { { -1, -1 }, { 0, -1 }, { -1, 0 } };
    const int botLeftNeigh[3][2] = { { 0, -1 }, { 1, -1 }, { 1, 0 } };

    const int topEdgeNeigh[5][2] = { { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
    const int botEdgeNeigh[5][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 } };
    const int leftEdgeNeigh[5][2] = { { 0, -1 }, { 1, -1 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    const int rightEdgeNeigh[5][2] = { { -1, -1 }, { 0, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
    const int coreNeigh[8][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

    dest = Mat(src.rows, src.cols, CV_8UC1, Scalar(0, 0, 0));

    // 4 corners
    treatPixel(src, dest, 0, 0, &(topLeftNeigh[0][0]), 3);	        // Top-Left
    treatPixel(src, dest, maxW, 0, &(topRightNeigh[0][0]), 3);	    // Top-Right
    treatPixel(src, dest, maxW, maxH, &(botRightNeigh[0][0]), 3);	// Bottom-Right
    treatPixel(src, dest, 0, maxH, &(botLeftNeigh[0][0]), 3);	    // Bottom-Left

    // 4 edges
    // horizontal
    for (uint i = 1; i < maxW; i++) {
        treatPixel(src, dest, i, 0, &(topEdgeNeigh[0][0]), 5);
        treatPixel(src, dest, i, maxH, &(botEdgeNeigh[0][0]), 5);
    }

    // vertical
    for (uint i = 1; i < maxH; i++) {
        treatPixel(src, dest, 0, i, &(leftEdgeNeigh[0][0]), 5);
        treatPixel(src, dest, maxW, i, &(rightEdgeNeigh[0][0]), 5);
    }

    // core
    for (uint y = 1; y < maxH; y++) {
        for (uint x = 1; x < maxW; x++) {
            treatPixel(src, dest, x, y, &(coreNeigh[0][0]), 8);
        }
    }
}