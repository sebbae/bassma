/*
 * VideoSource.h
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#ifndef VIDEOSOURCE_H_
#define VIDEOSOURCE_H_

namespace cv {
class Mat;
} /* namespace cv */

namespace bassma {

class VideoSource {
public:
	VideoSource() {};
	virtual ~VideoSource() {};

	VideoSource(const VideoSource& src) = delete;
	VideoSource& operator=(const VideoSource& src) = delete;

	virtual cv::Mat captureFrame() = 0;
};

} /* namespace bassma */

#endif /* VIDEOSOURCE_H_ */
