#pragma once
#include <opencv2/opencv.hpp>

class ABUSAlg
{
public:
	ABUSAlg(int paddingLen);
	~ABUSAlg();
	void DSC2D(cv::Mat &dsc_image, cv::Mat org_image, int method);
	void ReDSC2D(cv::Mat& re_dsc_image, cv::Mat &dsc_image, int method);
private:
	double m_alpha;
	int m_radius;
	int m_outputHeight;
	int m_outputWidth;
	int m_paddingLen;
};

