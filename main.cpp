#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "OTSU/solutionOneOTSU.h"
#include "Maximise_Busyness/solutionTwoMaximiseBusyness.h"
#include "BCV/solutionThreeBCV.h"
#include "Dynamic_Programming/DP.h"

using namespace cv;


int main(int argc, char** argv )
{
    //Processing Mats
    Mat image, im_Gray;
    int k = 20;

    HorizonLineDetector hld;
    auto total_time = 0;

    for (int i = 1 ; i < k+1; i++) {

        std::string image_url;

        std::string pred_url = "/home/kyle/Desktop/EGH400_1&2/datasets/PEXEL/ALL_IMAGES/Predictions/DP/play/";
        image_url  =  "/home/kyle/Desktop/EGH400_1&2/datasets/PEXEL/ALL_IMAGES/resized_/training/img_";
         image_url = image_url + std::to_string(i) + std::string(".png");

        image = imread(image_url, 1);

        if (!image.data) {
            printf("No image data \n");
            return -1;
        }

        cv::resize(image, image, cv::Size(image.cols * 0.2,image.rows * 0.2), 0, 0, CV_INTER_LINEAR);

        //RGB 2 GRAY
        //cvtColor(image, im_Gray, CV_BGR2GRAY);

        /**
         * SOLUTION ONE OTSU
         */
//        OTSU otsu( im_Gray, image, "linear" );
        /**
        * SOLUTION TWO MAXIMISE BUSYNESS
        */
//        MaximiseBusyness MB( im_Gray, image );
//        cv::Mat mag_img = MB.getMagImg();
        /**
        * SOLUTION THREE BCV
        */
//        auto start = std::chrono::steady_clock::now();
//        BetweenClassVariance BCV( im_Gray, image );
//        auto end = std::chrono::steady_clock::now();
        /**
         * SOLUTION FOUR DP
        */
        const bool is_run=true;
        const std::string file_path_in=image_url;
        const std::string file_path_out=pred_url + std::to_string(i) + std::string("_pred.png");
        const std::string file_path_out_edge=pred_url + std::to_string(i) + std::string("_pred_edge.png");
        const std::string file_path_out_mask=pred_url + std::to_string(i) + std::string("_pred_mask.png");
        auto start = std::chrono::steady_clock::now();
        std::cout << "num= " << i << std::endl;
        if (is_run)
        {

            hld.set_canny_param(25);
            hld.set_max_search_steps(3);
            hld.DynamicProgramming(image,file_path_out,file_path_out_edge,file_path_out_mask);
        }
        auto end = std::chrono::steady_clock::now();

//////timini
//
//        std::string mask_pred_url = pred_url + std::to_string(i) + std::string("_pred_mask.png");
//        std::string hld_pred_url = pred_url + std::to_string(i) + std::string("_pred.png");
////        std::string mag_url = pred_url + std::to_string(i) + std::string("_mag.png");
////        // Writing the binary mask
//        imwrite(mask_pred_url, im_Gray);
////        // Writing the image HL
//        imwrite(hld_pred_url, image);
        auto diff = end - start;
        //std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
        total_time = std::chrono::duration <double, std::milli> (diff).count() + total_time;
    }

    std::cout << 1000/(std::chrono::duration <double, std::milli> (total_time).count()/20) << " Hz" << std::endl;
    std::cout << std::chrono::duration <double, std::milli> (total_time).count() << " ms" << std::endl;



//    imshow("OTSU Image", im_Gray);
//    imshow( "BGR Image", image );


    waitKey(0);

    return 0;
}