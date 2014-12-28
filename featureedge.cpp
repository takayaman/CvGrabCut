/*=============================================================================
 * Project : CvGrabCut
 * Code : featureedge.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of cvgrabcut_base::Featureedge
 * Feature edge for keypoint graph
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <featureedge.h>
#include <stdint.h>
#include <glog/logging.h>

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_base {


/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
Featureedge::Featureedge() {
}

/* Default destructor */
Featureedge::~Featureedge() {
}

/*  Copy constructor */
Featureedge::Featureedge(const Featureedge& rhs) {
}

/* Assignment operator */
Featureedge& Featureedge::operator=(const Featureedge& rhs) {
  if (this != &rhs) {
    // TODO(N.Takayama): implement copy
  }
  return *this;
}

/*--- Operation -------------------------------------------------------------*/

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const Featureedge& rhs) {
  lhs.stream() << "cvgrabcut_base::Featureedge{" <<
      // TODO(N.Takayama): implement out stream of memder data
      "}" << std::endl;
  return lhs;
}

/*--- Accessor --------------------------------------------------------------*/

/*--- Event -----------------------------------------------------------------*/

}  // namespace cvgrabcut_base


