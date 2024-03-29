//
// Created by kyle on 15/05/19.
//

#ifndef NEW400FINAL_SOLUTIONTHREEBCV_H
#define NEW400FINAL_SOLUTIONTHREEBCV_H
#include <stdlib.h>
#include <string>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


class BetweenClassVariance
{

private:

    int M = 23;
    int N = 50;
    int K1 = 32;
    int K2 = 32;
    int searchRadius = 70;
    int computationalRadius = 8;

public:

    BetweenClassVariance( cv::Mat& image, cv::Mat& RGB );

    std::vector<double> coarseHLDetection( cv::Mat& image );

    void fineHLDetection( cv::Mat& image, cv::Mat& RGB, std::vector<double> rho_theta );

};
#endif //NEW400FINAL_SOLUTIONTHREEBCV_H
