//
// Created by kyle on 3/04/19.
//

#ifndef EGH400_1_2_SOLUTIONONEOTSU_H
#define EGH400_1_2_SOLUTIONONEOTSU_H

#include <stdlib.h>
#include <string>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

class OTSU
        {

private:

            cv::Mat grayImage;
            int kernalWidth = 1;
            int kernalHeight = 1;

public:
            OTSU( cv::Mat& image, cv::Mat& RGB , std::string solutionType = "nonlinear" );

            void performOTSUThreshold( cv::Mat& image, cv::Mat &RGB, std::string solutionType  );

                double confidenceEstimate(std::vector<cv::Point> points, cv::Mat &img );
        };
#endif //EGH400_1_2_SOLUTIONONEOTSU_H
