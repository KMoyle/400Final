//
// Created by kyle on 3/04/19.
//

#include "solutionOneOTSU.h"


/**
* Constructer
 **/
OTSU::OTSU( cv::Mat& image, cv::Mat& RGB, std::string solutionType ) {


    // Perform OTSU thresholding
    performOTSUThreshold(image, RGB, solutionType);


}

void OTSU::performOTSUThreshold(cv::Mat &image, cv::Mat &RGB, std::string solutionType  ) {

//    cv::equalizeHist( image, image );
    cv::Mat gray_im = image.clone();

    cv::Mat kernal_dialtion = cv::getStructuringElement( cv::MORPH_RECT, cv::Size(7,7));

    cv::equalizeHist( gray_im, gray_im );
    // Otsu threshold
    cv::threshold( gray_im, gray_im, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU );
//   cv::imshow("post thresh", gray_im);


    /**
     * Flood fill ground
     */
    cv::Mat flood_fill;
    flood_fill = gray_im.clone();

    cv::dilate(flood_fill, flood_fill, kernal_dialtion);

    cv::floodFill(flood_fill, cv::Point(0,0), cv::Scalar(0));
    cv::floodFill(flood_fill, cv::Point(0,flood_fill.rows-1), cv::Scalar(255));
    cv::floodFill(flood_fill, cv::Point(flood_fill.cols-1,0), cv::Scalar(0));

    cv::Mat smoothing_kernal_dialtion = cv::getStructuringElement( cv::MORPH_RECT, cv::Size(11,11));

//    cv::dilate(flood_fill, flood_fill, smoothing_kernal_dialtion);
    cv::morphologyEx( flood_fill, flood_fill , cv::MORPH_CLOSE, smoothing_kernal_dialtion);

//    cv::floodFill(flood_fill, cv::Point(0,0), cv::Scalar(0));

    for (int kk = 0; kk < flood_fill.cols; kk++) {
        cv::floodFill(flood_fill, cv::Point(kk, flood_fill.rows - 1), cv::Scalar(255));
    }
    cv::floodFill(flood_fill, cv::Point(flood_fill.cols-1,0), cv::Scalar(0));


    cv::Mat out_image;

    /**
     * pad image
     */
//    cv::Mat padded;
//    int padding = 1;
//    padded.create(image.rows + 2*padding, image.cols + 2*padding, image.type());
//    padded.setTo(cv::Scalar::all(255));
//    flood_fill.copyTo(padded(cv::Rect(padding, padding, image.cols, image.rows)));
//
//    for (int i = 0; i < padded.cols; i++ ){
//        padded.at<uchar>(padded.rows-1, i) = 0;
//    }

    /**
     * mask to remove holes
     */
//    // Loop through the border pixels and if they're black, floodFill from there
//    cv::Mat mask;
//    padded.copyTo(mask);
//    for (int i = 0; i < mask.cols; i++) {
//        if (mask.at<uchar>(0, i) == 0) {
//            cv::floodFill(mask, cv::Point(i, 0), 255, 0, 10, 10);
//        }
//        if (mask.at<uchar>(mask.rows-1, i) == 0) {
//            cv::floodFill(mask, cv::Point(i, mask.rows-1), 255, 0, 10, 10);
//        }
//    }
//    for (int i = 0; i < mask.rows; i++) {
//        if (mask.at<uchar>(i, 0) == 0) {
//            cv::floodFill(mask, cv::Point(0, i), 255, 0, 10, 10);
//        }
//        if (mask.at<uchar>(i, mask.cols-1) == 0) {
//            cv::floodFill(mask, cv::Point(mask.cols-1, i), 255, 0, 10, 10);
//        }
//    }
//
//// Compare mask with original.
//    cv::Mat newImage;
//    flood_fill.copyTo(newImage);
//    for (int row = 0; row < mask.rows; ++row) {
//        for (int col = 0; col < mask.cols; ++col) {
//            if (mask.at<uchar>(row, col) == 0) {
//                newImage.at<uchar>(row, col) = 255;
//            }
//        }
//    }

//    cv::imshow("with or with out", flood_fill);
//    cv::waitKey(0);

    cv::Mat kernal = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::morphologyEx(flood_fill, out_image, cv::MORPH_GRADIENT, kernal);

    std::vector<cv::Point> locations;   // output, locations of non-zero pixels
    cv::findNonZero(out_image, locations);
    cv::Vec4f myLine;
    cv::Vec4f fineLine;
    // Set conf of
    confidenceEstimate( locations, image );
    // set HL points
//    setHLPoints( locations );

//    std::cout << "Confidence= " << getConfidence()*100 <<"%" << std::endl;


    if ( solutionType == "nonlinear") {

        // For redline visualisation
        cv::Mat newRGB;// = cv::Mat(3, sizes, CV_8U, cv::Scalar(0) );
        std::vector<cv::Mat> split_im;
        cv::split( RGB, split_im);
        cv::add(split_im[2], out_image, split_im[2]);
        cv::subtract(split_im[0], out_image, split_im[0]);
        cv::subtract(split_im[1], out_image, split_im[1]);

        // merging channels into Mat array
        cv::merge( split_im, RGB );
    } else if ( solutionType == "linear"){

        cv::fitLine( locations, myLine, 2, 0, 0.01, 0.01 );
        const int N = 2;
        std::vector<cv::Point2i> coarseHL;
        std::vector<cv::Point2i> fineHL;

        int x1, y1, x2, y2; //points for HL
        int mag = image.cols/2*1.25;
        /**
        * setting first and last point for HL
        */
        x1 = 0;
        y1 = myLine[3]+myLine[1]*-mag;

        x2 = image.cols-1;
        y2 = myLine[3]+myLine[1]*mag;

        cv::line( RGB, cv::Point(x1,y1), cv::Point(x2, y2), cv::Scalar(0,0,255), 1);

        cv::Mat bin_mask(image.size(), CV_8U ,cv::Scalar(0));

        float delta_t = y2 - y1;
        float increament = delta_t/image.cols;

//        // output binary mask
//        for (int ii = 0; ii < image.cols - 1; ii++){
//            for (int jj = 0; jj < round(y1 + ii*increament); jj++) {
//                bin_mask.at<uchar>(jj,ii) = 255;
//            }
//        }
        // getting N COARSE POINTS
        for (int ii = 0; ii < image.cols - 1; ii += N ){

            coarseHL.push_back(cv::Point2i(ii, (y1 + ii*increament)));

        }
        setHLPoints( coarseHL );
//
//        double mean_sky, mean_ground = 0;
//        int best_x;
//        int best_y;
//        // compute fine HL adjustment
//        for ( int ii = 0; ii < coarseHL.size(); ii++ ){
//            double max_BCV = -100;
//            for ( int k = -searchRadius; k < searchRadius + 1; k++ ){
//                for (int j = 1; j < computationalRadius; j++){
//                    mean_sky = mean_sky + image.at<uchar>((coarseHL[ii].y+k-j), coarseHL[ii].x);
//                    mean_ground = mean_ground + image.at<uchar>((coarseHL[ii].y+k+j), coarseHL[ii].x);
//                }
//
//                double fine_BCV = pow( ( ( mean_sky/computationalRadius ) - ( mean_ground/computationalRadius ) ),2 );
//
//                if (fine_BCV > max_BCV){
//
//                    max_BCV = fine_BCV;
//                    best_x = coarseHL[ii].x;
//                    best_y = coarseHL[ii].y + k;
//
//                }
//
//                if (k == searchRadius){
//                        fineHL.push_back(cv::Point2i(best_x, best_y)) ;
//                }
//
//                mean_sky = 0;
//                mean_ground = 0;
//
//            }
//
//        }
//
//        cv::fitLine( fineHL, fineLine, 2, 0, 0.01, 0.01 );
//
////        for (int j = 0; j < fineHL.size(); j++){
////            image.at<uchar>(fineHL[j].y, fineHL[j].x) = 255;
////        }
//
//
//
//        /**
//* setting first and last point for HL
//*/
//        x1 = 0;
//        y1 = fineLine[3]+fineLine[1]*-mag;
//
//        x2 = image.cols-1;
//        y2 = fineLine[3]+fineLine[1]*mag;
//
//        cv::line( RGB, cv::Point(x1,y1), cv::Point(x2, y2), cv::Scalar(0,255,255), 4);

//        cv::imshow("Gray",RGB);

        /**
         * out image binary
         */
        image = bin_mask;

    }





}

