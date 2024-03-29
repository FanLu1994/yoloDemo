
#include "pch.h"
#include <iostream>

#ifdef _WIN32
#define OPENCV
#define GPU
#endif

#include "yolo_v2_class.hpp"	// imported functions from DLL
#include <opencv2/opencv.hpp>	// C++
#include "opencv2/highgui/highgui.hpp"  

//#pragma comment(lib, "opencv_world340.lib")//引入链接库

void draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names,
	int current_det_fps = -1, int current_cap_fps = -1)
{
	int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
	for (auto &i : result_vec) {
		cv::Scalar color = obj_id_to_color(i.obj_id);
		cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);
		if (obj_names.size() > i.obj_id) {
			std::string obj_name = obj_names[i.obj_id];
			if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
			cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
			int const max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
			cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 30, 0)),
				cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
				color, CV_FILLED, 8, 0);
			putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
		}
	}
}

std::vector<std::string> objects_names_from_file(std::string const filename) {
	std::ifstream file(filename);
	std::vector<std::string> file_lines;
	if (!file.is_open()) return file_lines;
	for (std::string line; getline(file, line);) file_lines.push_back(line);
	std::cout << "object names loaded \n";
	return file_lines;
}

int main()
{
	std::string  names_file = "data/9k.names";
	std::string  cfg_file = "data/yolo9000.cfg";
	std::string  weights_file = "data/yolo9000.weights";
	Detector detector(cfg_file, weights_file);//初始化检测器
	auto obj_names = objects_names_from_file(names_file);//获得分类对象名称

	cv::VideoCapture capture;
	//capture.open("D://迅雷下载//test.mp4");
	capture.open(0);
	if (!capture.isOpened())
	{
		printf("文件打开失败");
	}
	//获取整个帧数  
	cv::Mat frame;
	while(1)
	{
		capture >> frame;
		std::vector<bbox_t> result_vec = detector.detect(frame);
		draw_boxes(frame, result_vec, obj_names);
		cv::imshow("window name1", frame);
		cv::waitKey(3);
	}
	return 0;
}
