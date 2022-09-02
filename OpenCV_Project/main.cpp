#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(void)
{
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		cerr << "Camera open failed!" << endl;
		return 0;
	}

	cout << "Frame width: " << cvRound(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
	cout << "Frame height: " << cvRound(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;

	int count = 0;

	Mat frame;
	Mat current;
	Mat previous;
	Mat diff;
	Mat dilation;
	Mat bin;
	Mat result;

	vector<vector<Point>> contours;

	while (true) {
		cap >> frame;

		if (frame.empty()) {
			break;
		}

		if (waitKey(10) == 27) {
			break;
		}

		count++;

		result = frame.clone();

		if ((count % 2) == 0) {
			cvtColor(frame, current, COLOR_BGR2GRAY);
			GaussianBlur(current, current, Size(5, 5), 0);
		}
		else {
			cvtColor(frame, previous, COLOR_BGR2GRAY);
			GaussianBlur(previous, previous, Size(5, 5), 0);

			continue;
		}

		absdiff(previous, current, diff);
		imshow("diff", diff);

		// 모폴로지 팽창 연산
		// 일부 노이즈 제거
		dilate(diff, dilation, Mat::ones(Size(5, 5), CV_8UC1));

		// OTSU method를 통해 이진화
		threshold(dilation, bin, 0, 255, THRESH_OTSU);

		findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		for (vector<Point> contour : contours) {
			// threshold
			// 검출한 contours를 최소 영역의 직사각형으로 감쌌을 때의 영역이 100 이하라면 노이즈로 보고 무시
			if (contourArea(contour) < 100) {
				continue;
			}

			Rect rect = boundingRect(contour);

			Mat roi = frame(Rect(Point(rect.x, rect.y),
				Point(rect.x + rect.width, rect.y + rect.height)));
			// imshow("roi", roi);

			// 16 프레임마다 검출된 ROI를 저장
			// 테스트 시 경로 수정
			if ((count % 16) == 0) {
				imwrite(format("C:/Users/hisn16.DESKTOP-HGVGADP/source/repos/OpenCV_Project/OpenCV_Project/ROI/%dth_ROI.png", count), roi);
			}

			rectangle(result,
				Point(rect.x, rect.y),
				Point(rect.x + rect.width, rect.y + rect.height),
				Scalar(0, 255, 0),
				1);
		}
		imshow("result", result);
	}

	destroyAllWindows();

	return 0;
}