double OTSU::confidenceEstimate( std::vector<cv::Point> points, cv::Mat &img ) {
    int num_pts = static_cast<int> (points.size()*0.1);

    int search_radius = 5;
    double sum_above = 0;
    double sum_below = 0;
    double avg_above = 0;
    double avg_below = 0;
    int diff_thresh = 40;
    double count = 0;
    double horizon_count = 0;


    for ( int i = 1; i < points.size(); i = i + 5 ){

        int x = points[i].x;
        int y = points[i].y;

        for ( int j = 0; j < search_radius; j++){

            sum_above = sum_above + img.at<uchar>( ( y - j ) , x );

            sum_below = sum_below + img.at<uchar>( ( y + j ) , x );
        }

        avg_above = sum_above/10;
        avg_below = sum_below/10;

        if ( diff_thresh < ( avg_above - avg_below )){

            horizon_count+=1;

        }
        sum_above = 0;
        sum_below = 0;
        avg_above = 0;
        avg_below = 0;
        count+=1;

    }

    setConfidence( ( horizon_count/count ) );

    return  horizon_count/count;
}

void OTSU::setConfidence(double conf) {

    confidence = conf;

}

double OTSU::getConfidence() {

    return confidence;
}

double OTSU::getCostOTSU( int x, int y){


    return abs(round(HL_points[x].y) - y)*(confidence);
}

void OTSU::setHLPoints( std::vector<cv::Point> points ){

    HL_points = points;
}

void OTSU::clearHLPoints( ){

    HL_points.clear();
}





































