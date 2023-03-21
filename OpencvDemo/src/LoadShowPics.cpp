#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>


using namespace std;



int main(){
	cv::Mat img = cv::imread("../LinuxIndex.png");
	if(img.empty()){
		cout<<"error occur,please check out the exists of picture"<<endl;
		return -1;
	}
	
	cv::imshow("TheImg",img);
	cv::waitKey(0);
	return 0;
}
