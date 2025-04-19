#include <opencv2/opencv.hpp>
#include <vector>
#include <stdexcept>

class ArmCoordinateConverter {
public:
    ArmCoordinateConverter(int gridLines = 13)
        : gridLines(gridLines), calibrated(false) {}

    // Calibrate with 4 logical board points and 4 corresponding robot arm points
    void calibrate(const std::vector<cv::Point2f>& boardPts, const std::vector<cv::Point2f>& realPts) {
        if (boardPts.size() != 4 || realPts.size() != 4) {
            throw std::runtime_error("Need exactly 4 points for calibration.");
        }
        M = cv::getPerspectiveTransform(boardPts, realPts);
        calibrated = true;
    }

    // Convert logical board coordinates (row, col) to robot arm coordinates (X, Y)
    cv::Point2f logicalToArm(int row, int col) const {
        if (!calibrated) {
            throw std::runtime_error("Converter not calibrated!");
        }
        std::vector<cv::Point2f> inputPts = { cv::Point2f(static_cast<float>(col), static_cast<float>(row)) };
        std::vector<cv::Point2f> outputPts;
        cv::perspectiveTransform(inputPts, outputPts, M);
        return outputPts[0];
    }

private:
    int gridLines;
    bool calibrated;
    cv::Mat M;  // 3x3 perspective transform matrix
};
