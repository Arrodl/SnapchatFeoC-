#include "stdafx.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

// Declaramos las variables

// Abrimos un stream para la captura de video
VideoCapture stream(0);
Mat edges;
String face_cascade_name, eyes_cascade_name;
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name_face = "Face Detector";
String window_name_edges = "Edge Detector";
String imageName = ("lentesteodoro.png");

int thresh = 0;
int max_thresh = 2;
RNG rng(12345);
void thresh_callback(int, void*);
void MyPolygon(Mat img, int w);
void applyChangesToFrame(Mat frame);
void detectFaceInFrame(Mat frame);

int main(int argc, const char** argv)
{
	CommandLineParser parser(argc, argv,
		"{help h||}"
		"{face_cascade|haarcascades/haarcascade_frontalface_alt.xml|}"
		"{eyes_cascade|haarcascades/haarcascade_eye_tree_eyeglasses.xml|}");
	parser.printMessage();
	face_cascade_name = "haarcascades/haarcascade_frontalface_alt.xml";
	eyes_cascade_name = "haarcascades/haarcascade_eye_tree_eyeglasses.xml";
	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -1; };

	parser.printMessage();
	if (!stream.isOpened())  // check if we succeeded
		return -1;
	namedWindow(window_name_edges, 1);

	Mat frame;

	while (stream.read(frame)) {
		if (frame.empty()) {
			break;
		}
		createTrackbar(" Threshold:", window_name_edges, &thresh, max_thresh, thresh_callback);
		switch (thresh)
		{
		case 1:
			applyChangesToFrame(frame);
			break;
		case 2:
			detectFaceInFrame(frame);
			break;
		case 0:
			stream >> frame;
			cvtColor(frame, edges, COLOR_BGR2GRAY);
			GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
			imshow(window_name_edges, edges);
			break;
		}
		char c = (char)waitKey(10);
		if (c == 27)
			break;
	}
	return 0;
}

void thresh_callback(int, void*) {

}

/// <sumary>
/// Metodo para aplicar blur y deteccion de lineas a imagen
/// </sumary>
void applyChangesToFrame(Mat frame) {
	stream >> frame;
	cvtColor(frame, edges, COLOR_BGR2GRAY);
	Canny(edges, edges, 0, 30, 3);
	imshow(window_name_edges, edges);
}// changeEveryFrame

 /// <sumary>
 /// Metodo detectar la cara y sobreponer una imagen
 /// </sumary>
void detectFaceInFrame(Mat frame) {
	stream >> frame;
	std::vector<Rect> faces;
	Mat frame_gray;
	Mat image;
	image = imread(imageName, IMREAD_UNCHANGED);
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point2i center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

		if (!image.empty()) {
			//Resize image to face
			Size sizeOfFace(50, 10);
			resize(image, image, sizeOfFace, 0.0, 0.0, INTER_AREA);

			MyPolygon(frame_gray, faces[i].width);
		}
		//addWeighted(frame_gray, 1.0, image,1.0, 0.0, frame_gray);

		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		Mat faceROI = frame_gray(faces[i]);
	}
	imshow(window_name_edges, frame);
}

void MyPolygon(Mat img, int w)
{
	int lineType = LINE_8;
	Point rook_points[1][20];
	rook_points[0][0] = Point(w / 4, 7 * w / 8);
	rook_points[0][1] = Point(3 * w / 4, 7 * w / 8);
	rook_points[0][2] = Point(3 * w / 4, 13 * w / 16);
	rook_points[0][3] = Point(11 * w / 16, 13 * w / 16);
	rook_points[0][4] = Point(19 * w / 32, 3 * w / 8);
	rook_points[0][5] = Point(3 * w / 4, 3 * w / 8);
	rook_points[0][6] = Point(3 * w / 4, w / 8);
	rook_points[0][7] = Point(26 * w / 40, w / 8);
	rook_points[0][8] = Point(26 * w / 40, w / 4);
	rook_points[0][9] = Point(22 * w / 40, w / 4);
	rook_points[0][10] = Point(22 * w / 40, w / 8);
	rook_points[0][11] = Point(18 * w / 40, w / 8);
	rook_points[0][12] = Point(18 * w / 40, w / 4);
	rook_points[0][13] = Point(14 * w / 40, w / 4);
	rook_points[0][14] = Point(14 * w / 40, w / 8);
	rook_points[0][15] = Point(w / 4, w / 8);
	rook_points[0][16] = Point(w / 4, 3 * w / 8);
	rook_points[0][17] = Point(13 * w / 32, 3 * w / 8);
	rook_points[0][18] = Point(5 * w / 16, 13 * w / 16);
	rook_points[0][19] = Point(w / 4, 13 * w / 16);
	const Point* ppt[1] = { rook_points[0] };
	int npt[] = { 20 };
	fillPoly(img,
		ppt,
		npt,
		1,
		Scalar(255, 255, 255),
		lineType);
}