//
// Created by kyle on 5/09/19.
//

//
// Created by kyle on 5/08/19.
//

#include "sol5.h"
#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"



HorizonLineDetectorFive::HorizonLineDetectorFive()
{

    first_node = std::make_shared<NodeFive>();
    first_node->cost=0;
    first_node->prev=nullptr;
    last_node = std::make_shared<NodeFive>();
    last_node->cost=INFINITY;
    last_node->prev=nullptr;
}


bool HorizonLineDetectorFive::dp(std::shared_ptr<NodeFive> n, OTSU* otsu)
{
    //Check if I've been here already and took a cheaper path (already)
    const int curr_visited=visited(n->y,n->x);
    if ( (curr_visited>-1 && curr_visited <= n->cost) || n->cost>=last_node->cost ) return false;
    visited(n->y,n->x) = n->cost;
    //Check if we reached the last column(done!)
    if (n->x==current_edges.cols-1)
    {
        //Save the info in the last node if it's the cheapest path
        if (last_node->cost > n->cost)
        {
            last_node->cost=n->cost;
            last_node->prev=n;
        }
        return true;
    }
    else
    {
        bool good_neighbors=false;
        //Check for neighboring pixels to see if they are edges, launch dp with all the ones that are
        for (int i=1;i<2;i++)
        {
            for (int j=-2;j<3;j++)
            {
                if (i==0 && j==0) continue;
                if (n->x+i >= current_edges.cols || n->x+i < 0 ||
                    n->y+j >= current_edges.rows || n->y+j < 0) continue;
                if (current_edges.at<char>(n->y+j,n->x+i) != 0)
                {
                    good_neighbors=true;
                    auto n1=std::make_shared<NodeFive>(n,n->x+i,n->y+j);
                    n1->cost= (max_cost-current_edges.at<char>(n1->y,n1->x)) + n->cost;
                    //Increase cost for lines not going straight
//                    std::cout << "otsu cost= " << otsu->getCostOTSU( n1->x, n1->y) << std::endl;
                    if ( abs(n->y - n1->y) >= 1) {

                        ntree.insert(std::pair<int,std::shared_ptr<NodeFive> >(n1->cost*abs(n->y - n1->y)*STRAIGHT_PEN + STRAIGHT_PEN*otsu->getCostOTSU(n1->x, n1->y) ,n1)); //Penalising non straight horizons
                    } else {
                        ntree.insert(std::pair<int,std::shared_ptr<NodeFive> >(n1->cost + STRAIGHT_PEN*otsu->getCostOTSU( n1->x, n1->y),n1));
                    }

                }
            }
        }
        //If there are no edge neighbors go dig for gold!
        if (!good_neighbors)
        {
            //If I've been lost for tso long stop searching
            if (n->lost>=max_lost_steps)
                return false;
            //Otherwise continue seraching
            for (int i=1;i<2;i++)
            {
                for (int j=-2;j<3;j++)
                {
                    if (i==0 && j==0) continue;
                    if (n->x+i >= current_edges.cols || n->x+i < 0 ||
                        n->y+j >= current_edges.rows || n->y+j < 0) continue;

                    auto n1=std::make_shared<NodeFive>(n,n->x+i,n->y+j);
                    n1->lost=1+n->lost;
                    n1->cost= lost_step_cost + n->cost;
                    ntree.insert(std::pair<int,std::shared_ptr<NodeFive> >(n1->cost*abs(n->y - n1->y)*STRAIGHT_PEN + STRAIGHT_PEN*otsu->getCostOTSU(n1->x, n1->y),n1));
                }
            }
        }
    }
    return false;
}

void HorizonLineDetectorFive::add_node_to_horizon(std::shared_ptr<NodeFive> n)
{
    cv::Point2i p1;
    p1.x=n->x;
    p1.y=n->y;
    horizon.push_back(p1);
    if (n->prev!=nullptr)
        add_node_to_horizon(n->prev);
}

bool HorizonLineDetectorFive::compute_cheapest_path(OTSU* otsu)
{
    const cv::Mat mask;
    previous_horizon = horizon;
    horizon.clear();
    visited=cv::Mat_<int>::zeros(current_edges.rows,current_edges.cols);
    visited.setTo(-1,mask);

    //Take all the edges at the left of the image and initialize paths
    //First check paths that start with edges identified
    for (size_t i=max_lost_steps;i<current_edges.rows-max_lost_steps;i++)
        //for (size_t i=75;i<80;i++)
    {
        //Check if we're constraining the start location and if i violates that constraint
        if (( constraining_y_start && confidence_in_estimate > 0.8) && std::abs(i-y_start)>y_variation) continue;

        int cost = max_cost - current_edges.at<char>(i,0);
        auto n = std::make_shared<NodeFive>(first_node,0,i);//(first_node,0,i);
        int top = 0;
        if (cost==max_cost)
        {
            n->lost = 1;
            top = i;
            cost = lost_step_cost;
        }
        n->cost = cost;
        ntree.insert(std::pair<int,std::shared_ptr<NodeFive>>(n->cost+top+pow(i,3),n));

    }
    std::map<int,std::shared_ptr<NodeFive>>::iterator curr_node;//Iterator
    curr_node=ntree.begin();

    //Start expanding shortest paths first
    while(curr_node!=ntree.end())
    {
        dp(curr_node->second, otsu);
        //Move to next node
        ntree.erase(curr_node);
        curr_node=ntree.begin();
    }

    if (last_node->prev!=nullptr)
        add_node_to_horizon(last_node->prev);
    reset_dp();
    return true;
}

