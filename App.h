/*=============================================================================
 * Project : CvGrabCut
 * Code : App.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/20
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Definition of cvgrabcut_app::App
 * Application handling
 *===========================================================================*/

#ifndef _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_APP_H_
#define _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_APP_H_

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

#include <opencv2/opencv.hpp>

/*=== Define ================================================================*/

/*=== Class Definition  =====================================================*/

namespace cvgrabcut_app {

typedef enum AppState_TAG{
    STATE_DEFAULT = 0,
    STATE_NOTSET = STATE_DEFAULT,
    STATE_INPROCESS,
    STATE_SET,
    STATE_NUM,
}AppState;

const cv::Scalar Red = cv::Scalar(0, 0, 255);
const cv::Scalar Pink = cv::Scalar(230, 130, 255);
const cv::Scalar Blue = cv::Scalar(255, 0, 0);
const cv::Scalar Lightblue = cv::Scalar(255, 255, 160);
const cv::Scalar Green = cv::Scalar(0, 255, 0);

const int32_t BackKey = cv::EVENT_FLAG_CTRLKEY;
const int32_t ForeKey = cv::EVENT_FLAG_SHIFTKEY;

class App {
public:

    static const int32_t DefRadius = 2;
    static const int32_t DefThickness = -1;

    /*!
    * Defoult constructor
    */
    App(void);

    App(cv::Mat& image, const std::string& windowname);

    App(cv::Mat& image, const cv::Mat& foremask, const cv::Mat& backmask);

    /*!
    * Default destructor
    */
    ~App(void);

    /*!
    * Copy constructor
    */
    App(const App& rhs);

    /*!
    * Assignment operator
    * @param rhs Right hand side
    * @return pointer of this object
    */
    App& operator=(const App& rhs);

    void reset(void);

    void setImage(cv::Mat& image);

    void setImageAndWindowName(cv::Mat& image, const std::string& windowname);

    void showImage(void) const;

    void mouseClick(int32_t event , int32_t x, int32_t y, int32_t flags, void* param);

    void setForeMask(const cv::Mat& foremask);

    void setBackMask(const cv::Mat& backmask);

    void setLabelsFromMask(const cv::Mat& foremask, const cv::Mat& backmask);

    int32_t nextIterration(void);
    int32_t getIterationCount(void) const;

    std::string getRectStateAsString(void) const;
    std::string getLabelStateAsString(void) const;
    std::string getProbablyLabelStateAsString(void) const;

private:
    void setRectInMask(void);
    void setLabelsInMask(int32_t flags, cv::Point point, bool is_probably);
    std::string getStateAsString(AppState state) const;

private:
    cv::Mat* m_image;
    const std::string* m_windowname;

    const cv::Mat* m_foremask;
    const cv::Mat* m_backmask;

    cv::Mat m_usermask;
    cv::Mat m_backmodel, m_foremodel;

    AppState m_rectstate, m_labelsstatte, m_probably_labelsstate;
    bool is_initialized;

    cv::Rect m_rect;
    std::vector<cv::Point> m_forepixels, m_backpixels, m_probably_forepixels, m_probably_backpixels;
    int32_t m_itteration_count;
};

    static void getBinaryMask(const cv::Mat& commonmask, cv::Mat& binarymask);

    /*!
    * Log output operator
    * @param lhs Left hand side
    * @param rhs Right hand side
    * @return Pointer of google::LogSink object
    */
    google::LogMessage& operator<<(google::LogMessage& lhs, const App& rhs);

}  // namespace cvgrabcut_app


#endif  // _HOME_TAKAYAMAN_DOCUMENTS_PROGRAMMING_OPENCV_CVGRABCUT_CVGRABCUT_APP_H_
