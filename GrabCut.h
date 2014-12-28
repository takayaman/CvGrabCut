/*=============================================================================
 * Project : CvGrabCut
 * Code : GrabCut.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/20
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Definition of GrabCut
 *===========================================================================*/

#ifndef _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_GRABCUT_H_
#define _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_GRABCUT_H_

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

#include <opencv2/opencv.hpp>

#include "gmm.h"
#include "gcgraph.h"

/*=== Define ================================================================*/

/*=== Class Definition  =====================================================*/

namespace cvgrabcut_base {



class GrabCut {    
public:
    typedef enum InitMode_TAG{
        INIT_WITH_RECT,
        INIT_WITH_MASK,
        EVAL
    }InitMode;

    /*!
    * Defoult constructor
    */
    GrabCut(void);

    /*!
    * Default destructor
    */
    ~GrabCut(void);

    static void execute(cv::Mat& image, cv::Mat& mask, cv::Rect& rect,
                        cv::Mat& backmodel, cv::Mat& foremodel,
                        int32_t itterationcount, InitMode mode);

    static void initMaskWithRect(cv::Mat& mask,
                                 cv::Size imagesize, cv::Rect rect);

    static void initGmms(const cv::Mat& image, const cv::Mat& mask,
                         Gmm& backgmm, Gmm& foregmm);

    static double_t calcBeta(const cv::Mat& image);

    static void calcNeighborWeights(const cv::Mat &image,
                                    cv::Mat &leftweight,
                                    cv::Mat &upleftweight,
                                    cv::Mat &upweight,
                                    cv::Mat &uprightweight,
                                    double beta, double gamma);

    static void checkMask(const cv::Mat &image, const cv::Mat &mask);

    static void assignGmmComponents(const cv::Mat &image, const cv::Mat &mask,
                                    const Gmm &backgmm, const Gmm& foregmm,
                                    cv::Mat &componentindices);

    static void learnGmms(const cv::Mat &image, const cv::Mat &mask,
                          const cv::Mat &componentindices,
                          Gmm &backgmm, Gmm &foregmm);

    static void constructGcGraph(const cv::Mat& image, const cv::Mat& mask,
                                 const Gmm& backgmm, const Gmm& foregmm,
                                 double_t lambda,
                                 const cv::Mat& leftweight,
                                 const cv::Mat& upleftweight,
                                 const cv::Mat& upwreight,
                                 const cv::Mat& uprightweight, GCGraph &graph);

    static void estimateSegmentation(GCGraph &graph, cv::Mat &mask);

    static double_t m_gamma;
    static double_t m_lambda;
    static double_t m_beta;
};

/*!
 * Log output operator
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return Pointer of google::LogSink object
 */
google::LogSink& operator<<(google::LogSink& lhs, const GrabCut& rhs);

}  // namespace cvgrabcut_base


#endif  // _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_GRABCUT_H_
