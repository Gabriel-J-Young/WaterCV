#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main(){
	bool bSuccess;

	//open default camera to test
	cout << "file to read: \'a\' for 1_min_stable" << endl << "\'b\' for 1_min_unstable" << endl << "\'a\' for 2_min" << endl;
	cout << "input a, b, or c: ";
	string file_to_read;
	cin >> file_to_read;
	cout << file_to_read << endl;
	if (file_to_read == "a") {
		file_to_read = "1_min_stable";
	}
	else if (file_to_read == "b") {
		file_to_read = "1_min_unstable";
	}
	else if (file_to_read == "c") {
		file_to_read = "2_min";
	}
	else {
		cout << "invalid char!" << endl;
	}

	VideoCapture cap("video_files//" + file_to_read + ".MOV");

	if (cap.isOpened() == false) {
		cout << "Cannot open camera" << endl;
		cin.get();
		return -1;
	}

	//finds & prints camera dimensions 
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
	cout << "Resolution is: " << dWidth << " x " << dHeight << endl;

	string out_file = file_to_read + "_out.avi";
	VideoWriter video(out_file, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(dWidth, dHeight));

	//textfiles to write to
	ofstream water_percent;
	water_percent.open("water_percent.txt");
	ofstream water_color_avg;
	water_color_avg.open("water_color_avg.txt");

	//erosion kernal
	int erosion_size = 10;
	Mat element = getStructuringElement(MORPH_CROSS,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	while (true) {
		Mat frame, hsv, big_bubble, little_bubble, water, ycb, water_color, grey;

		cap >> frame;

		if (frame.empty())
			break;

		resize(frame, frame, Size(), .5, .5);
		imshow("frame", frame);
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		//resize(hsv, hsv, Size(), .6, .6);
		imshow("frame_hsv", hsv);
		//filter only water
		inRange(hsv, Scalar(7, 0, 0), Scalar(15, 255, 100), water); //this is fair appox of water coverage
		//(good for getting the approx suface area)
		//cout << countNonZero(water)/ (dWidth*dHeight) << endl;
		imshow("water", water);
		water_percent << fixed << setprecision(8) << countNonZero(water) / ((dWidth*.5) * (dHeight*.5)) << "\n";
		//resize(water, water, Size(), .6, .6);

		//erode(hsv, hsv, element);
		//dilate(hsv, hsv, element);
		inRange(frame, Scalar(0, 0, 0), Scalar(70, 150, 250), water_color); //this is an under appox of water coverage
		//only water pixels will be in here- no red at all. this is for calculating avg color of water
		//resize(water_color, water_color, Size(), .6, .6);
		imshow("water_color", water_color);

		//cvtColor(frame, ycb, COLOR_BGR2YCrCb);
		//imshow("frame_ycd", ycb);

		
		//get channels stuff
		uint8_t* pixelPtr = (uint8_t*)frame.data;
		int cn = frame.channels();
		Scalar_<uint8_t> bgrPixel;

		//get average color of water in image
		int count = 0; //number of water pixels
		int B = 0; //total H
		int G = 0;//total S
		int R = 0; //total V

		for (int i = 0; i < frame.rows; i++) { //y
			for (int j = 0; j < frame.cols; j++) { //x
				bgrPixel.val[0] = pixelPtr[i * frame.cols * cn + j * cn + 0]; // H
				bgrPixel.val[1] = pixelPtr[i * frame.cols * cn + j * cn + 1]; // S
				bgrPixel.val[2] = pixelPtr[i * frame.cols * cn + j * cn + 2]; // V
				//if Hue of piexl in in between 7 and 15 and Value is between 0 and 100,
				//add pixel HSV values to totals for average calculation
				if (bgrPixel.val[0] < 70 && bgrPixel.val[1] < 150 && bgrPixel.val[2] < 220) {
					count += 1;
					B += bgrPixel.val[0];
					G += bgrPixel.val[1];
					R += bgrPixel.val[2];
				}
			}
		}
		

		int average_hue = round(B / (double)count);
		int average_saturation = round(G / (double)count);
		int average_value = round(R / (double)count);

		water_color_avg << average_hue << "," << average_saturation << "," << average_value << endl;

		//put anything in write
		video.write(hsv);

		//texture filtering
		cvtColor(frame, grey, COLOR_BGR2GRAY);
		//resize(grey, grey, Size(66, 96), 0, 0, INTER_NEAREST);
		imshow("grey", grey);
		grey.convertTo(grey, CV_32F);
		//resize(grey, grey, Size(), .6, .6);
		//imshow("grey", grey);

		int kernel_size = 9;
		double sig = 1, th = 0, lm = 1, gm = .25, ps = 0;
		vector<Mat> destArray;
		
		Mat kernell;
		Mat dest;
		kernell = getGaborKernel(Size(kernel_size, kernel_size), sig, th, lm, gm, ps, CV_32F);
		cout << kernell << endl;
		filter2D(grey, dest, CV_32F, kernell);

		destArray.push_back(dest);

		imshow("texture", dest);

		int keyValue = waitKey(1);
		if (keyValue == 27)
		{
			cout << "Esc key is pressed by user. Stopping the video" << endl;
			break;
		}


	}
	water_percent.close();
	water_color_avg.close();
	// When everything done, release the video capture object
	cap.release();

	// Closes all the frames
	cv::destroyAllWindows();
	//you can leave it empty
	return 0;
}