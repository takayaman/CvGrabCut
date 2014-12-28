/*=============================================================================
 * Project : CvGrabCut
 * Code : FeatureGraph.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Definition of cvgrabcut_base::FeatureGraph
 * Feature Graph for cutting keypoint graph
 *===========================================================================*/

#ifndef _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREGRAPH_H_
#define _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREGRAPH_H_

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

/*=== Define ================================================================*/

/*=== Class Definition  =====================================================*/

namespace cvgrabcut_base {


class FeatureGraph {
 public:
  /*!
   * Defoult constructor
   */
  FeatureGraph(void);

  /*!
   * Default destructor
   */
  ~FeatureGraph(void);

  /*!
   * Copy constructor
   */
  FeatureGraph(const FeatureGraph& rhs);

  /*!
   * Assignment operator
   * @param rhs Right hand side
   * @return pointer of this object
   */
  FeatureGraph& operator=(const FeatureGraph& rhs);
};

/*!
 * Log output operator
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return Pointer of google::LogSink object
 */
google::LogMessage& operator<<(google::LogMessage& lhs, const FeatureGraph& rhs);

}  // namespace cvgrabcut_base


#endif  // _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREGRAPH_H_
