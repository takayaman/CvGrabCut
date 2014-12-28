/*=============================================================================
 * Project : CvGrabCut
 * Code : FeatureGraph.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/25
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of cvgrabcut_base::FeatureGraph
 * Feature Graph for cutting keypoint graph
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <featuregraph.h>
#include <stdint.h>
#include <glog/logging.h>

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_base {


/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
FeatureGraph::FeatureGraph() {
}

/* Default destructor */
FeatureGraph::~FeatureGraph() {
}

/*  Copy constructor */
FeatureGraph::FeatureGraph(const FeatureGraph& rhs) {
}

/* Assignment operator */
FeatureGraph& FeatureGraph::operator=(const FeatureGraph& rhs) {
  if (this != &rhs) {
    // TODO(N.Takayama): implement copy
  }
  return *this;
}

/*--- Operation -------------------------------------------------------------*/

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const FeatureGraph& rhs) {
  lhs.stream() << "cvgrabcut_base::FeatureGraph{" <<
      // TODO(N.Takayama): implement out stream of memder data
      "}" << std::endl;
  return lhs;
}

/*--- Accessor --------------------------------------------------------------*/

/*--- Event -----------------------------------------------------------------*/

}  // namespace cvgrabcut_base