void HorizonLineDetectorFive::compute_edges()
{
    int ratio = 3;
    int kernel_size = 3;

    cv::GaussianBlur(current_frame, current_edges, cv::Size(7,7), 1.5, 1.5);
    cv::Canny( current_edges, current_edges, canny_param, canny_param*ratio, kernel_size );

//    cv::imshow("current_edges", current_edges);
//    cv::waitKey(0);

}

void HorizonLineDetectorFive::detect_image(const cv::Mat &frame, OTSU* otsu){

    if (frame.channels()>1)
        cv::cvtColor(frame, current_frame, cv::COLOR_BGR2GRAY);
    else
        current_frame=frame;

    /// Canny Edge Detection
    compute_edges();
    /// The NUTs & BOLTs
    compute_cheapest_path( otsu );
}

bool HorizonLineDetectorFive::check_y_starts(const std::vector<float> &y_starts)
{
    if (y_starts.size()<2) return false;
    constraining_y_start = (abs(y_starts[y_starts.size()-1]- y_starts[y_starts.size()-2]) < 5 );
    y_start=y_starts[y_starts.size()-1];
    y_variation=10;
    return constraining_y_start;
}

bool HorizonLineDetectorFive::DynamicProgramming(const cv::Mat &frame,const std::string video_file_out, const std::string video_file_out_edge, const std::string video_file_out_mask, OTSU* otsu )
{
    int i = 0;
    frames = frames + 1;
    int horizon_not_found=0;
    std::vector<float> y_starts; //Vector where we store the y location of the initial frames before we constraint the y start

    /// Detect HL
    detect_image(frame, otsu);

    /// Create output mask
    cv::Mat output_mask = cv::Mat( current_edges.rows, current_edges.cols - 1, CV_8U, cv::Scalar(0));

    /// Check if horizon found
    if (horizon.size()>0)
    {
        std::cout<<"YASS Horizon "<<std::endl;

        /// Reset HL est counter for y_start
        horizon_not_found=0;
        /// Draw estimated HL
        for (int i = output_mask.cols; i > 0 ; i--){
            for (int j = 0; j < horizon[i].y  ; j++){
                output_mask.at<uint8_t >(j , horizon[i].x) = 255;
            }
        }
        /// resize output binary mask
        cv::resize(output_mask, output_mask, cv::Size(1024,768), 0, 0, cv::INTER_LINEAR);
        /// Confidence estimate of HL
        confidence_in_estimate = confidenceEstimate(horizon, current_frame);
//        std::cout << "confidence= " << confidence_in_estimate<< std::endl;


        if (constraining_y_start)
        {
            y_start = horizon[horizon.size()-2].y;
//            std::cout << "ystart= " << y_start << std::endl;
        }
        else
        {
            y_starts.push_back(horizon[horizon.size()-2].y);
            if (check_y_starts(y_starts))
                std::cout<<"Stable y coordinate of starting line found!"<<std::endl;
        }
    }
    else {
        std::cout << "NO Horizon " << std::endl;
        i++;
        horizon_not_found += i;
        //Reset the y constraint if horizon is not seen in the last n frames
        if (horizon_not_found > reset_y_constraint_condition && constraining_y_start) {
            constraining_y_start = false;
            y_starts.clear();
        }
    }

    /// Draw HL
    draw_horizon();
    /// Save prediction frame
    save_draw_frame(video_file_out);
    /// Show prediction frame
//    cv::imshow("pred", current_draw);
//    cv::waitKey(0);
    /// Write Edge image
//    cv::resize(current_edges, current_edges, cv::Size(1024,768), 0, 0, cv::INTER_LINEAR);
//    cv::imwrite(video_file_out_edge,current_edges);
    /// Write output binary mask
    //cv::imwrite(video_file_out_mask,output_mask);
    otsu->clearHLPoints();
    return true;
}

void HorizonLineDetectorFive::draw_horizon()
{
    const cv::Scalar s1(0,0,255);
    cv::cvtColor(current_frame,current_draw, cv::COLOR_GRAY2RGB);
    for (size_t i=0;i<horizon.size();i++)
    {
        if (horizon[i].x+1 < current_edges.cols && horizon[i].y+1 < current_edges.rows)
            current_draw.colRange(horizon[i].x,horizon[i].x+1)
                    .rowRange(horizon[i].y,horizon[i].y+1)=s1;
    }
}


void HorizonLineDetectorFive::save_draw_frame(const std::string file_name)
{
    /// resize output binary mask
    cv::resize(current_draw, current_draw, cv::Size(1024,768), 0, 0, cv::INTER_LINEAR);
    cv::imwrite(file_name,current_draw);


}

void HorizonLineDetectorFive::reset_dp()
{
    delete_nodes();
    first_node->cost=0;
    first_node->prev=nullptr;
    last_node->cost=INFINITY;
}

void HorizonLineDetectorFive::delete_nodes()
{
    last_node->prev=nullptr;
    ntree.clear();
}

//void HorizonLineDetectorFive::averageHorizon() {
//
//
//}

double HorizonLineDetectorFive::confidenceEstimate( std::vector<cv::Point> points, const cv::Mat &img ) {
    int num_pts = static_cast<int> (points.size()*0.1);

    int search_radius = 15;
    double sum_above = 0;
    double sum_below = 0;
    double avg_above = 0;
    double avg_below = 0;
    int diff_thresh = 70;
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

    return  horizon_count/count;
}