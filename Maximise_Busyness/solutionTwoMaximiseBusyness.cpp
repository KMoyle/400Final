//
// Created by kyle on 15/05/19.
//

#include "solutionTwoMaximiseBusyness.h"
/**
* Constructer
 **/
MaximiseBusyness::MaximiseBusyness( cv::Mat& image, cv::Mat& RGB ) {


    // filter to remove noise
//    GaussianBlur(image, image, cv::Size(5,5), 0,0);
    // calculate temporary/coarse HL
    prelimHL( image, RGB );

    // Calculate fine HL
    fineHL( image, RGB );

}

std::vector<cv::Point2i> MaximiseBusyness::tempHL( int t_strong, cv::Mat &Gmag ) {

    std::vector<cv::Point2i> P;

    for (int i = 0; i < Gmag.cols - 1; i++){
        P.push_back( cv::Point2i( i,( Gmag.rows-1 ) ) );
        for (int j = 0; j < Gmag.rows -1; j++ ){

            if ( Gmag.at<uint8_t >(j, i) > t_strong ){
//                std::cout << " pixel intensity at (" << j <<"," << i << ")=" << Gmag.at<uint8_t >(j, i) << std::endl;
//                std::cout << "t_strong=" << t_strong << std::endl;
                P[i] = cv::Point2i(i, j);
                break;
            }

        }

    }

    return P;



}

void MaximiseBusyness::prelimHL(cv::Mat &image, cv::Mat &RGB ){

    int t;
    int t_min = 10;
    int t_max = 100;
    int t_increment = 2;
    float J_max = 0;
    float busyness_sky = 0;
    float busyness_ground = 0;
    float y_vals_sky = 0;
    float y_vals_ground = 0;
    float J;

    //Sobel
    std::vector<cv::Point2i> P;
    std::vector<cv::Point2i> P_coarse;
    cv::Mat mag_im;
    cv::Sobel( image, mag_im, CV_8U, 0,2,3,2);
//    cv::Scharr( image, mag_im, CV_8U, 0,1,1);
//    cv::imshow( "Mag Image", mag_im );
//    cv::waitKey(0);
    setMagImg( mag_im);
    // Iterate over rows && columns saving horizon line points where threshold is max

    int n = ( t_max - t_min )/t_increment;

    for(int k = 1; k < n; k++ ){

        t = t_min + ( t_max - t_min )/( n-1 ) * ( k - 1 );
        // Temporary HL
        P = tempHL( t, mag_im );
        // Compute sky busyness levels
        for ( int i = 0; i < mag_im.cols-1; i++){
            for ( int j = 0; j < P[i].y  ;j++){
                busyness_sky = busyness_sky + mag_im.at<uint8_t>(j, i);
                y_vals_sky = y_vals_sky + P[i].y;
//                std::cout << "j=" << j << std::endl;
            }

        }
        busyness_sky = busyness_sky/y_vals_sky;
        // Compute ground busyness levels
        for ( int ii = 0; ii < mag_im.cols-1; ii++){
            for ( int jj = P[ii].y+1; jj < mag_im.rows - 1; jj++){
                busyness_ground = busyness_ground + mag_im.at<uint8_t>(jj, ii);
                y_vals_ground = y_vals_ground + ( mag_im.rows - P[ii].y);
            }

        }
        busyness_ground = busyness_ground/y_vals_ground;

        J = busyness_ground - busyness_sky;

        if ( J > J_max ){
            J_max = J;
            P_coarse = P;
        }
        busyness_ground = 0;
        busyness_sky = 0;
        y_vals_sky = 0;
        y_vals_ground = 0;

    }

    cv::Mat C_HL = cv::Mat( mag_im.rows, mag_im.cols, CV_8U, cv::Scalar(0));

    for(int kk = 0; kk < P_coarse.size(); kk++){

        C_HL.at<uint8_t>(P_coarse[kk].y, P_coarse[kk].x) = 255;
    }

    std::vector<cv::Mat> split_im;
    cv::split( RGB, split_im);
    cv::add(split_im[2], C_HL, split_im[2]);
    cv::subtract(split_im[0], C_HL, split_im[0]);
    cv::subtract(split_im[1], C_HL, split_im[1]);

    // merging channels into Mat array
    cv::merge( split_im, RGB );

//    cv::imshow("Coarse HL", C_HL);


    /**
     * make mask
     */
    cv::Mat mask = cv::Mat( image.rows, image.cols, CV_8U, cv::Scalar(0));
    for (int i = 0; i < mask.cols - 1 ; i++){
        for (int j = 0; j < P_coarse[i].y-1; j++){

            mask.at<uint8_t >(j , i) = 255;
        }
    }

//    cv::imshow("mask HL", mask);
    image = mask;
}


void MaximiseBusyness::fineHL( cv::Mat &HL, cv::Mat &img ) {


}

cv::Mat MaximiseBusyness::getMagImg(){

    return magIMG;
}

void MaximiseBusyness::setMagImg( cv::Mat &img ){

    magIMG = img;
}