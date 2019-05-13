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

    cv::Mat kernal_dialtion = getStructuringElement( cv::MORPH_CROSS, cv::Size(20,20));


    // Otsu threshold
    cv::threshold( image, image, 0, 255, cv::THRESH_BINARY_INV | CV_THRESH_OTSU );
//    cv::imshow("post thresh", image);

    /**
     * Flood fill ground
     */
    cv::Mat flood_fill;
    cv::dilate(image, image, kernal_dialtion);
    flood_fill = image.clone();

//    cv::imshow("pre flood fill", image);

    cv::floodFill(flood_fill, cv::Point(320,0), cv::Scalar(255));

//    cv::imshow("post flood fill", flood_fill);

    cv::Mat im_floodfill_inv;
    bitwise_not(flood_fill, im_floodfill_inv);
    image = ( image | im_floodfill_inv);

//    cv::imshow("pre erode", image);

    cv::erode(image, image , kernal_dialtion);
    cv::morphologyEx( image, image , cv::MORPH_OPEN, kernal_dialtion);

//    cv::imshow("post erode", image);

    // For computing outline of binary
    cv::Mat out_image;

    // NL OUTPUT MASK
    image = ~image;


    /**
     * pad image
     */
    cv::Mat padded;
    int padding = 1;
    padded.create(image.rows + 2*padding, image.cols + 2*padding, image.type());
    padded.setTo(cv::Scalar::all(255));
    image.copyTo(padded(cv::Rect(padding, padding, image.cols, image.rows)));

    for (int i = 0; i < padded.cols; i++ ){
        padded.at<uchar>(padded.rows-1, i) = 0;
    }

    /**
     * mask to remove holes
     */
    // Loop through the border pixels and if they're black, floodFill from there
    cv::Mat mask;
    padded.copyTo(mask);
    for (int i = 0; i < mask.cols; i++) {
        if (mask.at<uchar>(0, i) == 0) {
            cv::floodFill(mask, cv::Point(i, 0), 255, 0, 10, 10);
        }
        if (mask.at<uchar>(mask.rows-1, i) == 0) {
            cv::floodFill(mask, cv::Point(i, mask.rows-1), 255, 0, 10, 10);
        }
    }
    for (int i = 0; i < mask.rows; i++) {
        if (mask.at<uchar>(i, 0) == 0) {
            cv::floodFill(mask, cv::Point(0, i), 255, 0, 10, 10);
        }
        if (mask.at<uchar>(i, mask.cols-1) == 0) {
            cv::floodFill(mask, cv::Point(mask.cols-1, i), 255, 0, 10, 10);
        }
    }

// Compare mask with original.
    cv::Mat newImage;
    image.copyTo(newImage);
    for (int row = 0; row < mask.rows; ++row) {
        for (int col = 0; col < mask.cols; ++col) {
            if (mask.at<uchar>(row, col) == 0) {
                newImage.at<uchar>(row, col) = 255;
            }
        }
    }




//    cv::imshow("padded ", newImage);

    image = newImage;

    cv::Mat kernal_dilate = getStructuringElement( cv::MORPH_CROSS, cv::Size(3,3));
    cv::dilate(image, image, kernal_dilate);

//    cv::imshow("image", image);

    cv::Mat kernal = getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::morphologyEx(image, out_image, cv::MORPH_GRADIENT, kernal);

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

        std::vector<cv::Point> locations;   // output, locations of non-zero pixels
        cv::findNonZero(out_image, locations);
        cv::Vec4f myLine;

        cv::fitLine( locations, myLine, CV_DIST_L1, 0, 0.01, 0.01 );

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

    }





}


void OTSU::lineFitting( cv::Mat &HL, cv::Mat &img ) {


}

