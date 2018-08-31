/*
 * Eyebot driving controller for self-driving task
 * Created by Shuangquan Sun@USTC
 * 2018.8.31
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "eyebot++.h"

#define CAR CENTER_CAR6
#define CENTER_CAR4 184
#define CENTER_CAR5 152
#define CENTER_CAR6 194

int state = -1;
bool slow = false, obs = false, stop = false, giveway = false, sl = true, ped = false, park = false;


//drive state:  -1: stop due to obstacle, 0: stop due to stop sign, 1: straight, 2: left curve, 3:right curve
int drive_state(bool detect1, bool detect2, int left, int right, float leftangle, float rightangle) {
    if (giveway || sl || ped || park)
        slow = true;
    else if(!sl)
        slow = false;
    if (obs)
        return -1;              //stop due to obstacle
    else if (stop)
        return 0;               //stop due to stop sign
    else {
        if (detect1 && detect2) //drive straight
            return 1;
        else if (!detect1 && detect2 && rightangle < 35) {
            if (right > 350)    //turn left soon
                return 1;
            else return 2;      //turning left
        }
        else if (detect1 && !detect2 && leftangle > -35) {
            if (left < 50)      //turn right soon
                return 1;
            else return 3;      //turning right

        }
        else
            return state;       //keep last state
    }
}



int normal_drive(int left, int right){
    int middle, offset;
    middle = (left + right)/2;
    offset = CAR - middle;
    if(state == -1){    //stop due to obstacle
        VWSetSpeed(0, 0);
        return offset;
    }
    if(state == 0){     //stop due to stop sign
        VWSetSpeed(0, 0);
        return offset;
    }
    if(state == 1) {    //driving straight
        if (!slow) {    //fast mode
            if (offset >= -10 && offset <= 10)
                VWSetSpeed(300, 0);
            else
                VWSetSpeed(300, offset/2);
        }
        else {          //slow mode
            if (offset >= -10 && offset <= 10)
                VWSetSpeed(150, 0);
            else{
                VWSetSpeed(150, offset/2);
                //if(offset > 0) VWSetSpeed(150, 10);
                //else VWSetSpeed(150, -10);
            }
        }

    }
    else{//driving curves
        if(!slow) {
            if (offset >= -10 && offset <= 10)       //middle
                VWSetSpeed(250, 0);
            else if (offset < -10 && offset >= -40)  //left to centre
                VWSetSpeed(250, -20);
            else if (offset > 10 && offset < 40)     //right to centre
                VWSetSpeed(250, 20);
            else if (offset < -40)                   //very left to centre
                VWSetSpeed(250, -45);
            else                                     //very right to centre
                VWSetSpeed(250, 50);
        }
        else{
            if (offset >= -10 && offset <= 10)       //middle
                VWSetSpeed(120, 0);
            else if (offset < -10 && offset >= -40)  //left to centre
                VWSetSpeed(120, -10);
            else if (offset > 10 && offset < 40)     //right to centre
                VWSetSpeed(120, 10);
            else if (offset < -40)                   //very left to centre
                VWSetSpeed(120, -20);
            else                                     //very right to centre
                VWSetSpeed(120, 25);
        }
    }

    if(offset < 0)	LCDSetPrintf(4,0,"Dist to center line: %4d(left)",offset);
    else LCDSetPrintf(4,0,"Dist to center line: %3d(right)",offset);

    return offset;
}





int main()
{
    int key = 0, imgshow = 1, imgflag = 1;   //variables for menu switch
    int hrs, mins, secs1, secs2, ticks1, ticks2, fps, FPS = 0, tickflag = 0, frmcnt = 0;    //variables for fps calculation
    float len1, len2, len3, tmp1, tmp2, tmp3, ang, angle, leftangle = 0, rightangle = 0, linecount = 0;
    int a1, a2, a3, b1, b2, b3, c1, c2, c3, left = 0, right = 320, leftl[4], rightl[4], stopl[4];   //variables for lane detection
    int psd1;
    int offset;
    //int prv_offset, col = 0;
    bool detect1 = false, detect2 = false, detect3 = false;     // detect1/2/3: left/right/stop line detection
    cv::Mat edges;
    std::vector<cv::Vec4i> lines;

    //Hough Transform Parameters
    int threshold = 30;
    double minLineLength = 40;
    double maxLineGap = 120;

    QVGAcol img;
    CAMInit(QVGA);
    LCDMenu("Image Off", "Fast", "Slow", "End");


    do {

        tmp1 = 0;
        tmp2 = 0;
        tmp3 = 300;
        linecount = 0;

        /* Image Preprocessing & Feature Extraction using OpenCV */
        CAMGet(img);
        cv::Mat frame(240, 320, CV_8UC3, img);
        cv::Mat frame_roi = frame(cv::Rect(0, 150, 320, 90));      //Set ROI
        cv::cvtColor(frame_roi, edges, cv::COLOR_RGB2GRAY);         //convert RGB to GRAY
        cv::GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);   //GaussianBlur
        cv::Canny(edges, edges, 50, 100, 3);                        //Canny edge detection
        cv::HoughLinesP(edges, lines, 1, CV_PI / 180, threshold, minLineLength, maxLineGap); //Hough Transform



        /* Dividing lines to associated with right and left edge of the road basing on calculated slopes */
        for (size_t i = 0; i < lines.size(); i++) {
            cv::Vec4i l = lines[i];
            if ((l[2] - l[0]) == 0)
                angle = -90;
            else {
                ang = atan((float) (l[3] - l[1]) / (l[2] - l[0]));
                angle = ang * 180 / CV_PI;
            }
            /* Calculate the distance from line to the corner spot */
            if (angle > -60 && angle < -15) { //find left edge
                a1 = l[3] - l[1];
                b1 = l[0] - l[2];
                c1 = l[1] * l[2] - l[0] * l[3];
                len1 = c1 * c1 / (float) (a1 * a1 + b1 * b1);
                linecount++;
                if (len1 > tmp1) {
                    leftl[0] = l[0];
                    leftl[1] = l[1];
                    leftl[2] = l[2];
                    leftl[3] = l[3];
                    tmp1 = len1;
                    leftangle = angle;
                    detect1 = true;
                }
            } else if (angle > 15 && angle < 60) { //find right edge
                a2 = l[3] - l[1];
                b2 = l[0] - l[2];
                c2 = l[1] * l[2] - l[0] * l[3];
                len2 = (320 * a2 + c2) * (320 * a2 + c2) / (float) (a2 * a2 + b2 * b2);
                linecount++;
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
            /* Find horizontal lines */
            else if (angle > -5 && angle < 5){
                a3 = l[3] - l[1];
                b3 = l[0] - l[2];
                c3 = l[1] * l[2] - l[0] * l[3];
                len3 = (160 * a3 + 90 * b3 + c3) * (160 * a3 + 90 * b3 + c3) / (float) (a3 * a3 + b3 * b3);
                if (len3 < tmp3) {
                    stopl[0] = l[0];
                    stopl[1] = l[1];
                    stopl[2] = l[2];
                    stopl[3] = l[3];
                    tmp3 = len3;
                    detect3 = true;
                }
            }
        }



        /* Calculate left and right edge's horizontal position & Feature validation*/
        a1 = leftl[3] - leftl[1];
        b1 = leftl[0] - leftl[2];
        c1 = leftl[1] * leftl[2] - leftl[0] * leftl[3];//Left line: a1*x+b1*y+c1 = 0
        a2 = rightl[3] - rightl[1];
        b2 = rightl[0] - rightl[2];
        c2 = rightl[1] * rightl[2] - rightl[0] * rightl[3];//Right line: a2*x+b2*y+c2 = 0
        left = -(80 * b1 + c1) / a1;
        right = -(80 * b2 + c2) / a2;
        if(left < -100) detect1 = false;
        if(right > 500) detect2 = false;
        if(detect1 && detect2){
            if(-c1/a1 >= -c2/a2){ //Vanishing point is inside the image
                detect1 = false;
                detect2 = false;
            }
            else{
                line(frame_roi, cv::Point(leftl[0], leftl[1]), cv::Point(leftl[2], leftl[3]), CV_RGB(255,0,0), 2, CV_AA);
                line(frame_roi, cv::Point(rightl[0], rightl[1]), cv::Point(rightl[2], rightl[3]), CV_RGB(255,0,0), 2, CV_AA);
            }

        }
        else if(detect1 && !detect2) {
            if (left > 150){
                detect1 = false;
            }
            else
                line(frame_roi, cv::Point(leftl[0], leftl[1]), cv::Point(leftl[2], leftl[3]), CV_RGB(255, 0, 0), 2, CV_AA);
        }
        else if(!detect1 && detect2){
            if(right < 150){
                detect2 = false;
            }
            else
                line(frame_roi, cv::Point(rightl[0], rightl[1]), cv::Point(rightl[2], rightl[3]), CV_RGB(255,0,0), 2, CV_AA);
        }
        if(detect3)
            line(frame_roi, cv::Point(stopl[0], stopl[1]), cv::Point(stopl[2], stopl[3]), CV_RGB(0,0,255), 2, CV_AA);



        /* Determine the driving state and apply control algorithm */
        state = drive_state(detect1, detect2, left, right, leftangle, rightangle);
        offset = normal_drive(left, right);



        /* Print Information on LCD (every 5 frames) */
        if (frmcnt == 0 || frmcnt == 5) {
            if (!detect1 && detect2) LCDSetPrintf(0, 0, "EDGE DETECTED:     R");
            else if (detect1 && !detect2) LCDSetPrintf(0, 0, "EDGE DETECTED: L    ");
            else if (!detect1 && !detect2) LCDSetPrintf(0, 0, "EDGE DETECTED:      ");
            else LCDSetPrintf(0, 0, "EDGE DETECTED: L & R");
            switch (state) {
                case -1:
                    LCDSetPrintf(1, 0, "State: -1 - OBSTACLE AHEAD");
                    break;
                case 0:
                    LCDSetPrintf(1, 0, "State: 0 - STOP SIGN AHEAD");
                    break;
                case 1:
                    LCDSetPrintf(1, 0, "State: 1 - GO STRAIGHT    ");
                    break;
                case 2:
                    LCDSetPrintf(1, 0, "State: 2 - LEFT CURVE     ");
                    break;
                case 3:
                    LCDSetPrintf(1, 0, "State: 3 - RIGHT CURVE    ");
                    break;
                default:
                    break;
            }
            LCDSetPrintf(2, 0, "Pos_L/R: %4d %4d", left, right);
            LCDSetPrintf(3, 0, "Angle_L/R: %5.1f %5.1f", leftangle, rightangle);


//            LCDLine(0,100,480,100,WHITE);
//            LCDLine(0,160,480,160,WHITE);
//            LCDLine(0,180,480,180,WHITE);
//            LCDLine(0,200,480,200,WHITE);
//            LCDLine(0,260,480,260,WHITE);
//            if(col > 0) LCDLine(col - 1, 180 + prv_offset*2, col, 180 + offset*2, YELLOW);
//            col++;
//            if(col > 479){
//                col = 0;
//                LCDArea(0, 120, 480, 280, BLACK, 1);
//            }
//            prv_offset = offset;


        }

        detect1 = false;
        detect2 = false;
        detect3 = false;



        /* Show image on the LCD (every 10 frames) */
        if (imgshow == 1 && frmcnt == 9) {
            LCDMenu("Image Off", "Fast", "Slow", "End");
//			unsigned char *qvga_edges = edges.data;
//			LCDImageStart(0, 210, 320, 90);
//			LCDImageGray(qvga_edges);
            unsigned char* qvga_frame_roi = frame_roi.data;
            LCDImageStart(0, 120, 320, 90);
            LCDImage(qvga_frame_roi);
        } else if (imgshow == 0) {
            LCDClear();
            LCDMenu("Image On", "Fast", "Slow", "End");
            imgshow = 2;
            key = 0;
        }



        /* Meun switch */
        key = KEYRead();
        if (key == KEY1) {
            if (imgflag == 0) {
                imgshow = 1;
                imgflag = 1;
            }
            else {
                imgshow = 0;
                imgflag = 0;
            }
        }
        if (key == KEY2) sl = false;
        else if (key == KEY3) sl = true;



        /* Calculate and Print FPS & Obstacle Avoidance */
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
        //Obstacle Avoidance
        if (frmcnt == 10){
            psd1 = PSDGet(1);
            if (psd1 <= 200)
                obs = true;
            else
                obs = false;
            frmcnt = 0;
        }
        if (frmcnt == 0) {
            LCDSetPrintf(0, 45, "FPS:%2d", FPS / 10);
            FPS = 0;
        }

    } while(key != KEY4);

    VWSetSpeed(0,0);

    return 0;
}