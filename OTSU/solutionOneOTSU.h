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
    int searchRadius = 50;
    int computationalRadius = 5;
    double confidence = 0;
    std::vector<cv::Point> HL_points;

public:


    OTSU();

    OTSU( cv::Mat& image, cv::Mat& RGB , std::string solutionType = "nonlinear" );

    void performOTSUThreshold( cv::Mat& image, cv::Mat &RGB, std::string solutionType  );

    double confidenceEstimate(std::vector<cv::Point> points, cv::Mat &img );

    void setConfidence( double conf );

    double getConfidence( );

    double getCostOTSU( int x, int y);

    void setHLPoints( std::vector<cv::Point> points );

    void clearHLPoints( );

};
#endif //EGH400_1_2_SOLUTIONONEOTSU_H