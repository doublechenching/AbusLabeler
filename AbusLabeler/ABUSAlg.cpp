#include "stdafx.h"
#include "ABUSAlg.h"
#include <math.h>

static const double PI = 3.1415928;

ABUSAlg::ABUSAlg(int paddingLen)
{
	m_paddingLen = paddingLen;
	m_alpha = 20;
	m_radius = 2500;
}

void ABUSAlg::DSC2D(cv::Mat &dsc_image, cv::Mat org_image, int method)
{
	// padding array
	cv::Mat padedImage(cv::Size(org_image.cols, org_image.rows + m_paddingLen), CV_32F);
	int height = padedImage.rows;
	int width  = padedImage.cols;
	double delta_alpha = m_alpha / (width - 1) * PI / 180;
	double angle = 0;
	cv::Mat mapX(padedImage.size(), CV_32F);
	cv::Mat mapY(padedImage.size(), CV_32F);
	double center_x = width / 2;
	double center_y = -m_radius;
	double radius = 0;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			angle = (i - center_x) * delta_alpha;
			radius = center_y - j;
			mapX.at<float>(j, i) = center_x - radius * sin(angle);  
			mapY.at<float>(j, i) = center_y - radius * cos(angle);
		}
	}

	cv::remap(org_image, dsc_image, mapX, mapY, method, cv::BORDER_CONSTANT, cv::Scalar(0));
	
}


void ABUSAlg::ReDSC2D(cv::Mat& re_dsc_image, cv::Mat &dsc_image, int method)
{
	int height = dsc_image.rows;
	int width = dsc_image.cols;
	cv::Mat mapX(dsc_image.size(), CV_32F);
	cv::Mat mapY(dsc_image.size(), CV_32F);
	double delta_alpha = m_alpha / (width - 1) * PI / 180;
	double angle = 0;
	double center_x = width / 2;
	double center_y = -m_radius;
	double radius = 0;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			radius = sqrt(pow(i - center_x, 2) + pow(j - center_y, 2));
			mapX.at<float>(j, i) = atan2((i - center_x), (j - center_y)) / delta_alpha + center_x;
			mapY.at<float>(j, i) = radius - m_radius;
		}
	}
	cv::remap(dsc_image, re_dsc_image, mapX, mapY, method, cv::BORDER_CONSTANT, cv::Scalar(0));
	cv::Mat roi = re_dsc_image(cv::Rect(0, 0, re_dsc_image.cols, re_dsc_image.rows - m_paddingLen));
	re_dsc_image = roi;
}

ABUSAlg::~ABUSAlg()
{
}
