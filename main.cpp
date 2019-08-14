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

   double total_time;

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

        //RGB 2 GRAY
        cvtColor(image, im_Gray, CV_BGR2GRAY);

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

        HorizonLineDetector hld;

        const bool is_run=true;
        const std::string file_path_in=image_url;
        const std::string file_path_out=pred_url + std::to_string(i) + std::string("_pred.png");

        if (is_run)
        {
            std::string training_file="/home/kyle/Desktop/EGH400_1&2/code/Dynamic_Programming/Training/saved_model.svm";
            std::string file_path_mask="";

            cv::Mat mask;
            if (!file_path_mask.empty())
                mask=cv::imread(file_path_mask);
            if (!mask.empty())
                std::cout<<"Using mask"<<std::endl;
            hld.set_canny_param(40);
            hld.set_max_search_steps(10);
            hld.init_detector(training_file);
            hld.detect_video(image,file_path_out,mask);
        }
        //auto total_time+=
//////timini
//
//        std::string mask_pred_url = pred_url + std::to_string(i) + std::string("_pred_mask.png");
//        std::string hld_pred_url = pred_url + std::to_string(i) + std::string("_pred.png");
////        std::string mag_url = pred_url + std::to_string(i) + std::string("_mag.png");
////        // Writing the binary mask
//        imwrite(mask_pred_url, im_Gray);
////        // Writing the image HL
//        imwrite(hld_pred_url, image);


    }


    //auto diff = end - start;

//    std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

//    imshow("OTSU Image", im_Gray);
//    imshow( "BGR Image", image );


    waitKey(0);

    return 0;
}