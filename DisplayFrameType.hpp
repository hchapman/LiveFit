#ifndef DISPLAYFRAMETYPE
#define DISPLAYFRAMETYPE

/**
 * @brief The DisplayFrameType enum
 * Types of video that the TrackingStream stack can emit;
 * basically options for a select box
 */
enum DisplayFrameType {
    DF_VIDEO, DF_THRESH, DF_BLUR, DF_CONTOUR
};

#endif // DISPLAYFRAMETYPE

