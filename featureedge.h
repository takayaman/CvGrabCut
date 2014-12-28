/*=============================================================================
 * Project : CvGrabCut
 * Code : featureedge.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Definition of cvgrabcut_base::Featureedge
 * Feature edge for keypoint graph
 *===========================================================================*/

#ifndef _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREEDGE_H_
#define _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREEDGE_H_

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

/*=== Define ================================================================*/

/*=== Class Definition  =====================================================*/

namespace cvgrabcut_base {


class Featureedge {
 public:
  /*!
   * Defoult constructor
   */
  Featureedge(void);

  /*!
   * Default destructor
   */
  ~Featureedge(void);

  /*!
   * Copy constructor
   */
  Featureedge(const Featureedge& rhs);

  /*!
   * Assignment operator
   * @param rhs Right hand side
   * @return pointer of this object
   */
  Featureedge& operator=(const Featureedge& rhs);
};

/*!
 * Log output operator
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return Pointer of google::LogSink object
 */
google::LogMessage& operator<<(google::LogMessage& lhs, const Featureedge& rhs);

}  // namespace cvgrabcut_base


#endif  // _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_FEATUREEDGE_H_
