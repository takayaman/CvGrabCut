/*=============================================================================
 * Project : CvGrabCut
 * Code : GrabCut.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/20
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation GrabCut
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <assert.h>
#include <stdint.h>
#include <glog/logging.h>

#include <GrabCut.h>

#include "gmm.h"

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_base {

double_t GrabCut::m_gamma;
double_t GrabCut::m_lambda;
double_t GrabCut::m_beta;

/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
GrabCut::GrabCut(void)   
{
}

/* Default destructor */
GrabCut::~GrabCut(void) {
}


/*--- Operation -------------------------------------------------------------*/
void GrabCut::execute(cv::Mat& image, cv::Mat& mask, cv::Rect& rect, cv::Mat& backmodel, cv::Mat& foremodel, int32_t itterationcount, InitMode mode){
    if(image.empty()){
        LOG(ERROR) << "image is empty" << std::endl;
        return;
    }
    if(CV_8UC3 != image.type()){
        LOG(ERROR) << "imagge must have CV_8UC3 type";
        return;
    }

    //Gmm backgmm(backmodel), foregmm(foremodel);
    Gmm backgmm, foregmm;
    cv::Mat componet_indices(image.size(), CV_32SC1);

    /* GMMの初期化 */
    if(INIT_WITH_RECT == mode || INIT_WITH_MASK == mode){
        if(INIT_WITH_RECT == mode)
            initMaskWithRect(mask, image.size(), rect);
        else
            checkMask(image, mask);
        initGmms(image, mask, backgmm, foregmm);
    }
    if(0 >= itterationcount)
        return;
    if(EVAL == mode)
        checkMask(image, mask);
    m_gamma = 50;
    m_lambda = 9 * m_gamma;
    m_beta = calcBeta(image);

    cv::Mat leftweight, upleftweight, upweight, uprightweight;
    calcNeighborWeights(image, leftweight, upleftweight, upweight, uprightweight, m_beta, m_gamma);

    /* GMMの学習と領域分割 */
    for(int32_t i = 0; i < itterationcount; i++){
        GCGraph graph;
        assignGmmComponents(image, mask, backgmm, foregmm, componet_indices);
        learnGmms(image, mask, componet_indices, backgmm, foregmm);
        constructGcGraph(image, mask, backgmm, foregmm, m_lambda, leftweight, upleftweight, upweight, uprightweight, graph);
        estimateSegmentation(graph, mask);
    }
}

void GrabCut::initMaskWithRect(cv::Mat& mask, cv::Size imagesize, cv::Rect rect){
    mask.create(imagesize, CV_8UC1);
    mask.setTo(cv::GC_BGD);

    rect.x = std::max(0, rect.x);
    rect.y = std::max(0, rect.y);
    rect.width = std::min(rect.width, imagesize.width - rect.x);
    rect.height = std::min(rect.height, imagesize.height - rect.y);

    (mask(rect)).setTo(cv::Scalar(cv::GC_PR_FGD));
}

void GrabCut::initGmms(const cv::Mat &image, const cv::Mat &mask, Gmm &backgmm, Gmm &foregmm){
    const int32_t kMeansItterationcCount = 10;
    const int32_t kMeansType = cv::KMEANS_PP_CENTERS;

    cv::Mat backlabels, forelabels;
    std::vector<cv::Vec3f> backsamples, foresamples;
    cv::Point point;
    for(point.y = 0; point.y < image.rows; point.y++)
        for(point.x = 0; point.x < image.cols; point.x++){
            if(cv::GC_BGD == mask.at<uint8_t>(point) || cv::GC_PR_BGD == mask.at<uint8_t>(point))
                backsamples.push_back((cv::Vec3f)image.at<cv::Vec3b>(point));
            else
                foresamples.push_back((cv::Vec3f)image.at<cv::Vec3b>(point));
        }
    CV_Assert(!backsamples.empty() && !foresamples.empty());
    cv::Mat tempbacksamples((int32_t)backsamples.size(), 3, CV_32FC1, &backsamples[0][0]);

    cv::TermCriteria backtermcrit = cv::TermCriteria(cv::TermCriteria::COUNT, kMeansItterationcCount, 0.0);
    cv::kmeans(tempbacksamples, Gmm::kComponentsCount, backlabels, backtermcrit, 0, kMeansType);

    cv::Mat tempforesamples((int32_t)foresamples.size(), 3, CV_32FC1, &foresamples[0][0]);

    cv::TermCriteria foretermcrit = cv::TermCriteria(cv::TermCriteria::COUNT, kMeansItterationcCount, 0.0);
    cv::kmeans(tempforesamples, Gmm::kComponentsCount, forelabels, foretermcrit, 0, kMeansType);

    backgmm.initLearning();
    for(int32_t i = 0; i < (int32_t)backsamples.size(); i++){
        backgmm.addSample(backlabels.at<int32_t>(i, 0), backsamples[i]);
    }
    backgmm.endLearning();

    foregmm.initLearning();
    for(int32_t i = 0; i < (int32_t)foresamples.size(); i++){
        foregmm.addSample(forelabels.at<int32_t>(i, 0), foresamples[i]);
    }
    foregmm.endLearning();
}

