//objectTrackingTutorial.cpp

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.hpp>
#include <opencv\cv.h>
using std::string;
using std::vector;

using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MINCam1 = 0;
int H_MAXCam1 = 256;
int S_MINCam1 = 0;
int S_MAXCam1 = 256;
int V_MINCam1 = 0;
int V_MAXCam1 = 256;

int H_MINCam2 = 0;
int H_MAXCam2 = 256;
int S_MINCam2 = 0;
int S_MAXCam2 = 256;
int V_MINCam2 = 0;
int V_MAXCam2 = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowNameCam1 = "Original Image Camera1";
const string windowNameCam2 = "Original Image Camera2";
const string windowName1 = "HSV Image";
const string windowName2Cam1 = "Thresholded Image Camera1";
const string windowName2Cam2 = "Thresholded Image Camera2";
const string windowName3Cam1 = "After Morphological Operations Camera1";
const string windowName3Cam2 = "After Morphological Operations Camera2";
const string trackbarWindowNameCam1 = "TrackbarsCam1";
const string trackbarWindowNameCam2 = "TrackbarsCam2";

//coordinate storage buffer
double cam1_x[9999];
double cam2_x[9999];
double cam1_y[9999];
double cam2_y[9999];
int delayBuf[9999];
int capture_iteration = 0;

