//
// Created by kyle on 26/04/19.
//

#include "solutionThreeBCV.h"
#include <math.h>

/**
* Constructer
 **/
BetweenClassVariance::BetweenClassVariance( cv::Mat& image, cv::Mat& RGB ) {

    std::vector<double> rho_theta;

    rho_theta = coarseHLDetection( image );

    fineHLDetection( image, RGB, rho_theta );
}


std::vector<double> BetweenClassVariance::coarseHLDetection( cv::Mat& image ){

    //1.  resize img
    //    cols/M and rows/M
//    cv::imshow("Gray",image);
    cv::Mat scaled_im;
    cv::resize( image, scaled_im, cv::Size(image.cols/M, image.rows/M) );
    //2.  Select straight lines

//    cv::imshow("scaled Gray",scaled_im);
    double new_theta, new_rho;
    double sky_count = 0, nonsky_count = 0;
    double mean_s = 0, mean_g = 0;
    int total_intensities_g = 0, total_intensities_s = 0;
    double ratio_s = 0, ratio_g = 0;
    double BCV = -100;
    double best_rho = 0;
    double best_theta = 0;
    double WH = scaled_im.rows*scaled_im.cols;
    double WH_org = image.rows*image.cols;
    double scale = sqrt(pow(image.cols, 2.0) + pow(image.rows, 2.0))/sqrt(pow(scaled_im.cols, 2.0) + pow(scaled_im.rows, 2.0));

    for ( double j = 0; j < K2-1; j++){

        new_rho = j/(K2 - 1)*sqrt(pow(scaled_im.cols, 2.0) + pow(scaled_im.rows, 2.0));

        for ( double k = 0; k < K1-1; k++) {

            new_theta = (k / (K1 - 1)) * 2*M_PI;

            for (int jj = 0; jj < scaled_im.rows- 1; jj++) {

                for ( int kk = 0; kk < scaled_im.cols - 1; kk++) {

                    if (( kk * cos(new_theta) + jj * sin(new_theta)) > new_rho) {
                        nonsky_count++;
                        total_intensities_g += scaled_im.at<uchar>(jj, kk);
                    } else {
                        sky_count++;
                        total_intensities_s += scaled_im.at<uchar>(jj, kk);
                    }
                }
            }

            ratio_g = nonsky_count/WH;
            ratio_s = sky_count/WH;

            if ( total_intensities_s != 0 || sky_count != 0){
                mean_s = total_intensities_s/sky_count;
            } else{
                mean_s = 0;
            }
            if (total_intensities_g != 0 || nonsky_count != 0){
                mean_g = total_intensities_g/nonsky_count;
            } else{
                mean_g = 0;
            }

            double new_bcv = ratio_g * ratio_s * pow( ( mean_s - mean_g ), 2 );

            if ( new_bcv > BCV ){
                BCV = new_bcv;
                best_rho = new_rho;
                best_theta = new_theta;
            }

            sky_count = 0;
            nonsky_count = 0;
            total_intensities_g = 0;
            total_intensities_s = 0;
            mean_s = 0;
            mean_g = 0;
        }



    }

    cv::Point pt1, pt2;
    double a = cos(best_theta), b = sin(best_theta);

    if (pt1.x < 0){pt1.x = 0;}

//    cv::line(scaled_im, pt1, pt2, cv::Scalar(255,255, 255 ), 1, CV_AA);

//    cv::imshow( "scaled_im line", scaled_im );

//    std::cout << "best rho=" << best_rho << "\nbest theta=" << best_theta  << std::endl;


    double scaled_rho = scale * best_rho;
    double x0 = a * scaled_rho, y0 = b * scaled_rho;

    pt1.x = cvRound(x0 + (image.cols-1) * (-b));
    pt1.y = cvRound(y0 + (image.rows-1) * (a));
    pt2.x = cvRound(x0 - (image.cols-1) * (-b));
    pt2.y = cvRound(y0 - (image.rows-1) * (a));

    if (pt1.x < 0){pt1.x = 0;}

//    cv::line(image, pt1, pt2, cv::Scalar(255,255, 255 ), 1, CV_AA);

//    cv::imshow( "rho line", image );
    std::vector<double> rho_theta;
    rho_theta.push_back(scaled_rho);
    rho_theta.push_back(best_theta);

    return  rho_theta;

}



void BetweenClassVariance::fineHLDetection( cv::Mat& image, cv::Mat& RGB, std::vector<double> rho_theta ){

    double mean_sky, mean_ground = 0;
    std::vector<cv::Point2i> coarseHL(N);
    std::vector<cv::Point2i> fineHL(N);
    int x, y;
    // get points
    for( int i = 0; i < N; i++){

        if (i == 0) {
            x = 0;
        }else{
            x = (i*image.cols)/(N+1);
        }

        y = (rho_theta[0] - x*cos(rho_theta[1]))/sin(rho_theta[1]);
        coarseHL[i] = (cv::Point2i(x, y));

    }

    // compute fine HL adjustment
    for ( int ii = 0; ii < coarseHL.size(); ii++ ){
        double max_BCV = -100;
        for ( int k = -searchRadius; k < searchRadius; k++ ){
            for (int j = 1; j < computationalRadius; j++){
                mean_sky =+ image.at<uchar>((coarseHL[ii].y+k-j), coarseHL[ii].x);
                mean_ground =+ image.at<uchar>((coarseHL[ii].y+k+j), coarseHL[ii].x);
            }

           double fine_BCV = pow( ( ( mean_sky/computationalRadius ) - ( mean_ground/computationalRadius ) ),2 );

            if (fine_BCV > max_BCV){

                max_BCV = fine_BCV;
                fineHL[ii].y = coarseHL[ii].y + k;
                fineHL[ii].x =  coarseHL[ii].x;
            }
            mean_sky = 0;
            mean_ground = 0;

        }

    }

    cv::Vec4f myLine;

    /**
     * viewing fine HL points
     */
//    for (int j = 0; j < fineHL.size(); j++){
//        image.at<uchar>(fineHL[j].y, fineHL[j].x) = 255;
//    }

    cv::fitLine( fineHL, myLine, CV_DIST_HUBER, 0, 0.01, 0.01 );

    int x1, y1, x2, y2; //points for HL
    int mag = 600;
    /**
    * setting first and last point for HL
    */
    x1 = 0;
    y1 = myLine[3]+myLine[1]*-mag;

    x2 = image.cols-1;
    y2 = myLine[3]+myLine[1]*mag;


    cv::line( RGB, cv::Point(x1,y1), cv::Point(x2, y2), cv::Scalar(0,0,255), 4);


    cv::Mat bin_mask(image.size(), CV_8U );
    bin_mask = 0;

    float delta_t = y2 - y1;
    float increament = delta_t/image.cols;

    for (int ii = 0; ii < image.cols - 1; ii++){
        for (int jj = 0; jj < round(y1 + ii*increament); jj++) {
            bin_mask.at<uchar>(jj,ii) = 255;
        }
    }

    /**
     * out image binary
     */
    image = bin_mask;

//    cv::imshow("bin mask",bin_mask);



}
