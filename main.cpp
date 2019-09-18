#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgcodecs.hpp>

#include "OTSU/solutionOneOTSU.h"
#include "Maximise_Busyness/solutionTwoMaximiseBusyness.h"
#include "BCV/solutionThreeBCV.h"
#include "Dynamic_Programming/DP.h"
#include "SOL5/sol5.h"



int main(int argc, char** argv )
{
    //Processing Mats
    cv::Mat image, im_Gray;

    int k = 250;
    int SOL  = 5;

    int p = 9604;
    HorizonLineDetector hld;
    HorizonLineDetectorFive Sol_5;


    auto total_time = 0;
    bool run_all = false;
    for (int i = p ; i < k +p; i++) {

        std::string image_url;

        if ( run_all ){

            switch ( i ){
                case 1: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/1/9618.ppm";
                    break;
                case 2: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/5/48010.ppm";
                    break;
                case 3: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/1/17690.ppm";
                    break;
                case 4: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/3/29172.ppm";
                    break;
                case 5: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/1/9606.ppm";
                    break;
                case 6: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/3/28808.ppm";
                    break;
                case 7: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/4/38411.ppm";
                    break;
                case 8: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/1/9604.ppm";
                    break;
                case 9: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/2/19215.ppm";
                    break;
                case 10: image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-20_27_42/camera_images/1/9607.ppm";
                    break;
            }
        }

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/1/9618.ppm";
//          image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/1/";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/5/48010.ppm";
//          image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20181023-02_24_33/camera_images/5/";

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/1/17690.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/1/";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/3/29172.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20170928-02_24_46/camera_images/3/";

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/ecod2018/aslhost-20180223-07_52_44.3ohyf/camera_images/devices/GIGE7/8/76804.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/ecod2018/aslhost-20180223-07_52_44.3ohyf/camera_images/devices/GIGE7/8/";

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/1/9606.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/1/";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/3/28808.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/3/";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/4/38411.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20180906-02_08_24/camera_images/4/";

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/1/9604.ppm";
        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/1/";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/2/19215.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-19_13_47/camera_images/2/";

//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-20_27_42/camera_images/1/9607.ppm";
//        image_url  =  "/home/kyle/mnt/daa/detectdata/datasets/aslhost-20171205-20_27_42/camera_images/1/";



        image_url = image_url + std::to_string(i) + std::string(".ppm");

        image = cv::imread(image_url, 1);

        if (!image.data) {
            printf("No image data \n");
            return -1;
        }

        // Ensure image is correct size before downscaling
        cv::resize(image, image, cv::Size(1024, 768), 0, 0);


        //RGB 2 GRAY
        cv::cvtColor(image, im_Gray, cv::COLOR_BGR2GRAY);
        cv::Mat otsu_test_img = image.clone();
        cv::Mat DP_test_img = image.clone();
        cv::Mat MB_test_img = image.clone();
        cv::Mat BVC_test_img = image.clone();
        cv::Mat SOL5_test_img = image.clone();


            switch (SOL) {

                case 1: {
                    /**
                     * SOLUTION ONE OTSU
                     */
                    auto start_otsu = std::chrono::steady_clock::now();
                    std::string pred_url_otsu = "/home/kyle/Desktop/HLD/predictions/OTSU/play/img_otsu";
                    std::string hld_pred_url = pred_url_otsu + std::to_string(i) + std::string("_pred.png");

                    OTSU otsu(im_Gray, otsu_test_img, "linear");

                    cv::imwrite(hld_pred_url, otsu_test_img);
//                    cv::imshow("pred", image);
//                    cv::waitKey(0);
                    auto end_otsu = std::chrono::steady_clock::now();
                    auto diff_otsu = end_otsu - start_otsu;

                    std::cout << std::chrono::duration<double, std::milli>(diff_otsu).count() << " ms" << std::endl;

                    break;
                }
                case 2: {
                    /**
                   * SOLUTION TWO MAXIMISE BUSYNESS
                     */

                    std::string pred_url_MB = "/home/kyle/Desktop/HLD/predictions/MB/play/img_";
                    std::string hld_pred_url = pred_url_MB + std::to_string(i) + std::string("_pred.png");
                    auto start_MB= std::chrono::steady_clock::now();
                    MaximiseBusyness MB(im_Gray, MB_test_img);
                    cv::Mat mag_img = MB.getMagImg();
                    auto end_MB = std::chrono::steady_clock::now();
                    auto diff_MB = end_MB - start_MB;

                    total_time = std::chrono::duration<double, std::milli>(diff_MB).count() + total_time;
                    cv::imwrite(hld_pred_url, MB_test_img);
                    std::cout << std::chrono::duration<double, std::milli>(diff_MB).count() << " ms" << std::endl;
                    break;
                }
                case 3: {
                    /**
                    * SOLUTION THREE BCV
                    */

                    std::string pred_url_BVC = "/home/kyle/Desktop/HLD/predictions/BVC/play/img_";
                    std::string hld_pred_url = pred_url_BVC + std::to_string(i) + std::string("_pred.png");
                    auto start_BVC= std::chrono::steady_clock::now();
                    cv::Rect region_of_interest = cv::Rect(0, 400, im_Gray.cols - 1, (im_Gray.rows - 1) - 400);
                    cv::Mat image_roi = im_Gray(region_of_interest);
                    cv::Mat image_roi_test = BVC_test_img(region_of_interest);
                    BetweenClassVariance BCV(image_roi, image_roi_test);

                    auto end_BVC = std::chrono::steady_clock::now();
                    auto diff_BVC = end_BVC - start_BVC;
                    total_time = std::chrono::duration<double, std::milli>(diff_BVC).count() + total_time;

                    cv::imwrite(hld_pred_url, image_roi_test);
                    break;
                }
                case 4: {
                    /**
                     * SOLUTION FOUR DP
                     */
                    cv::resize(DP_test_img, DP_test_img, cv::Size(image.cols * 0.2, image.rows * 0.2), 0, 0,
                               cv::INTER_LINEAR);

                    const bool is_run = true;
                    const std::string file_path_in = image_url;
                    std::string pred_url = "/home/kyle/Desktop/HLD/predictions/DP/play/";
                    const std::string file_path_out = pred_url + std::to_string(i) + std::string("_pred.png");
                    const std::string file_path_out_edge = pred_url + std::to_string(i) + std::string("_pred_edge.png");
                    const std::string file_path_out_mask = pred_url + std::to_string(i) + std::string("_pred_mask.png");
                    auto start = std::chrono::steady_clock::now();
                    std::cout << "num= " << i << std::endl;
                    if (is_run) {
                        hld.set_canny_param(12);
                        hld.set_max_search_steps(4);
                        hld.DynamicProgramming(DP_test_img, file_path_out, file_path_out_edge, file_path_out_mask);
                    }
                    auto end = std::chrono::steady_clock::now();
                    auto diff = end - start;
                    total_time = std::chrono::duration<double, std::milli>(diff).count() + total_time;
                    break;
                }
                case 5: {



                    /**
                     * SOLUTION ONE OTSU
                     */
                    cv::resize(im_Gray, im_Gray, cv::Size(image.cols * 0.2, image.rows * 0.2), 0, 0,
                               cv::INTER_LINEAR);
                    cv::resize(otsu_test_img, otsu_test_img, cv::Size(image.cols * 0.2, image.rows * 0.2), 0, 0,
                               cv::INTER_LINEAR);
                    auto start_otsu = std::chrono::steady_clock::now();
                    std::string pred_url_otsu = "/home/kyle/Desktop/HLD/predictions/SOL5/play/";
                    std::string hld_pred_url = pred_url_otsu + std::to_string(i) + std::string("_otsu_pred.png");

                    OTSU* ptr_otsu;
                    OTSU otsu(im_Gray, otsu_test_img, "linear");
                    auto end_otsu = std::chrono::steady_clock::now();
                    auto diff_otsu = end_otsu - start_otsu;
                    ptr_otsu = &otsu;

                    std::cout << std::chrono::duration <double, std::milli> (diff_otsu).count() << " ms - otsu" << std::endl;
                    cv::resize(otsu_test_img, otsu_test_img, cv::Size(1024,768), 0, 0, cv::INTER_LINEAR);
//                    cv::imwrite(hld_pred_url, otsu_test_img);
//                    cv::imshow("pred", otsu_test_img);
//                    cv::waitKey(0);


                    /**
                       * SOLUTION FIVE MIX
                    */
                    cv::resize(DP_test_img, DP_test_img, cv::Size(image.cols * 0.2, image.rows * 0.2), 0, 0,
                               cv::INTER_LINEAR);

                    const bool is_run = true;
                    const std::string file_path_in = image_url;
                    std::string pred_url = "/home/kyle/Desktop/HLD/predictions/SOL5/play/";
                    const std::string file_path_out = pred_url + std::to_string(i) + std::string("_pred.png");
                    const std::string file_path_out_edge = pred_url + std::to_string(i) + std::string("_pred_edge.png");
                    const std::string file_path_out_mask = pred_url + std::to_string(i) + std::string("_pred_mask.png");
                    auto start = std::chrono::steady_clock::now();
                    std::cout << "num= " << i << std::endl;
                    if (is_run) {
                        Sol_5.set_canny_param(10);
                        Sol_5.set_max_search_steps(9);
                        Sol_5.DynamicProgramming(DP_test_img, file_path_out, file_path_out_edge, file_path_out_mask, ptr_otsu);
                    }
                    auto end = std::chrono::steady_clock::now();
                    auto diff = end - start;
                    std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms - dp" << std::endl;
                    total_time = std::chrono::duration<double, std::milli>(diff).count() + total_time + std::chrono::duration<double, std::milli>(diff_otsu).count();
                    break;
                }

        }

    }

    std::cout << 1000/(std::chrono::duration <double, std::milli> (total_time).count()/k) << " Hz" << std::endl;
    std::cout << std::chrono::duration <double, std::milli> (total_time).count() << " ms" << std::endl;
//    imshow("OTSU Image", image);
//    imshow( "BGR Image", image );


    cv::waitKey(0);

    return 0;
}