#include <QCoreApplication>
#include <QTime>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
VideoCapture cam(0);
Mat frame;
Point center(0,0);
Point leftTop;
Point rightBottom;
vector<vector<Point>>contours;
Mat mask;
QTime t;
string str;
vector<double>result;
void ErodeDilate(Mat mask)
{
    erode(mask, mask, getStructuringElement(MORPH_RECT, Size(3, 3)) );
    erode(mask, mask, getStructuringElement(MORPH_RECT, Size(3, 3)) );
    dilate( mask, mask, getStructuringElement(MORPH_RECT, Size(8, 8)) );
    dilate( mask, mask, getStructuringElement(MORPH_RECT, Size(8, 8)) );
}
void detectRoiCenter()
{
    Mat hsv,mask;

    for (int i=0;i<100;i++) {
        cam >> frame;
        flip(frame,frame,1);
        cvtColor(frame, hsv, COLOR_BGR2HLS);
        inRange(hsv, Scalar(33, 90, 205), Scalar(179, 255, 255), mask);
        ErodeDilate(mask);
        Moments m = moments(mask,true);
        center=Point(m.m10/m.m00,m.m01/m.m00);
        leftTop=Point(center.x-30,center.y-30);
        rightBottom=Point(center.x+30,center.y+30);
        circle(frame,center,5,Scalar(255,255,0),4);
        rectangle(frame,leftTop,rightBottom,Scalar(255,255,0),1);
        imshow("dectecting",frame);
        if (waitKey(1)==27)break;
    }
}
bool StartWheel(vector<vector<Point>>contours1)
{
    if (contours1.empty()) {
        putText(frame,"Time : " +str,Point(10,40),2,1,Scalar(255,0,0));
        return true;
    } else {
        putText(frame,"Time : " +str,Point(10,40),2,1,Scalar(255,0,0));
        return  false;
    }

}
string intToString(double time)
{
    stringstream ss;
    ss << time;
    str = ss.str();
    return str;
}
void StorageTime(double time)
{
    if (time > 1) {
        result.push_back(time);
    }

}
void findRoIMask()
{

    Mat RoiRect(frame,Rect(leftTop,rightBottom));
    Mat hsv,mask1;
    vector<vector<Point>>contours1;
    cvtColor(RoiRect, hsv, COLOR_BGR2HSV);
    double time = double(t.elapsed())/1000;
    intToString(time);
    inRange(hsv, Scalar(0, 255, 0), Scalar(179, 255, 255), mask1);
    findContours(mask1,contours1,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    if (!StartWheel(contours1)) {
        if (rightBottom.x-10<center.x || rightBottom.x == center.x) {
            StorageTime(time);
            t.start();
        }
    }
//    cout << "QTime : " <<  str <<endl;

}
int main()
{
    detectRoiCenter();
    destroyWindow("dectecting");
    while (1) {
        cam >> frame;
        flip(frame,frame,1);
        Mat hsv, mask;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(33, 90, 205), Scalar(179, 255, 255), mask);
        ErodeDilate(mask);
        Moments m = moments(mask,true);
        center=Point(m.m10/m.m00,m.m01/m.m00);
        circle(frame,center,5,Scalar(255,255,0),4);
        rectangle(frame,leftTop,rightBottom,Scalar(0,0,0),1);
        findRoIMask();
        for (int i = 0; i < result.size(); i++) {
            putText(frame,"set Time : "+intToString(result[result.size()-1]),Point(400,40),2,1,Scalar(255,0,0));
        }
        imshow("frame",frame);
        if (waitKey(1)==27)break;
    }
}
