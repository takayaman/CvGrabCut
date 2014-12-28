/*=============================================================================
 * Project : CvGrabCut
 * Code : App.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/20
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of cvgrabcut_app::App
 * Application handling
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <App.h>
#include <stdint.h>
#include <glog/logging.h>

#include "GrabCut.h"

/*=== Local Define / Local Const ============================================*/

/*=== Class Implementation ==================================================*/

namespace cvgrabcut_app {

/*  Log output operator */
google::LogMessage& operator<<(google::LogMessage& lhs, const App& rhs) {
  lhs.stream() << "cvgrabcut_app::App{" << std::endl \
      << "m_rectstate : " << rhs.getRectStateAsString() << std::endl \
      << "m_labelstate : " << rhs.getLabelStateAsString() << std::endl \
      << "m_probably_labelstate : " << rhs.getProbablyLabelStateAsString() << std::endl \
      << "m_itteration_count : " << rhs.getIterationCount() << std::endl \
      << "}" << std::endl;
  return lhs;
}

void getBinaryMask(const cv::Mat &commonmask, cv::Mat &binarymask)
{
    if(commonmask.empty() || CV_8UC1 != commonmask.type()){
        LOG(ERROR) << "commonmask is empty or has incorrect type (not CV_8UC1)" << std::endl;
        return;
    }
    if(binarymask.empty() || commonmask.rows != binarymask.rows || commonmask.cols != binarymask.cols)
        binarymask.create(commonmask.size(), CV_8UC1);
    binarymask = commonmask & 1;
}


/*--- Constructor / Destructor / Initialize ---------------------------------*/

/* Defoult constructor */
App::App(void)
    : m_usermask(cv::Mat()),
      m_backmodel(cv::Mat()),
      m_foremodel(cv::Mat()),
      m_rectstate(STATE_DEFAULT),
      m_labelsstatte(STATE_DEFAULT),
      m_probably_labelsstate(STATE_DEFAULT),
      is_initialized(false),
      m_rect(cv::Rect()),
      m_itteration_count(0)
{
    m_forepixels.clear();
    m_backpixels.clear();
    m_probably_forepixels.clear();
    m_probably_backpixels.clear();
}

App::App(cv::Mat &image, const std::string &windowname)
    : m_usermask(cv::Mat()),
      m_backmodel(cv::Mat()),
      m_foremodel(cv::Mat()),
      m_rectstate(STATE_DEFAULT),
      m_labelsstatte(STATE_DEFAULT),
      m_probably_labelsstate(STATE_DEFAULT),
      is_initialized(false),
      m_rect(cv::Rect()),
      m_itteration_count(0)
{
    m_forepixels.clear();
    m_backpixels.clear();
    m_probably_forepixels.clear();
    m_probably_backpixels.clear();
    setImageAndWindowName(image, windowname);
}

App::App(cv::Mat &image, const cv::Mat &foremask, const cv::Mat &backmask)
    : m_usermask(cv::Mat()),
      m_backmodel(cv::Mat()),
      m_foremodel(cv::Mat()),
      m_rectstate(STATE_DEFAULT),
      m_labelsstatte(STATE_DEFAULT),
      m_probably_labelsstate(STATE_DEFAULT),
      is_initialized(false),
      m_rect(cv::Rect()),
      m_itteration_count(0)
{
    m_forepixels.clear();
    m_backpixels.clear();
    m_probably_forepixels.clear();
    m_probably_backpixels.clear();
    setImage(image);
    setForeMask(foremask);
    setBackMask(backmask);
}

/* Default destructor */
App::~App() {
    m_forepixels.clear();
    m_backpixels.clear();
    m_probably_forepixels.clear();
    m_probably_backpixels.clear();
}

/*  Copy constructor */
App::App(const App& rhs) {
}

/* Assignment operator */
App& App::operator=(const App& rhs) {
  if (this != &rhs) {
    // TODO(N.Takayama): implement copy
  }
  return *this;
}

void App::reset(void)
{
    if(!m_usermask.empty())
        m_usermask.setTo(cv::Scalar::all(cv::GC_BGD));
    if(!m_backmodel.empty())
        m_backmodel.setTo(cv::Scalar::all(cv::GC_BGD));
    if(!m_foremodel.empty())
        m_foremodel.setTo(cv::Scalar::all(cv::GC_PR_FGD));
    m_backpixels.clear();
    m_forepixels.clear();
    m_probably_backpixels.clear();
    m_probably_forepixels.clear();
    is_initialized = false;
    m_rectstate = STATE_DEFAULT;
    m_labelsstatte = STATE_DEFAULT;
    m_probably_labelsstate = STATE_DEFAULT;
    m_itteration_count = 0;
}

/*--- Operation -------------------------------------------------------------*/
int32_t App::nextIterration(void)
{
    if(is_initialized)
        cvgrabcut_base::GrabCut::execute(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1, cvgrabcut_base::GrabCut::EVAL);
        //cv::grabCut(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1);
    else{
        if(STATE_SET != m_rectstate)
            return m_itteration_count;
        if(STATE_SET == m_labelsstatte || STATE_SET == m_probably_labelsstate)
            cvgrabcut_base::GrabCut::execute(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1, cvgrabcut_base::GrabCut::INIT_WITH_MASK);
            //cv::grabCut(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1, cv::GC_INIT_WITH_MASK);
        else
            cvgrabcut_base::GrabCut::execute(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1, cvgrabcut_base::GrabCut::INIT_WITH_RECT);
            //cv::grabCut(*m_image, m_usermask, m_rect, m_backmodel, m_foremodel, 1, cv::GC_INIT_WITH_RECT);
        is_initialized = true;
    }
    m_itteration_count++;

    m_backpixels.clear();
    m_forepixels.clear();
    m_probably_backpixels.clear();
    m_probably_forepixels.clear();
    return m_itteration_count;
}

/*--- Accessor --------------------------------------------------------------*/
void App::setImage(cv::Mat &image)
{
    m_image = &image;
}

int32_t App::getIterationCount(void) const
{
    return m_itteration_count;
}

void App::setImageAndWindowName(cv::Mat &image, const std::string &windowname)
{
    if(image.empty() || windowname.empty()){
        LOG(ERROR) << "invalid arguments. image or windowname is empty." << std::endl;
        return;
    }
    m_image = &image;
    m_windowname = &windowname;
    m_usermask.create(image.rows, image.cols, CV_8UC1);
    reset();
}

std::string App::getRectStateAsString(void) const
{
    return getStateAsString(m_rectstate);
}

std::string App::getLabelStateAsString(void) const
{
    return getStateAsString(m_labelsstatte);
}

std::string App::getProbablyLabelStateAsString(void) const
{
    return getStateAsString(m_probably_labelsstate);
}

std::string App::getStateAsString(AppState state) const
{
    if(STATE_NOTSET == state)
        return "STATE_NOTSET";
    if(STATE_INPROCESS == state)
        return "STATE_INPROCESS";
    if(STATE_SET == state)
        return "STATE_SET";
    return "IRREGAL_STATE";
}
/*--- Event -----------------------------------------------------------------*/
void App::mouseClick(int32_t event, int32_t x, int32_t y, int32_t flags, void *param)
{
    switch (event) {
    case cv::EVENT_LBUTTONDOWN:
    {
        bool is_back = (flags & BackKey) != 0;
        bool is_fore = (flags & ForeKey) != 0;
        if(STATE_NOTSET == m_rectstate && !is_back && !is_fore){
            m_rectstate = STATE_INPROCESS;
            m_rect = cv::Rect(x, y, 1, 1);
        }
        if(STATE_SET == m_rectstate && (is_back || is_fore))
            m_labelsstatte = STATE_INPROCESS;
        break;
    }
    case cv::EVENT_RBUTTONDOWN:
    {
        bool is_back = (flags & BackKey) != 0;
        bool is_fore = (flags & ForeKey) != 0;
        if(STATE_SET ==  m_rectstate && (is_back || is_fore))
            m_probably_labelsstate = STATE_INPROCESS;
        break;
    }
    case cv::EVENT_LBUTTONUP:
    {
        if(STATE_INPROCESS == m_rectstate){
            m_rect = cv::Rect(cv::Point(m_rect.x, m_rect.y), cv::Point(x, y));
            m_rectstate = STATE_SET;
            setRectInMask();
            CV_Assert(m_backpixels.empty() && m_forepixels.empty() && m_probably_backpixels.empty() && m_probably_forepixels.empty());
            showImage();
        }
        if(STATE_INPROCESS == m_labelsstatte){
            setLabelsInMask(flags, cv::Point(x, y), false);
            m_labelsstatte = STATE_SET;
            showImage();
        }
        break;
    }
    case cv::EVENT_RBUTTONUP:
    {
        if(STATE_INPROCESS == m_probably_labelsstate){
            setLabelsInMask(flags, cv::Point(x, y), true);
            m_probably_labelsstate = STATE_SET;
            showImage();
        }
        break;
    }
    case cv::EVENT_MOUSEMOVE:
    {
        if(STATE_INPROCESS == m_rectstate){
            m_rect = cv::Rect(cv::Point(m_rect.x, m_rect.y), cv::Point(x, y));
            CV_Assert(m_backpixels.empty() && m_forepixels.empty() && m_probably_backpixels.empty() && m_probably_forepixels.empty());
            showImage();
        }else if(STATE_INPROCESS ==  m_labelsstatte){
            setLabelsInMask(flags, cv::Point(x, y), false);
            showImage();
        }else if(STATE_INPROCESS == m_probably_labelsstate){
            setLabelsInMask(flags, cv::Point(x, y), true);
            showImage();
        }
        break;
    }
    default:
        break;
    }
}


void App::setRectInMask(void)
{
    CV_Assert(!m_usermask.empty());
    m_usermask.setTo(cv::GC_BGD);
    m_rect.x = std::max(0, m_rect.x);
    m_rect.y = std::max(0, m_rect.y);
    m_rect.width = std::min(m_rect.width, m_image->cols - m_rect.x);
    m_rect.height = std::min(m_rect.height, m_image->rows - m_rect.y);
    cv::Mat roi = m_usermask(m_rect);
    roi.setTo(cv::Scalar(cv::GC_PR_FGD));
}

void App::setLabelsInMask(int32_t flags, cv::Point point, bool is_probably)
{
    std::vector<cv::Point> *p_backpixels, *p_forepixels;
    uint8_t backvalue, forevalue;
    if(!is_probably){
        p_backpixels = &m_backpixels;
        p_forepixels = &m_forepixels;
        backvalue = cv::GC_BGD;
        forevalue = cv::GC_FGD;
    }else{
        p_backpixels = &m_probably_backpixels;
        p_forepixels = &m_probably_forepixels;
        backvalue = cv::GC_PR_BGD;
        forevalue = cv::GC_PR_FGD;
    }
    if(flags & BackKey){
        p_backpixels->push_back(point);
        cv::circle(m_usermask, point, 1, backvalue, DefThickness);
    }else{
        p_forepixels->push_back(point);
        cv::circle(m_usermask, point, 1, forevalue, DefThickness);
    }
}

void App::setForeMask(const cv::Mat &foremask)
{
    if(STATE_SET != m_rectstate)
            return;
    if(foremask.empty() || m_image->size() != foremask.size() || CV_8UC1 != foremask.type())
        return;
    m_foremask = &foremask;
    for(int32_t y = 0; y < foremask.rows; y++)
        for(int32_t x = 0; x < foremask.cols; x++){
            uint8_t value = foremask.at<uint8_t>(y, x);
            if(0 == value)
                continue;
            setLabelsInMask(ForeKey, cv::Point(x, y), false);
        }
}

void App::setBackMask(const cv::Mat &backmask)
{
    if(STATE_SET != m_rectstate)
        return;
    if(backmask.empty() || m_image->size() != backmask.size() || CV_8UC1 != backmask.type())
        return;
    m_backmask = &backmask;
    for(int32_t y = 0; y < backmask.rows; y++)
        for(int32_t x = 0; x < backmask.cols; x++){
            uint8_t value = backmask.at<uint8_t>(y, x);
            if(0 == value)
                continue;
            setLabelsInMask(BackKey, cv::Point(x, y), false);
        }
}

void App::setLabelsFromMask(const cv::Mat &foremask, const cv::Mat &backmask)
{
    if(m_image->empty())
        return;
    m_rect = cv::Rect(cv::Point(0, 0), cv::Point(m_image->cols, m_image->rows));
    setRectInMask();
    m_rectstate = STATE_SET;
    setForeMask(foremask);
    setBackMask(backmask);
    showImage();
    m_labelsstatte = STATE_SET;
}

void App::showImage(void) const
{
    if(m_image->empty() || m_windowname->empty()){
        return;
    }

    cv::Mat resource;
    cv::Mat binarymask;
    if(!is_initialized)
        m_image->copyTo(resource);
    else{
        getBinaryMask(m_usermask, binarymask);
        m_image->copyTo(resource, binarymask);
    }

    std::vector<cv::Point>::const_iterator it;
    for(it = m_backpixels.begin(); it != m_backpixels.end(); ++it)
        cv::circle(resource, *it, DefRadius, Blue, DefThickness);
    for(it = m_forepixels.begin(); it != m_forepixels.end(); ++it)
        cv::circle(resource, *it, DefRadius, Red, DefThickness);
    for(it = m_probably_backpixels.begin(); it != m_probably_backpixels.end(); ++it)
        cv::circle(resource, *it, DefRadius, Lightblue, DefThickness);
    for(it = m_probably_forepixels.begin(); it != m_probably_forepixels.end(); ++it)
        cv::circle(resource, *it, DefRadius, Pink, DefThickness);

    if(STATE_INPROCESS == m_rectstate || STATE_SET == m_rectstate)
        cv::rectangle(resource, m_rect.tl(), m_rect.br(), Green, 2);
    cv::imshow(*m_windowname, resource);
}

}  // namespace cvgrabcut_app


