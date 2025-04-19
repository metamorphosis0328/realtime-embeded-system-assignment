#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 棋盘角点检测（基于轮廓）
vector<Point2f> detect_board_corners_by_contour(const Mat& frame) {
    Mat gray, blurImg, edges;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurImg, Size(5, 5), 0);
    Canny(blurImg, edges, 50, 150);

    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), [](const vector<Point>& a, const vector<Point>& b) {
        return contourArea(a) > contourArea(b);
    });

    for (const auto& cnt : contours) {
        vector<Point> approx;
        approxPolyDP(cnt, approx, 0.02 * arcLength(cnt, true), true);
        if (approx.size() == 4) {
            vector<Point2f> corners;
            for (const Point& p : approx)
                corners.push_back(p);
            return corners;
        }
    }
    return {};
}

// 排序角点（左上、右上、右下、左下）
vector<Point2f> order_points(const vector<Point2f>& pts) {
    vector<Point2f> ordered(4);
    vector<float> sum, diff;
    for (const auto& pt : pts) {
        sum.push_back(pt.x + pt.y);
        diff.push_back(pt.y - pt.x);
    }
    ordered[0] = pts[min_element(sum.begin(), sum.end()) - sum.begin()]; // 左上
    ordered[1] = pts[min_element(diff.begin(), diff.end()) - diff.begin()]; // 右上
    ordered[2] = pts[max_element(sum.begin(), sum.end()) - sum.begin()]; // 右下
    ordered[3] = pts[max_element(diff.begin(), diff.end()) - diff.begin()]; // 左下
    return ordered;
}

// 透视变换拉直
Mat warp_board(const Mat& frame, const vector<Point2f>& corners, int size = 480) {
    vector<Point2f> src = order_points(corners);
    vector<Point2f> dst = { {0,0}, {float(size),0}, {float(size),float(size)}, {0,float(size)} };
    Mat M = getPerspectiveTransform(src, dst);
    Mat warped;
    warpPerspective(frame, warped, M, Size(size, size));
    return warped;
}

// 灰度均值识别棋盘状态
vector<vector<int>> extract_board_state_gray_based(const Mat& warped, int board_size = 12) {
    Mat gray;
    cvtColor(warped, gray, COLOR_BGR2GRAY);

    int h = gray.rows, w = gray.cols;
    int cell_h = h / board_size, cell_w = w / board_size;

    vector<vector<int>> board(board_size, vector<int>(board_size, 0));

    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            Rect roi(j * cell_w + 5, i * cell_h + 5, cell_w - 10, cell_h - 10);
            Mat cell = gray(roi);
            Scalar avg = mean(cell);

            if (avg[0] < 90)
                board[i][j] = 1;  // 黑子
            else if (avg[0] > 230)
                board[i][j] = 2;  // 白子
            else
                board[i][j] = 0;  // 空格
        }
    }
    return board;
}

// 主程序
int main() {
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cout << "摄像头无法打开" << endl;
        return -1;
    }

    cout << "按 q 退出程序" << endl;

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        vector<Point2f> corners = detect_board_corners_by_contour(frame);
        if (corners.size() == 4) {
            Mat warped = warp_board(frame, corners);
            auto board = extract_board_state_gray_based(warped);

            int cell_size = warped.rows / 12;
            for (int i = 0; i < 12; ++i) {
                for (int j = 0; j < 12; ++j) {
                    if (board[i][j] == 0) continue;

                    Point center(j * cell_size + cell_size / 2, i * cell_size + cell_size / 2);
                    Scalar color = (board[i][j] == 1) ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
                    circle(warped, center, 10, color, 2);
                }
            }
            imshow("Warped Board + Detected Pieces", warped);

            // 显示角点
            for (const auto& pt : corners) {
                circle(frame, pt, 8, Scalar(0, 255, 0), -1);
            }
        }

        imshow("Original", frame);
        if ((char)waitKey(1) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
