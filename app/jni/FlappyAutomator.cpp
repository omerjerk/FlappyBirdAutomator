#include <jni.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

using namespace cv;

class ContentFinder {
    private:
    	int histSize[2];
        const float* ranges[2];
        int channels[2];
        float hranges[2];
        float sranges[2];
    public:
    	ContentFinder() {
            // Prepare arguments for a color histogram
            histSize[0] = 180;
            histSize[1] = 256;
            hranges[0] = 0; hranges[1] = 180;
            sranges[0] = 0; sranges[1] = 256;
            ranges[0] = hranges;
            ranges[1] = sranges;
            channels[0] = 0; // the three channels
            channels[1] = 1;
        }

        MatND getHistogram(const Mat& image) {
        	MatND hist;
        	calcHist(&image, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
        	return hist;
        }

        Mat backProject (const Mat& image, const MatND& histogram) {
        	Mat result;
        	normalize(histogram, histogram, 0, 255, NORM_MINMAX, -1, Mat());
        	calcBackProject(&image, 1, channels, histogram, result, ranges, 1);
        	threshold(result, result, 5, 255, 0);
        	return result;
        }

        void getMeanShift(Mat& result, Rect& track_window) {
        	TermCriteria criteria(TermCriteria::MAX_ITER, 10, 0.01);
        	meanShift(result, track_window, criteria);
        }

        int findHeadCoord (Mat& result) {
        	uchar *data;
        	int rows = result.rows;
        	int elemSize = result.elemSize();
        	for (int i = 0; i < rows; ++i) {
        		data = result.data + i*result.step + 88*elemSize;
        		if (*data == 255) {
        			return i;
        		}
        	}

        	return 0;
        }
};

JNIEXPORT jlong JNICALL
Java_in_omerjerk_flappybirdautomator_nativeCreateObject
(JNIEnv * env, jobject  obj) {
    jlong result = (jlong) new ContentFinder();
    return result;
}

JNIEXPORT void JNICALL
Java_in_omerjerk_flappybirdautomator_nativeDestroyObject
(JNIEnv * env, jobject  obj, jlong thiz) {
    if (thiz != 0) {
        delete (ContentFinder*) thiz;
    }
}