double_t GrabCut::calcBeta(const cv::Mat& image){
    double_t beta = 0;
    for(int32_t y = 0; y < image.rows; y++)
        for(int32_t x = 0; x < image.cols; x++){
            cv::Vec3d color = image.at<cv::Vec3b>(y, x);
            if(0 < x){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y, x - 1);
                beta += diff.dot(diff);
            }
            if(0 < y && 0 < x){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x - 1);
                beta += diff.dot(diff);
            }
            if(0 < y){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x);
                beta += diff.dot(diff);
            }
            if(0 < y && image.cols - 1 > x){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x + 1);
                beta += diff.dot(diff);
            }
        }
    if(beta <= std::numeric_limits<double_t>::epsilon())
        beta = 0;
    else
        beta = 1.f / (2 * beta / (4 * image.cols * image.rows - 3 * image.cols - 3 * image.rows + 2));
    return beta;
}

void GrabCut::calcNeighborWeights(const cv::Mat &image,
                                  cv::Mat &leftweight,
                                  cv::Mat &upleftweight,
                                  cv::Mat &upweight,
                                  cv::Mat &uprightweight,
                                  double beta, double gamma) {
    const double_t gamma_diversion_sqrt2 = gamma / std::sqrt(2.0f);
    leftweight.create(image.rows, image.cols, CV_64FC1);
    upleftweight.create(image.rows, image.cols, CV_64FC1);
    upweight.create(image.rows, image.cols, CV_64FC1);;
    uprightweight.create(image.rows, image.cols, CV_64FC1);
    for(int32_t y = 0; y < image.rows; y++)
        for(int32_t x = 0; x < image.cols; x++){
            cv::Vec3d color = image.at<cv::Vec3b>(y, x);
            if(0 <= x - 1){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y, x - 1);
                leftweight.at<double_t>(y, x) = gamma * cv::exp(-beta * diff.dot(diff));
            }else
                leftweight.at<double_t>(y, x) = 0;
            if(0 <= x - 1 && 0 <= y - 1){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x - 1);
                upleftweight.at<double_t>(y, x) = gamma_diversion_sqrt2 * cv::exp(-beta * diff.dot(diff));
            }else
                upleftweight.at<double_t>(y, x) = 0;
            if(0 <= y - 1){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x);
                upweight.at<double_t>(y, x) = gamma * cv::exp(-beta * diff.dot(diff));
            }else
                upweight.at<double_t>(y, x) = 0;
            if(image.cols > x + 1 && 0 <= y - 1){
                cv::Vec3d diff = color - (cv::Vec3d)image.at<cv::Vec3b>(y - 1, x + 1);
                uprightweight.at<double_t>(y, x) = gamma_diversion_sqrt2 * cv::exp(-beta * diff.dot(diff));
            }else
                uprightweight.at<double_t>(y, x) = 0;
        }
}

void GrabCut::checkMask(const cv::Mat &image, const cv::Mat &mask) {
    if(mask.empty()){
        LOG(ERROR) << "mask is empty" << std::endl;
        return;
    }
    if(CV_8UC1 != mask.type()){
        LOG(ERROR) << "mask must have CV_8UC1 type" << std::endl;
        return;
    }
    if(image.cols != mask.cols || image.rows != mask.rows){
        LOG(ERROR) << "mask must have as many rows and cols as image";
        return;
    }
    for(int32_t y = 0; y < mask.rows; y++)
        for(int32_t x = 0; x < mask.cols; x++){
            uint8_t value = mask.at<uint8_t>(y, x);
            if(cv::GC_BGD != value && cv::GC_FGD != value &&
                    cv::GC_PR_BGD != value && cv::GC_PR_FGD != value){
                LOG(ERROR) << "mask element value must be equal to" << std::endl
                           << "GC_BGD or GC_FGD or GC_PR_BGD or GC_PR_FGD" << std::endl;
                return;
            }
        }
}

void GrabCut::assignGmmComponents(const cv::Mat &image,
                                  const cv::Mat &mask,
                                  const Gmm &backgmm,
                                  const Gmm &foregmm,
                                  cv::Mat &componentindices){
    cv::Point point;
    for(point.y = 0; point.y < image.rows; point.y++)
        for(point.x = 0; point.x < image.cols; point.x++){
            cv::Vec3d color = image.at<cv::Vec3b>(point);
            uint8_t maskval = mask.at<uint8_t>(point);
            if(cv::GC_BGD == maskval || cv::GC_PR_BGD == maskval)
                componentindices.at<int32_t>(point) = backgmm.whichComponent(color);
            else
                componentindices.at<int32_t>(point) = foregmm.whichComponent(color);
        }
}