void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed





}
string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}
void createTrackbars(){
	//create window for trackbars


    namedWindow(trackbarWindowNameCam1, 0);
	namedWindow(trackbarWindowNameCam2, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MINCam1);
	sprintf( TrackbarName, "H_MAX", H_MAXCam1);
	sprintf( TrackbarName, "S_MIN", S_MINCam1);
	sprintf( TrackbarName, "S_MAX", S_MAXCam1);
	sprintf( TrackbarName, "V_MIN", V_MINCam1);
	sprintf( TrackbarName, "V_MAX", V_MAXCam1);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", trackbarWindowNameCam1, &H_MINCam1, H_MAXCam1, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowNameCam1, &H_MAXCam1, H_MAXCam1, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowNameCam1, &S_MINCam1, S_MAXCam1, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowNameCam1, &S_MAXCam1, S_MAXCam1, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowNameCam1, &V_MINCam1, V_MAXCam1, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowNameCam1, &V_MAXCam1, V_MAXCam1, on_trackbar );

	createTrackbar("H_MIN", trackbarWindowNameCam2, &H_MINCam2, H_MAXCam2, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowNameCam2, &H_MAXCam2, H_MAXCam2, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowNameCam2, &S_MINCam2, S_MAXCam2, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowNameCam2, &S_MAXCam2, S_MAXCam2, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowNameCam2, &V_MINCam2, V_MAXCam2, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowNameCam2, &V_MAXCam2, V_MAXCam2, on_trackbar);


}
void drawObject(int x, int y,Mat &frame){

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(10,10));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(16,16));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	


}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed, int delay, int cameraID) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	
	//delay desplay variable
	String delayText = "";


	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;

					//store data into buffer
					switch (cameraID)
					{
					case 1:
						cam1_x[capture_iteration] = x;
						cam1_y[capture_iteration] = y;
						break;
					case 2:
						cam2_x[capture_iteration] = x;
						cam2_x[capture_iteration] = y;
						break;
					default:
						break;
					}
					

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				delayText = "delay =" + intToString(delay) + "ms";
				putText(cameraFeed,delayText,Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

double * threeDCalculation(int cam1_x, int cam2_x, int cam1_y, int cam2_y, int d, int f) {
	//reconstruct 3-d position 
	//fomular reference can be found here http://www.docin.com/p-381981146.html 
	//need (x1,y1) (x2,y2) difference between 2 cams d and Focal of cam f
	double coord[3];
	if (cam2_x - cam1_x != 0) {
		coord[0] = d*cam1_x / (cam2_x - cam1_x);
		coord[1] = d*cam1_y / (cam2_x - cam1_x);
		coord[2] = d*f / (cam2_x - cam1_x);
	}
	else {
		coord[0] = 999;
		coord[1] = 999;
		coord[2] = 999;
	}
	return coord;
}

int main(int argc, char* argv[])
{
	//some boolean variables for different functionality within this
	//program
    bool trackObjects = true;
    bool useMorphOps = true;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeedCam1;
	Mat cameraFeedCam2;
	//matrix storage for HSV image
	Mat HSVCam1;
	Mat HSVCam2;

	//matrix storage for binary threshold image
	Mat threshold1;
	Mat threshold2;
	//x and y values for the location of the object
	int x=0, y=0;
	
	//timestampe record
	int t1, t2, tdiff = 999;
	
	//create slider bars for HSV filtering
	createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture captureCam1;
	VideoCapture captureCam2;
	//open capture object at location zero (default location for webcam)
	captureCam1.open("Green.mp4");
	captureCam2.open("Green.mp4");

	if (!captureCam1.isOpened()) {
		printf("ERROR ACQUIRING VIDEO FEED\n");
		getchar();
		return -1;
	}

	//add while loop to detect last video feed frame
	while ((captureCam1.get(CV_CAP_PROP_POS_FRAMES) < captureCam1.get(CV_CAP_PROP_FRAME_COUNT) - 1)&& (captureCam2.get(CV_CAP_PROP_POS_FRAMES) < captureCam2.get(CV_CAP_PROP_FRAME_COUNT) - 1)) {
		//set height and width of capture frame
		captureCam1.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
		captureCam2.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
		captureCam1.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
		captureCam2.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
		//start an infinite loop where webcam feed is copied to cameraFeed matrix
		//all of our operations will be performed within this loop
		while (1) {
			//store image to matrix
			captureCam1.read(cameraFeedCam1);
			captureCam2.read(cameraFeedCam2);
			//convert frame from BGR to HSV colorspace
			cvtColor(cameraFeedCam1, HSVCam1, COLOR_BGR2HSV);
			cvtColor(cameraFeedCam2, HSVCam2, COLOR_BGR2HSV);
			//filter HSV image between values and store filtered image to
			//threshold matrix
			//inRange(HSVCam1, Scalar(H_MINCam1, S_MINCam1, V_MINCam1), Scalar(H_MAXCam1, S_MAXCam1, V_MAXCam1), threshold1);
			//inRange(HSVCam2, Scalar(H_MINCam2, S_MINCam2, V_MINCam2), Scalar(H_MAXCam2, S_MAXCam2, V_MAXCam2), threshold2);
			//hardcode HSV for puregreen comment out for different color
			inRange(HSVCam1, Scalar(67, 0, 86), Scalar(95, 256, 256), threshold1);
			inRange(HSVCam2, Scalar(67, 0, 86), Scalar(95, 256, 256), threshold2);


			//hardcode thread
			//inRange(HSV, Scalar(19, 161, 160), Scalar(251, 256, 256), threshold);
			//perform morphological operations on thresholded image to eliminate noise
			//and emphasize the filtered object(s)
			//Start timestamp
			t1 = getTickCount();
			if (useMorphOps)
				morphOps(threshold1);
			morphOps(threshold2);
			//pass in thresholded frame to our object tracking function
			//this function will return the x and y coordinates of the
			//filtered object
			if (trackObjects)
				trackFilteredObject(x, y, threshold1, cameraFeedCam1, tdiff, 1);
			trackFilteredObject(x, y, threshold2, cameraFeedCam2, tdiff, 2);
			//calculate 3d coordinate, might want a separate thread so thatcapture would not be stopped
			//for now put distance between camera to be 10 and focal to be 10 since
			double * threeD = threeDCalculation(cam1_x[capture_iteration], cam2_x[capture_iteration], cam1_y[capture_iteration], cam2_y[capture_iteration], 10, 10);
			t2 = getTickCount();
			tdiff = t2 - t1;
			tdiff = tdiff / (getTickFrequency() / 1000);
			delayBuf[capture_iteration] = tdiff;
			printf("3D coordinate x: %f y: %f z: %f delay: %dms\n", threeD[0], threeD[1], threeD[2], tdiff);
			//show frames 
			imshow(windowName2Cam1, threshold1);
			imshow(windowName2Cam2, threshold2);
			imshow(windowNameCam1, cameraFeedCam1);
			imshow(windowNameCam2, cameraFeedCam2);
			//imshow(windowName1,HSV);

			//iteration index
			capture_iteration++;

			//delay 30ms so that screen can refresh.
			//image will not appear without this waitKey() command
			waitKey(30);
		}

	}




	return 0;
}
