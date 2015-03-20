#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    cv::Mat frame;
    cv::Mat back;
    cv::Mat fore;
    cv::VideoCapture cap(0);
    cv::BackgroundSubtractorMOG2 bg;
    int backgroundFrame=100;
    std::vector<std::vector<cv::Point> > contours;
    bg.set("nmixtures",3);
    cv::namedWindow("Frame");
    cv::namedWindow("Background");
    
    for(;;)
    {
        cap >> frame;
        medianBlur(frame,frame,5);
        if (backgroundFrame > 0)
        {
            bg.operator ()(frame,fore);
            backgroundFrame--;
        }
        else
        {
            bg.operator ()(frame,fore,0);
        }
        bg.getBackgroundImage(back);
        erode(fore,fore,cv::Mat());
        dilate(fore,fore,cv::Mat());
        findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        vector<vector<Point> >hull( contours.size() );
        vector<vector<int> >hullsI( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
            if (contourArea( contours[i])>5000)
                {  convexHull( Mat(contours[i]), hull[i], false );
                    convexHull( Mat(contours[i]), hullsI[i], false );
                    drawContours( frame, hull, -1,Scalar(255,255,255),2);
                    RotatedRect rect=minAreaRect(Mat(contours[i]));
                    vector<Vec4i> defects;
                    if (hullsI.size()>0)
                    {
                        Point2f rect_points[4];rect.points(rect_points);
                        for( int j = 0; j < 4; j++ )
                            line( frame, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 1, 8 );
                        convexityDefects(Mat(contours[i]), hullsI[i], defects);
                        if(defects.size()>=3)
                        {
                            vector<Point> palm_points;
                            for(int j=0;j<defects.size();j++)
                            {
                                int startidx=defects[j][0]; Point ptStart( contours[i][startidx] );
                                int endidx=defects[j][1]; Point ptEnd( contours[i][endidx] );
                                int faridx=defects[j][2]; Point ptFar( contours[i][faridx] );
                                palm_points.push_back(ptFar);
                                palm_points.push_back(ptStart);
                                palm_points.push_back(ptEnd);
                                circle( frame, ptStart,   4
                                       , Scalar(255,255,255), 2 );
                            }
                        }
                    }
                }
    


    
        //drawContours(frame,contours,-1,Scalar(255,255,255),2);
        if(backgroundFrame>0)
        {
            putText(frame, "Background extracting, please hold...", cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
        }
        imshow("Frame",frame);
        imshow("Background",back);
        
        if(waitKey(30) >= 0) break;
    }
    return 0;
}