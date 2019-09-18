//
// Created by kyle on 5/09/19.
//
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/imgcodecs.hpp"
#include "../OTSU/solutionOneOTSU.h"

#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <map>
#include <list>
#include <math.h>
#ifndef NEW400FINAL_SOL5_H
#define NEW400FINAL_SOL5_H


class NodeFive
{
public:
    NodeFive(){}
    NodeFive(std::shared_ptr<NodeFive> p,int x_,int y_){prev=p;x=x_;y=y_;lost=0;}
    NodeFive(NodeFive &n1){x=n1.x;y=n1.y;cost=n1.cost;lost=n1.lost;prev=n1.prev;}
    std::shared_ptr<NodeFive> prev;   //Previous nodes
    int cost;                     //Total cost until now
    int lost;                     //Number of steps taken without a clear path
    int x,y;
    NodeFive& operator=(const NodeFive &n1){x=n1.x;y=n1.y;cost=n1.cost;lost=n1.lost;prev=n1.prev; return *this;}
    NodeFive& operator=(NodeFive &&n1){x=n1.x;y=n1.y;cost=n1.cost;lost=n1.lost;prev=n1.prev;n1.prev=nullptr;return *this;}

};

class HorizonLineDetectorFive
{
private:
    int max_lost_steps = 10;
    int lost_step_cost = 8;
    int max_cost = 2;
    bool constraining_y_start = true;
    float y_start = 0;
    float y_variation = 5;
    int reset_y_constraint_condition = 1;
    int canny_param = 30;
    double confidence_in_estimate = 0;


    const int STRAIGHT_PEN = 200000;

    void compute_edges();
    void delete_nodes();
    bool compute_cheapest_path( OTSU* otsu);
    bool dp(std::shared_ptr<NodeFive> n, OTSU* otsu);
    void reset_dp();
    void add_node_to_horizon(std::shared_ptr<NodeFive> n);
    bool check_y_starts(const std::vector<float> &y_starts);

    cv::Mat current_frame,current_edges, current_edges_list,current_draw;
    std::shared_ptr<NodeFive> first_node, last_node;
    cv::Mat_<int> visited;

    std::multimap<int,std::shared_ptr<NodeFive>> ntree;

public:

    int frames = 0;
    std::vector<cv::Point2i> horizon;
    std::vector<cv::Point2i> previous_horizon;
    HorizonLineDetectorFive();
    ~HorizonLineDetectorFive(){}


    void set_max_lost_steps(const int max_lost){max_lost_steps=max_lost;}
    int get_max_lost_steps(){return max_lost_steps;}
    void draw_horizon();
    int get_cany_param(){return canny_param;}

    void set_canny_param(const int cp){canny_param=cp;}
    int get_max_search_steps(){return max_lost_steps;}
    void set_max_search_steps(const int mss){max_lost_steps=mss;}
    //Detect horizon line in image
    void detect_image(const cv::Mat &frame, OTSU* otsu);
    //Detect horizon file in video
    bool DynamicProgramming(const cv::Mat &frame, const std::string video_file_out, const std::string video_file_out_edge, const std::string video_file_out_mask, OTSU* otsu);
    void save_draw_frame(const std::string file_name="draw.jpg");
    double confidenceEstimate( std::vector<cv::Point> points, const cv::Mat &img );



};
#endif //NEW400FINAL_SOL5_H
