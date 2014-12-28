/*=============================================================================
 * Project : CvGrabCut
 * Code : featurevertex.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Definition of cvgrabcut_base::Featurevertex
 * Feature vertex for keypoint graph
 *===========================================================================*/

#ifndef _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREVERTEX_H_
#define _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREVERTEX_H_

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

/*=== Define ================================================================*/

/*=== Class Definition  =====================================================*/

namespace cvgrabcut_base {


class FeatureVertex {
 public:
  /*!
   * Defoult constructor
   */
  FeatureVertex(void);

  /*!
   * Default destructor
   */
  ~FeatureVertex(void);

  /*!
   * Copy constructor
   */
  FeatureVertex(const FeatureVertex& rhs);

  /*!
   * Assignment operator
   * @param rhs Right hand side
   * @return pointer of this object
   */
  FeatureVertex& operator=(const FeatureVertex& rhs);
};

/*!
 * Log output operator
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return Pointer of google::LogSink object
 */
google::LogMessage& operator<<(google::LogMessage& lhs, const FeatureVertex& rhs);

}  // namespace cvgrabcut_base


#endif  // _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREVERTEX_H_
