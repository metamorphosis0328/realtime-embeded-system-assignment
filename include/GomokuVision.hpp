#ifndef GOMOKUVISION_HPP
#define GOMOKUVISION_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <tuple>

class PieceEventCallback {
public:
    virtual void onNewPieceDetected(int row, int col, const std::string& color) = 0;
    virtual ~PieceEventCallback() = default;
};

class GomokuVision {
public:
    GomokuVision(int camera_id = 0, int board_size = 600, int grid_lines = 9);
    void registerCallback(PieceEventCallback* cb);
    void run();

private:
    int board_size;
    int grid_lines;
    float spacing;
    cv::VideoCapture cap;
    std::vector<PieceEventCallback*> callbacks;
    std::vector<std::vector<int>> board_state; // 0=empty, 1=black, 2=white

    cv::Mat detectBoard(const cv::Mat& frame);
    std::vector<cv::Point2f> orderPoints(const std::vector<cv::Point>& pts);
    std::vector<std::tuple<int, int, std::string>> detectPieces(cv::Mat& warped_img);
    std::string detectPieceColor(const cv::Mat& bgr, int x, int y, int r);

    struct PieceCandidate {
        int frames_confirmed;
        std::string color;
    };

    std::map<std::pair<int, int>, PieceCandidate> piece_candidates;
    const int FRAME_THRESHOLD = 5;  // 连续检测帧数增加到5帧，降低误报率
};

#endif