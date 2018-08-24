//
// Created by Locke on 2018/8/16.
//

#include <pthread.h>
#include "eyebot++.h"
#ifdef TIMEOUT
#undef TIMEOUT
#endif
#ifdef NORMAL
#undef NORMAL
#endif
#ifdef CUSTOM
#undef CUSTOM
#endif
#ifdef Status
#undef Status
#endif
#include <opencv2/opencv.hpp>

#define CENTER_CAR1 173
#define CENTER_CAR4 173
#define CENTER_CAR6 176

#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/cmd_line_parser.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;
using namespace cv;

int stopsign=0;
int tempstop; // stop and run flag

QVGAgray img;
int number=0;
int hrs[5];
int mins[5];
int secs[5];
int ticks[5];
int during[4];

int imgenable = 1, enable=0, execing=1, count=0;
//pthread_mutex_t mutex;

int state;  //-1: stop due to obstacle, 0: stop due to stop sign, 1: drive straight, 2: turn left, 3:turn right
bool slow = false, obs = false, stop = false, giveway = false, sl = false, ped = false, park = false;
//int X;
//BYTE image1[SIZE];
//BYTE image[SIZEG];
//BYTE output[SIZEG],b[SIZEG],g[SIZEG];
//int sumb = 0,num = 0, mid=0;


int drive_state(bool detect1, bool detect2, int left, int right, float leftangle, float rightangle) {
    if (giveway || sl || ped || park)
        slow = true;
    else if(!sl)
        slow = false;
    if (obs)
        return -1;              //stop due to obstacle
    else if (stop)
        return 0;              //stop due to stop sign(using -1 now)
    else {
        if (detect1 && detect2) //drive straight
            return 1;
        else if (!detect1 && detect2 && rightangle < 35) {
            if (right > 300)    // turn left soon
                return 1;
            else return 2;      //turning left
        }
        else if (detect1 && !detect2 && leftangle > -35) {
            if (left < 50)      // turn right soon
                return 1;
            else return 3;      //turning right

        }
        else
            return state;       // keep last state
    }
}


void normal_drive(int left, int right){
    int middle, offset;
    middle = (left + right)/2;
    offset = CENTER_CAR4 - middle;
    if(state == -1){    //stop
        VWSetSpeed(0, 0);
        return;
    }
    if(state == 0){ //stop sign
        VWSetSpeed(0, 0);
        return;
    }
    if(!slow) {
        if (offset >= -25 && offset <= 25)       //middle
            VWSetSpeed(250, 0);
        else if (offset < -25 && offset >= -45)  //left to centre
            VWSetSpeed(250, -20);
        else if (offset > 25 && offset < 45)        //right to centre
            VWSetSpeed(250, 20);
        else if (offset < -45)                   //right turn
            VWSetSpeed(250, -50);
        else                                    //left turn
            VWSetSpeed(250, 50);
    }
    else{
        if (offset >= -25 && offset <= 25)       //middle
            VWSetSpeed(150, 0);
        else if (offset < -25 && offset >= -45)  //left to centre
            VWSetSpeed(150, -10);
        else if (offset > 25 && offset < 45)        //right to centre
            VWSetSpeed(150, 10);
        else if (offset < -45)                   //right turn
            VWSetSpeed(150, -25);
        else                                    //left turn
            VWSetSpeed(150, 25);
    }
//using state not stable.
//    else if(state == 1){//drive straight
//        if(offset >= -25 && offset <= 25)       //middle
//            VWSetSpeed(250, 0);
//        else if(offset < -25 && offset >= -45)  //left to centre
//            VWSetSpeed(250, -20);
//        else if(offset > 25 && offset < 45)       //right to centre
//            VWSetSpeed(250, 20);
//        else if(offset < -45)                   //right turn
//            VWSetSpeed(250, -50);
//        else                                    //left turn
//            VWSetSpeed(250, 50);
//    }
//    else if(state == 2){  //turning left
//        if(offset >= -25 && offset <= 25)
//            VWSetSpeed(150, 15);
//        else if(offset < -25 && offset >= -50)    //left to centre
//            VWSetSpeed(150, 5);
//        else if(offset > 25 && offset < 50)   //right to centre
//            VWSetSpeed(150, 25);
//        else if(offset < -50)
//            VWSetSpeed(150, -10);
//        else
//            VWSetSpeed(150, 40);
//    }
//    else if(state == 3){  //turning right
//        if(offset >= -25 && offset <= 25)
//            VWSetSpeed(150,  -15);
//        else if(offset < -25 && offset >= -50)    //left to centre
//            VWSetSpeed(150, -5);
//        else if(offset > 25 && offset < 50)   //right to centre
//            VWSetSpeed(150, -25);
//        else if(offset < -50)
//            VWSetSpeed(150, 10);
//        else
//            VWSetSpeed(150, -40);
//    }
//    if(offset < 0)    LCDSetPrintf(4,0,"Dist to center line: %4d(left)",offset);
//    else LCDSetPrintf(4,0,"Dist to center line: %3d(right)",offset);
}