void GrabCut::learnGmms(const cv::Mat &image, const cv::Mat &mask,
                        const cv::Mat &componentindices,
                        Gmm &backgmm, Gmm &foregmm){
    backgmm.initLearning();
    foregmm.initLearning();
    cv::Point point;
    for(int32_t index_component = 0; index_component < Gmm::kComponentsCount; index_component++){
        for(point.y = 0; point.y < image.rows; point.y++)
            for(point.x = 0; point.x < image.cols; point.x++){
                if(index_component == componentindices.at<int32_t>(point)){
                    if(cv::GC_BGD == mask.at<uint8_t>(point) || cv::GC_PR_BGD == mask.at<uint8_t>(point)){
                        backgmm.addSample(index_component, image.at<cv::Vec3b>(point));
                    }
                    else{
                        foregmm.addSample(index_component, image.at<cv::Vec3b>(point));
                    }
                }
            }
    }
    backgmm.endLearning();
    foregmm.endLearning();
}

void GrabCut::constructGcGraph(const cv::Mat &image, const cv::Mat &mask,
                               const Gmm &backgmm, const Gmm &foregmm,
                               double_t lambda,
                               const cv::Mat &leftweight,
                               const cv::Mat &upleftweight,
                               const cv::Mat &upweight,
                               const cv::Mat &uprightweight, GCGraph &graph){
    int32_t vertexcount = image.cols * image.rows;
    int32_t edgecount = 2 * (4 * image.cols * image.rows - 3 * (image.cols + image.rows) + 2);
    graph.create(vertexcount, edgecount);
    cv::Point point;
    for(point.y = 0; point.y < image.rows; point.y++)
        for(point.x = 0; point.x < image.cols; point.x++){
            /* 頂点の追加 */
            int32_t originvertex = graph.addVertex();
            cv::Vec3b origincolor = image.at<cv::Vec3b>(point);

            /* Source, Sinkへの重み追加 */
            double_t fromsource_weight, tosink_weight;
            if(cv::GC_PR_BGD == mask.at<uint8_t>(point) || cv::GC_PR_FGD == mask.at<uint8_t>(point)){
                fromsource_weight = -log(backgmm(origincolor));
                tosink_weight = -log(foregmm(origincolor));
            }else if(cv::GC_BGD == mask.at<uint8_t>(point)){
                fromsource_weight = 0;
                tosink_weight = lambda;
            }else{
                fromsource_weight = lambda;
                tosink_weight = 0;
            }
            graph.addTerminalWeights(originvertex, fromsource_weight, tosink_weight);

            /* 隣接頂点への重み追加 */
            if(0 < point.x){
                double_t weight = leftweight.at<double_t>(point);
                graph.addEdges(originvertex, originvertex - 1, weight, weight);
            }
            if(0 < point.x && 0 < point.y){
                double_t weight = upleftweight.at<double_t>(point);
                graph.addEdges(originvertex, originvertex - image.cols - 1, weight, weight);
            }
            if(0 < point.y){
                double_t weight = upweight.at<double_t>(point);
                graph.addEdges(originvertex, originvertex - image.cols, weight, weight);
            }
            if(image.cols - 1 > point.x && 0 < point.y){
                double_t weight = uprightweight.at<double_t>(point);
                graph.addEdges(originvertex, originvertex - image.cols + 1, weight, weight);
            }
        }
}

void GrabCut::estimateSegmentation(GCGraph &graph, cv::Mat &mask){
    graph.maxFlow();
    cv::Point point;
    for(point.y = 0; point.y < mask.rows; point.y++)
        for(point.x = 0; point.x < mask.cols; point.x++){
            if(cv::GC_PR_BGD == mask.at<uint8_t>(point) || cv::GC_PR_FGD == mask.at<uint8_t>(point)){
                if(graph.inSourceSegment(point.y * mask.cols + point.x))
                    mask.at<uint8_t>(point) = cv::GC_PR_FGD;
                else
                    mask.at<uint8_t>(point) = cv::GC_PR_BGD;
            }
        }
}

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const GrabCut& rhs) {
  lhs.stream() << "cvgrabcut_base::GrabCut{" <<
      // TODO(N.Takayama): implement out stream of memder data
      "}" << std::endl;
  return lhs;
}

/*--- Accessor --------------------------------------------------------------*/


/*--- Event -----------------------------------------------------------------*/

}  // namespace cvgrabcut_base


