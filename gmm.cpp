/*=============================================================================
 * Project : CvGraphCut
 * Code : gmm.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of cvgraphcut_base::Gmm
 * This class is modified version of GMM class in opencv3.0a
 *===========================================================================*/

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/*=== Include ===============================================================*/

#include <gmm.h>
#include <stdint.h>
#include <glog/logging.h>

#include <fstream>

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_base {

/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
Gmm::Gmm(void)
  : m_totalsamplecount(0) {
  init();
}

/* Default destructor */
Gmm::~Gmm(void) {
}

double_t Gmm::operator ()(const cv::Vec3d color) const {
  cv::Mat vector = cv::Mat(1, kVectorSize, CV_64FC1);
  for(int32_t i = 0; i < kVectorSize; i++)
    vector.at<double_t>(0, i) = color.val[i];
  return calcSumOfLikelihood(vector);
}

double_t Gmm::operator ()(int32_t index_component, const cv::Vec3d color) const {
  cv::Mat vector = cv::Mat(1, kVectorSize, CV_64FC1);
  for(int32_t i = 0; i < kVectorSize; i++)
    vector.at<double_t>(0, i) = color.val[i];
  return calcLikelihoodInComponent(index_component, vector);
}

double_t Gmm::operator ()(const cv::Mat &vector) const {
  return calcSumOfLikelihood(vector);
}


double_t Gmm::operator ()(int32_t index_component, const cv::Mat &vector) const {
  return calcLikelihoodInComponent(index_component, vector);
}

void Gmm::init(void) {
  m_coefficients.resize(kComponentsCount);
  m_covariant_matrices.resize(kComponentsCount);
  m_inverse_covariant_matrices.resize(kComponentsCount);
  m_covariant_determinants.resize(kComponentsCount);
  m_product_matrices.resize(kComponentsCount);
  m_vectormeans.resize(kComponentsCount);
  m_vectorsums.resize(kComponentsCount);
  for(int32_t i = 0; i < kComponentsCount; i++) {
    m_coefficients[i] = 0.0;
    m_covariant_matrices[i] = cv::Mat::zeros(kVectorSize, kVectorSize, CV_64FC1);
    m_inverse_covariant_matrices[i] = cv::Mat::zeros(kVectorSize, kVectorSize, CV_64FC1);
    m_covariant_determinants[i] = 0;
    m_product_matrices[i] = cv::Mat::zeros(kVectorSize, kVectorSize, CV_64FC1);
    m_samplecounts[i] = 0;
    m_vectormeans[i] = MeanData(0, 0);
    m_vectorsums[i] = MeanData(0, 0);
  }
}

void Gmm::initLearning(void) {
  for(int32_t index_components = 0; index_components < kComponentsCount; index_components++) {
    m_product_matrices[index_components] = cv::Mat::zeros(kVectorSize, kVectorSize, CV_64FC1);
    m_samplecounts[index_components] = 0;
    m_vectorsums[index_components] = MeanData(0, 0);
  }
  m_totalsamplecount = 0;
}

void Gmm::endLearning(void) {
  const double_t variance = 0.01;
  for(int32_t index_component = 0; index_component < kComponentsCount; index_component++) {
    double_t samplecount = static_cast<double_t>(m_samplecounts[index_component]);
    if(0 == samplecount)
      m_coefficients[index_component] = 0.0;
    else {
      m_coefficients[index_component] = samplecount / static_cast<double_t>(m_totalsamplecount);
    }

    MeanData &mean = m_vectormeans.at(index_component);
    for(int32_t i = 0; i < mean.channels; i++)
        mean[i] = m_vectorsums[index_component][i] / samplecount;
    //mean[0] = m_vectorsums[index_component][0] / samplecount;
    //mean[1] = m_vectorsums[index_component][1] / samplecount;
    //mean[2] = m_vectorsums[index_component][2] / samplecount;

    for(int32_t i = 0; i < m_covariant_matrices[index_component].rows; i++)
      for(int32_t j = 0; j < m_covariant_matrices[index_component].cols; j++) {
        double_t meanproduct = mean.val[i] * mean.val[j];
        double_t debug = m_product_matrices[index_component].at<double_t>(i, j);
        debug = debug / samplecount;
        debug = debug - meanproduct;
        m_covariant_matrices[index_component].at<double_t>(i, j) = m_product_matrices[index_component].at<double_t>(i, j) / samplecount - meanproduct;
      }
    double_t determinant = cv::determinant(m_covariant_matrices[index_component]);
    if(std::numeric_limits<double_t>::epsilon() >= determinant) {
      for(int32_t i = 0; i < m_covariant_matrices[index_component].rows; i++)
        m_covariant_matrices[index_component].at<double_t>(i, i) += variance;
    }
    calcInverseCovAndDeterm(index_component);
  }
  debugPrintMembers();
}

/*--- Operation -------------------------------------------------------------*/
void Gmm::addSample(int32_t index_component, const cv::Vec3d color) {
  cv::Mat vector = cv::Mat(1, kVectorSize, CV_64FC1);
  for(int32_t i = 0; i < kVectorSize; i++)
    vector.at<double_t>(0, i) = color.val[i];
  addSample(index_component, vector);
}

void Gmm::addSample(int32_t index_component, const cv::Mat vector) {
  if(m_product_matrices[index_component].cols != vector.cols) {
    LOG(ERROR) << "vector.rows must be equal to m_product_matrices[index_component].rows" << std::endl;
    return;
  }
  for(int32_t i = 0; i < m_product_matrices[index_component].rows; i++)
    for(int32_t j = 0; j < m_product_matrices[index_component].cols; j++) {
      double_t product = vector.at<double_t>(0, i) * vector.at<double_t>(0, j);
      m_product_matrices[index_component].at<double_t>(i, j) += product;
    }
  for(int32_t i = 0; i < m_vectorsums[index_component].channels; i++){
      m_vectormeans[index_component][i] += vector.at<double_t>(0, i);
  }
  m_samplecounts[index_component]++;
  m_totalsamplecount++;
}

void Gmm::calcInverseCovAndDeterm(int32_t index_component) {
  if(0 < m_coefficients[index_component]) {
    double_t determinant = cv::determinant(m_covariant_matrices[index_component]);
    m_covariant_determinants[index_component] = determinant;
    CV_Assert(std::numeric_limits<double_t>::epsilon() < determinant);
    m_inverse_covariant_matrices[index_component] = m_covariant_matrices[index_component].inv();
  }
}

double_t Gmm::calcLikelihoodInComponent(int32_t index_component, const cv::Mat &vector) const {
  double_t likelihood = 0.0;
  if(0 < m_coefficients[index_component]) {
    CV_Assert(std::numeric_limits<double_t>::epsilon() < m_covariant_determinants[index_component]);
    cv::Mat diff;
    cv::Mat mean = cv::Mat(1, kVectorSize, CV_64FC1);
    for(int32_t i = 0; i < kVectorSize; i++)
        mean.at<double_t>(0, i) = m_vectormeans[index_component][i];
    //mean.at<double_t>(0, 0) = m_vectormeans[index_component][0];
   // mean.at<double_t>(0, 1) = m_vectormeans[index_component][1];
    //mean.at<double_t>(0, 2) = m_vectormeans[index_component][2];
    CV_Assert(vector.cols == mean.cols);
    diff = vector - mean;
    cv::Mat diff_transposition = diff.t();
    cv::Mat temp = diff * m_inverse_covariant_matrices[index_component];
    temp = temp * diff_transposition;
    likelihood = 1.0f / sqrt(m_covariant_determinants[index_component]) * exp(-0.5f * temp.at<double_t>(0, 0));

    // debug print
    //LOG(INFO) << "diff : " << diff.at<double_t>(0, 0) << "," << diff.at<double_t>(0, 1) << "," << diff.at<double_t>(0, 2) << std::endl;
    //LOG(INFO) << "likelihood : " << index_component << "," << likelihood <<std::endl;
  }
  return likelihood;
}

double_t Gmm::calcSumOfLikelihood(const cv::Mat &vector) const {
  double_t likelihood = 0.0;
  for(int32_t index_component = 0; index_component < kComponentsCount; index_component++)
    likelihood += m_coefficients[index_component] * calcLikelihoodInComponent(index_component, vector);
  return likelihood;
}

void Gmm::debugPrintMembers(void) {
  /* 値の照合 */
  std::ofstream output;
  output.open("loggmm.txt");
  if(!output.is_open()) {
    LOG(INFO) << "Can not open loggmm.txt" << std::endl;
    return;
  }

  for(int32_t i = 0; i < kComponentsCount; i++) {
    output << "Component : " << i << std::endl;
    output << "Coefficient : " << m_coefficients[i] << std::endl;
    for(int32_t j = 0; j < m_vectormeans[i].cols; j++) {
      if(0 == j)
        output << "Vectormeans : " << std::endl;
      output << "(0," << j << ") : " << m_vectormeans[i].val[j] << "," << std::endl;
    }
    for(int32_t j = 0; j < m_vectorsums[i].cols; j++) {
      if(0 == j)
        output << "Vectorsums : " << std::endl;
      output << "(0," << j << ") : " << m_vectorsums[i].val[j] << "," << std::endl;
    }
    for(int32_t j = 0; j < m_product_matrices[i].rows; j++) {
      for(int32_t k = 0; k < m_product_matrices[i].cols; k++) {
        if(0 == j && 0 == k)
          output << "Productmatrix : " << std::endl;
        output << "(" << j << "," << k << ") : " << m_product_matrices[i].at<double_t>(j, k) << "," << std::endl;
      }
    }
    output << "Determinant" << m_covariant_determinants[i] << std::endl;
    for(int32_t j = 0; j < m_covariant_matrices[i].rows; j++) {
      for(int32_t k = 0; k < m_covariant_matrices[i].cols; k++) {
        if(0 == j && 0 == k)
          output << "Covariatnmatrix : " << std::endl;
        output << "(" << j << "," << k << ") : " << m_covariant_matrices[i].at<double_t>(j, k) << "," << std::endl;
      }
    }
    for(int32_t j = 0; j < m_inverse_covariant_matrices[i].rows; j++) {
      for(int32_t k = 0; k < m_inverse_covariant_matrices[i].cols; k++) {
        if(0 == j && 0 == k)
          output << "InverseCovariantmatrix : " << std::endl;
        output << "(" << j << "," << k << ") : " << m_inverse_covariant_matrices[i].at<double_t>(j, k) << "," << std::endl;
      }
    }
  }
}

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const Gmm& rhs) {
  lhs.stream() << "cvgraphcut_base::Gmm{" <<
               // TODO(N.Takayama): implement out stream of memder data
               "}" << std::endl;
  return lhs;
}

/*--- Accessor --------------------------------------------------------------*/
int32_t Gmm::whichComponent(const cv::Vec3d color) const {
  int32_t retval;
  cv::Mat vector = cv::Mat(1, kVectorSize, CV_64FC1);
  for(int32_t i = 0; i < kVectorSize; i++)
    vector.at<double_t>(0, i) = color.val[i];
  retval = whichComponent(vector);
  return retval;
}

int32_t Gmm::whichComponent(const cv::Mat vector) const {
  int32_t which = 0;
  double_t max = 0.0;
  for(int32_t index_component = 0; index_component < kComponentsCount; index_component++) {
    double_t likelihood = calcLikelihoodInComponent(index_component, vector);
    if(likelihood > max) {
      which = index_component;
      max = likelihood;
    }
  }
  return which;
}

/*--- Event -----------------------------------------------------------------*/

}  // namespace cvgraphcut_base