void running()
{
    int key = 0, imgshow = 0, imgflag = 0, frmcnt = 0;
    int hrs, mins, secs1, secs2, ticks1, ticks2, fps, FPS = 0, tickflag = 0;
    int left = 0, right = 320, leftl[4], rightl[4];
    int a1, a2, b1, b2, c1, c2;
    int psd1;
    //int psd2 = 801, psd3 = 801;
    float len1, len2, tmp1, tmp2, ang, angle, leftangle = 0, rightangle = 0;
    bool detect1 = false, detect2 = false;
    cv::Mat edges;
    std::vector<cv::Vec4i> lines;
    unsigned char* qvga_frame_roi;

    //Hough Transform Parameters
    int threshold = 30;
    double minLineLength = 30;
    double maxLineGap = 120;

//    QVGAcol img;
//    CAMInit(QVGA);
//    LCDMenu( "Image On", "", "", "End");


    do {

//        pthread_mutex_lock(&mutex);
        CAMGetGray(img);
//        pthread_mutex_unlock(&mutex);

        cv::Mat frame(240, 320, CV_8UC1, img);
        cv::Mat frame_roi = frame(cv::Rect(0, 160, 320, 80));    //Set ROI
        edges = frame_roi;
//        cv::cvtColor(frame_roi, edges, cv::COLOR_RGB2GRAY);    //convert RGB to GRAY
        cv::GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);//GaussianBlur
        cv::Canny(edges, edges, 50, 100, 3);                //Canny edge detection

        cv::HoughLinesP(edges, lines, 1, CV_PI / 180, threshold, minLineLength, maxLineGap); //Hough Transform
        tmp1 = 0;
        tmp2 = 0;


        /* Dividing lines to associated with right and left edge of the road basing on calculated slopes */
        for (size_t i = 0; i < lines.size(); i++) {
            cv::Vec4i l = lines[i];
            //Calculate the slope of each line
            if ((l[2] - l[0]) == 0)
                angle = -90;
            else {
                ang = atan((float) (l[3] - l[1]) / (l[2] - l[0]));
                angle = ang * 180 / CV_PI;
            }
            /* Calculate the length to the nearest corner */
            if (angle > -60 && angle < -10) {        //find left edge
                a1 = l[3] - l[1];
                b1 = l[0] - l[2];
                c1 = l[1] * l[2] - l[0] * l[3];
                len1 = c1 * c1 / (float) (a1 * a1 + b1 * b1);
                if (len1 > tmp1) {
                    leftl[0] = l[0];
                    leftl[1] = l[1];
                    leftl[2] = l[2];
                    leftl[3] = l[3];
                    tmp1 = len1;
                    leftangle = angle;
                    detect1 = true;
                }
            } else if (angle > 10 && angle < 60) {        //find right edge
                a2 = l[3] - l[1];
                b2 = l[0] - l[2];
                c2 = l[1] * l[2] - l[0] * l[3];
                len2 = (320 * a2 + c2) * (320 * a2 + c2) / (float) (a2 * a2 + b2 * b2);
                if (len2 > tmp2) {
                    rightl[0] = l[0];
                    rightl[1] = l[1];
                    rightl[2] = l[2];
                    rightl[3] = l[3];
                    tmp2 = len2;
                    rightangle = angle;
                    detect2 = true;
                }
            }
        }


        /* Determining horizontal position of car on road in regards to road edges */
        a1 = leftl[3] - leftl[1];
        b1 = leftl[0] - leftl[2];
        c1 = leftl[1] * leftl[2] - leftl[0] * leftl[3];
        a2 = rightl[3] - rightl[1];
        b2 = rightl[0] - rightl[2];
        c2 = rightl[1] * rightl[2] - rightl[0] * rightl[3];
        left = -(80 * b1 + c1) / a1;
        right = -(80 * b2 + c2) / a2;

//        validation
        if(detect1 && detect2){
            if(-c1/a1 >= -c2/a2){
                if(leftangle < -60 || leftangle > -20){
                    cv::line(frame_roi, cv::Point(rightl[0], rightl[1]), cv::Point(rightl[2], rightl[3]), CV_RGB(255,0,0), 2, CV_AA);
                    detect1 = false;
                }

                if(rightangle < 20 || rightangle > 60){
                    cv::line(frame_roi, cv::Point(leftl[0], leftl[1]), cv::Point(leftl[2], leftl[3]), CV_RGB(255,0,0), 2, CV_AA);
                    detect2 = false;
                }
            }
            else{
                cv::line(frame_roi, cv::Point(leftl[0], leftl[1]), cv::Point(leftl[2], leftl[3]), CV_RGB(255,0,0), 2, CV_AA);
                cv::line(frame_roi, cv::Point(rightl[0], rightl[1]), cv::Point(rightl[2], rightl[3]), CV_RGB(255,0,0), 2, CV_AA);
            }

        }
        else if(detect1 && !detect2) {
            if (left < -100)
                detect1 = false;
            else
                cv::line(frame_roi, cv::Point(leftl[0], leftl[1]), cv::Point(leftl[2], leftl[3]), CV_RGB(255, 0, 0), 2,
                     CV_AA);
        }
        else if(!detect1 && detect2){
            if(right > 400)
                detect2 = false;
            else
                cv::line(frame_roi, cv::Point(rightl[0], rightl[1]), cv::Point(rightl[2], rightl[3]), CV_RGB(255,0,0), 2, CV_AA);
        }

        state = drive_state(detect1, detect2, left, right, leftangle, rightangle);
        normal_drive(left, right);


        /* Print Information on LCD & Obstacle Avoidance */
//        if (!detect1 && detect2) LCDSetPrintf(0, 0, "EDGE DETECTED:     R");
//        else if (detect1 && !detect2) LCDSetPrintf(0, 0, "EDGE DETECTED: L    ");
//        else if (!detect1 && !detect2) LCDSetPrintf(0, 0, "EDGE DETECTED:      ");
//        else LCDSetPrintf(0, 0, "EDGE DETECTED: L & R");
        //Obstacle Avoidance
        if (psd1 <= 200) {
            obs = true;
//            LCDSetPrintf(0, 26, "OBSTACLE AHEAD!");
        }
        else {
//            LCDSetPrintf(0, 26, "               ");
            obs = false;
        }
//        LCDSetPrintf(1, 0, "Pos_L/R: %4d %4d", left, right);
//        LCDSetPrintf(2, 0, "Angle_L/R: %5.1f %5.1f", leftangle, rightangle);
//        //LCDSetPrintf(2,0,"psd1/2/3=%3d %3d %3d", psd1, psd2, psd3);
//        LCDSetPrintf(3, 0, "                            ");
//        switch(state) {
//            case 0:
//                LCDSetPrintf(3, 0, "State:0 - KEEP DRIVING");
//                break;
//            case 1:
//                LCDSetPrintf(3, 0, "State:1 - GO STRAIGHT");
//                break;
//            case 2:
//                LCDSetPrintf(3, 0, "State:2 - TURN LEFT");
//                break;
//            case 3:
//                LCDSetPrintf(3, 0, "State:3 - TURN RIGHT");
//                break;
//        }
        //LCDSetPrintf(4,0,"Dist to center line: %4d(XXXX)",offset);

        detect1 = false;
        detect2 = false;

//        /* Show image on the LCD */
//        if (imgshow == 1 && frmcnt == 9) {
//            LCDMenu("Image Off", "", "", "End");
////            unsigned char *qvga_edges = edges.data;
////            LCDImageStart(0, 190, 320, 80);
////            LCDImageGray(qvga_edges);
//            qvga_frame_roi = frame_roi.data;
//            LCDImageStart(0, 110, 320, 80);
//            LCDImage(qvga_frame_roi);
//        } else if (imgshow == 0) {
//            LCDClear();
//            LCDMenu("Image On", "", "", "End");
//            imgshow = 2; // not 0 not 1
//            key = 0;
//        }
//        key = KEYRead();
//        if (key == 1) {
//            if (imgflag == 0) {
//                imgshow = 1;
//                imgflag = 1;
//            }
//            else {
//                imgshow = 0;
//                imgflag = 0;
//            }
//        }


        /* Calculate FPS */
        if (tickflag == 0) {
            OSGetTime(&hrs, &mins, &secs1, &ticks1);
            tickflag = 1;
        } else {
            OSGetTime(&hrs, &mins, &secs2, &ticks2);
            tickflag = 0;
        }
        if (tickflag == 0) {
            if (secs1 == secs2) fps = 1000 / (ticks2 - ticks1);
            else fps = 1000 / (ticks2 - ticks1 + 1000 * (secs2 - secs1));
        } else {
            if (secs2 == secs1) fps = 1000 / (ticks1 - ticks2);
            else fps = 1000 / (ticks1 - ticks2 + 1000 * (secs1 - secs2));
        }
        FPS += fps;
        frmcnt++;
        if (frmcnt == 10){
            psd1 = PSDGet(1);
            //psd2 = PSDGet(2);
            //psd3 = PSDGet(3);
            frmcnt = 0;
        } // no crash

        if (frmcnt == 0) {
            LCDSetPrintf(0, 45, "FPS:%2d", FPS / 10);
            FPS = 0;
        }

    } while(execing);

    VWSetSpeed(0,0);

}
struct TrafficSign {
    string name;
    string svm_path;
    rgb_pixel color;
    TrafficSign(string name, string svm_path, rgb_pixel color) :
            name(name), svm_path(svm_path), color(color) {};
};
void detectsign(BYTE* img) {
    Mat frame(240, 320, CV_8UC1, img); //img to mat
    array2d<unsigned char> dlibImage;
    const unsigned long upsample_amount = 0;
    int inputnum = 1;
    assign_image(dlibImage, cv_image<unsigned char>(frame));
        for (unsigned long i = 0; i < upsample_amount; ++i) {
            for (unsigned long j = 0; j < 1; ++j) {
                pyramid_up(dlibImage);
            }
        }
        OSGetTime(hrs+2,mins+2,secs+2,ticks+2);
        during[1] = (secs[2]-secs[1])*1000 + (ticks[2]-ticks[1]);
        //LCDSetPrintf(2,0,"SetValueAndZoomIn = %d",during[1]);
        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;

        // Load SVM detectors
        std::vector<TrafficSign> signs;

        signs.push_back(TrafficSign("UNLIMIT", "/home/pi/usr/transito-cv/merged_limitcancel_detector.svm",
                                    rgb_pixel(0,255,0)));
        signs.push_back(TrafficSign("PEDESTRIAN", "/home/pi/usr/transito-cv/pedestrian_detector.svm",
                                    rgb_pixel(0,0,255)));
        signs.push_back(TrafficSign("LIMIT", "/home/pi/usr/transito-cv/merged_limit_detector.svm",
                                    rgb_pixel(255,0,0)));
        signs.push_back(TrafficSign("PARK", "/home/pi/usr/transito-cv/park_detector.svm",
                                    rgb_pixel(255,255,0)));
        signs.push_back(TrafficSign("GIVEWAY", "/home/pi/usr/transito-cv/giveway_detector.svm",
                                    rgb_pixel(0,255,255)));
        signs.push_back(TrafficSign("STOP", "/home/pi/usr/transito-cv/stop_detector.svm",
                                    rgb_pixel(255,0,255)));

        std::vector<object_detector<image_scanner_type> > detectors;

        for (int i = 0; i < signs.size(); i++) {
            object_detector<image_scanner_type> detector;
            deserialize(signs[i].svm_path) >> detector;
            detectors.push_back(detector);
        }
            OSGetTime(hrs+3,mins+3,secs+3,ticks+3);
            during[2] = (secs[3]-secs[2])*1000 + (ticks[3]-ticks[2]);
            //LCDSetPrintf(3,0,"ReadFromModel = %d",during[2]);
        //image_window win;
        std::vector<rect_detection> rects;
        // set rects' value(time comsuming nearly 0.8 seconds)
        for (unsigned long i = 0; i < 1; ++i) {
            evaluate_detectors(detectors, dlibImage, rects);
                OSGetTime(hrs+4,mins+4,secs+4,ticks+4);
                during[3] = (secs[4]-secs[3])*1000 + (ticks[4]-ticks[3]);
                //LCDSetPrintf(4,0,"Finish = %d",during[3]);

            for (unsigned long j = 0; j < rects.size(); ++j) {
                cout << signs[rects[j].weight_index].name << endl; //print stop
                if(signs[rects[j].weight_index].name == "LIMIT") sl = true;
                if(signs[rects[j].weight_index].name == "UNLIMIT") sl = false;
                if(signs[rects[j].weight_index].name == "PARK") park = true;
                else park = false;
                if(signs[rects[j].weight_index].name == "PEDESTRIAN") ped = true;
                else ped = false;
                if(signs[rects[j].weight_index].name == "GIVEWAY") giveway = true;
                else giveway = false;
                if (tempstop == 0) {
                    if (signs[rects[j].weight_index].name == "STOP") {
                        stop = true;
                        OSWait(5000);
                        tempstop = 1;
                    } 
                    else
                    stop = false;
                }
                else{
                stop = false;
                OSWait(2000);
                tempstop = 0;
                }
                LCDSetPrintf(1,35,"sl = %d", sl);
                LCDSetPrintf(2,35,"park = %d", park);
                LCDSetPrintf(3,35,"ped = %d", ped);
                LCDSetPrintf(4,35,"giveway = %d", giveway);
                LCDSetPrintf(5,35,"stop = %d", stop);
            }
        }
    
    
}

void *slave1(void *arg)
{
    do {
        detectsign(img);
    }while(execing);
    return NULL;
}

void *slave2(void *arg)
{
    running();
    return NULL;
}

void *master(void *arg) {
    while (execing) {
        switch (KEYGet()) {
            case KEY1:
                enable = 1;
                imgenable = 1;
                break;
            case KEY2:
                enable = 0;
//                LCDClear();
//                LCDMenu("RUN", "STOP", "IMG", "END");
                break;
            case KEY3:
                imgenable = 0;
//                LCDClear();
//                LCDMenu("RUN", "STOP", "IMG", "END");
                break;
            case KEY4:
                execing = 0;
                break;
        }
    }
    return NULL;
}

int main() {
    pthread_mutex_t mutex;
    pthread_t t1, t2, t3;
    XInitThreads();
    pthread_mutex_init(&mutex,NULL);
    CAMInit(QVGA);
    LCDMenu( "", "", "", "End");
//    CAMGetGray(img);

    pthread_create(&t1, NULL, slave1, (void *) 1);
    pthread_create(&t2, NULL, slave2, (void *) 2);
    pthread_create(&t3, NULL, master, (void *) 0);
    pthread_exit(0); // will terminate program
    return  0;
}


