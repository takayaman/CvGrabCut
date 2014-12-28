/*=============================================================================
 * Project : CvGrabCut
 * Code : featurevertex.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of cvgrabcut_base::Featurevertex
 * Feature vertex for keypoint graph
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <featurevertex.h>
#include <stdint.h>
#include <glog/logging.h>

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_base {


/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
FeatureVertex::FeatureVertex() {
}

/* Default destructor */
FeatureVertex::~FeatureVertex() {
}

/*  Copy constructor */
FeatureVertex::FeatureVertex(const FeatureVertex& rhs) {
}

/* Assignment operator */
FeatureVertex& FeatureVertex::operator=(const FeatureVertex& rhs) {
  if (this != &rhs) {
    // TODO(N.Takayama): implement copy
  }
  return *this;
}

/*--- Operation -------------------------------------------------------------*/

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const FeatureVertex& rhs) {
  lhs.stream() << "cvgrabcut_base::Featurevertex{" <<
      // TODO(N.Takayama): implement out stream of memder data
      "}" << std::endl;
  return lhs;
}

/*--- Accessor --------------------------------------------------------------*/

/*--- Event -----------------------------------------------------------------*/

}  // namespace cvgrabcut_base


