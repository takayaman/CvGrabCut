/*=============================================================================
 * Project : CvGrabCut
 * Code : main.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/20
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * GrabCut Test
 *===========================================================================*/

/*=== Include ===============================================================*/

#include <stdint.h>
#include <glog/logging.h>

#include <opencv2/opencv.hpp>
#include "App.h"

/*=== Local Define / Local Const ============================================*/
typedef enum AppMode_TAG{
    MODE_DEFAULT = 0,
    MODE_INTERRACTIVE = MODE_DEFAULT,
    MODE_AUTO,
}AppMode;

static const std::string DefWindowName = "CvGrabCut";

/*=== Local Variable ========================================================*/
AppMode appmode = MODE_DEFAULT;

cvgrabcut_app::App* p_app;

/*=== Local Function Define =================================================*/
static void on_mouse(int32_t event, int32_t x, int32_t y, int32_t flags, void* param);


/*=== Local Function Implementation =========================================*/
void on_mouse(int32_t event, int32_t x, int32_t y, int32_t flags, void *param)
{
    if(!p_app)
        return;
    p_app->mouseClick(event, x, y, flags, param);
}


/*=== Global Function Implementation ========================================*/

int main(int argc, char *argv[]) {
    /* Initialize */
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    cv::Mat inputimage;
    cv::Mat foremask;
    cv::Mat backmask;


    if(argc < 2){
        LOG(ERROR) << "Usage: CvGrabCut [inputimage] [foregroundmask] [backgroundmask]" << std::endl;
        return EXIT_FAILURE;
    }

    inputimage = cv::imread(argv[1]);
    if(inputimage.empty()){
        LOG(ERROR) << "Can not read inputimage." << std::endl;
        return EXIT_FAILURE;
    }

    if(argc > 2){
        foremask = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
        if(foremask.empty()){
            LOG(ERROR) << "Can not read foremask." << std::endl;
            return EXIT_FAILURE;
        }
        appmode = MODE_AUTO;
    }

    if(argc > 3){
        backmask = cv::imread(argv[3], cv::IMREAD_GRAYSCALE);
        if(backmask.empty()){
            LOG(ERROR) << "Can not read backmask." << std::endl;
            return EXIT_FAILURE;
        }
    }


    if(MODE_INTERRACTIVE == appmode){
        cv::namedWindow(DefWindowName);
        cv::setMouseCallback(DefWindowName, on_mouse, 0);
        p_app = new cvgrabcut_app::App(inputimage, DefWindowName);
        p_app->showImage();

        while(1){
            uint8_t key = cv::waitKey(0);
            switch(key){
            case '\x1b':
            {
                LOG(INFO) << "Exiting ..." << std::endl;
                delete p_app;
                return EXIT_SUCCESS;
            }
            case 'r':
            {
                LOG(INFO) << std::endl;
                p_app->reset();
                p_app->showImage();
                break;
            }
            case 'n':
            {
                int32_t itteration_count = p_app->getIterationCount();
                LOG(INFO) << "<" << itteration_count << "...";
                int32_t newitteration_count = p_app->nextIterration();
                if(newitteration_count > itteration_count){
                    p_app->showImage();
                    LOG(INFO) << itteration_count << ">" << std::endl;
                }else{
                    LOG(INFO) << "rect must be determind>" << std::endl;
                }
                break;
            }
            default:
                break;
            }
        }
    }else if(MODE_AUTO == appmode){
        cv::namedWindow(DefWindowName);
        p_app = new cvgrabcut_app::App(inputimage, DefWindowName);
        p_app->setLabelsFromMask(foremask, backmask);
        while(1){
            uint8_t key = cv::waitKey(0);
            switch(key){
            {
            case '\x1b':
            {
                LOG(INFO) << "Exiting ..." << std::endl;
                delete p_app;
                return EXIT_SUCCESS;
            }
            case 'n':
            {
                int32_t itteration_count = p_app->getIterationCount();
                LOG(INFO) << "<" << itteration_count << "...";
                int32_t newitteration_count = p_app->nextIterration();
                if(newitteration_count > itteration_count){
                    p_app->showImage();
                    LOG(INFO) << itteration_count << ">" << std::endl;
                }else{
                    LOG(INFO) << "rect must be determind>" << std::endl;
                }
                break;
            }
            default:
                break;
            }
            }
        }
    }

    /* Finalize */
    google::InstallFailureSignalHandler();

    if(!p_app)
        delete p_app;

    return EXIT_SUCCESS;
}
