//
// Created by kyle on 11/04/19.
//

#ifndef EGH400_1_2_SOLUTIONTWOMAXIMISEBUSYNESS_H
#define EGH400_1_2_SOLUTIONTWOMAXIMISEBUSYNESS_H

#include <stdlib.h>
#include <string>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

class MaximiseBusyness
{

private:

    cv::Mat grayImage;
    int kernalWidth = 15;
    int kernalHeight = 15;
    cv::Mat magIMG;


public:


    MaximiseBusyness( cv::Mat& image, cv::Mat& RGB );

    std::vector<cv::Point2i> tempHL( int t, cv::Mat &Gmag);

    void filterImage( cv::Mat& img );

    void fineHL( cv::Mat &HL, cv::Mat &img );

    void prelimHL(cv::Mat &image, cv::Mat &RGB );

    void setMagImg( cv::Mat &img );

    cv::Mat getMagImg();

    double confidenceEstimate( std::vector<cv::Point> points, cv::Mat &img );
};
#endif //EGH400_1_2_SOLUTIONTWOMAXIMISEBUSYNESS_H
