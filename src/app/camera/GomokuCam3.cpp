// gomoku_detector.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

class BoardDetector {
public:
    int board_size, grid_lines;
    float spacing;

    BoardDetector(int board_size = 600, int grid_lines = 13)
        : board_size(board_size), grid_lines(grid_lines) {
        spacing = static_cast<float>(board_size) / (grid_lines - 1);
    }

    vector<Point2f> orderPoints(const vector<Point>& pts) {
        vector<Point2f> rect(4);
        vector<float> s, d;
        for (const auto& p : pts) {
            s.push_back(p.x + p.y);
            d.push_back(p.x - p.y);
        }
        rect[0] = pts[min_element(s.begin(), s.end()) - s.begin()];
        rect[2] = pts[max_element(s.begin(), s.end()) - s.begin()];
        rect[1] = pts[min_element(d.begin(), d.end()) - d.begin()];
        rect[3] = pts[max_element(d.begin(), d.end()) - d.begin()];
        return rect;
    }

    Mat detectBoard(const Mat& frame) {
        Mat gray, blurImg, edges;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, blurImg, Size(7, 7), 0);
        Canny(blurImg, edges, 50, 150);

        vector<vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        sort(contours.begin(), contours.end(), [](auto& a, auto& b) {
            return contourArea(a) > contourArea(b);
        });

        if (!contours.empty()) {
            vector<Point> approx;
            approxPolyDP(contours[0], approx, arcLength(contours[0], true) * 0.02, true);
            if (approx.size() == 4) {
                vector<Point2f> src_pts = orderPoints(approx);
                vector<Point2f> dst_pts = {
                    {0, 0},
                    {(float)(board_size - 1), 0},
                    {(float)(board_size - 1), (float)(board_size - 1)},
                    {0, (float)(board_size - 1)}
                };
                Mat M = getPerspectiveTransform(src_pts, dst_pts);
                Mat warped;
                warpPerspective(frame, warped, M, Size(board_size, board_size));
                return warped;
            }
        }
        return Mat();
    }
};

class GameLogic {
public:
    int grid_lines;
    vector<vector<int>> board;

    GameLogic(int grid_lines = 13) : grid_lines(grid_lines) {
        board = vector<vector<int>>(grid_lines, vector<int>(grid_lines, 0));
    }

    void updateBoard(int row, int col, const string& color) {
        board[row][col] = (color == "black") ? 1 : 2;
    }

    int checkWinner() {
        vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
        for (int i = 0; i < grid_lines; i++) {
            for (int j = 0; j < grid_lines; j++) {
                int player = board[i][j];
                if (player == 0) continue;
                for (auto [dx, dy] : directions) {
                    int count = 1;
                    for (int step = 1; step < 5; step++) {
                        int ni = i + step * dx;
                        int nj = j + step * dy;
                        if (ni >= 0 && ni < grid_lines && nj >= 0 && nj < grid_lines && board[ni][nj] == player) {
                            count++;
                        } else break;
                    }
                    if (count == 5) return player;
                }
            }
        }
        return 0;
    }
};

string detectPieceColor(const Mat& gray, int x, int y, int r) {
    Mat mask = Mat::zeros(gray.size(), CV_8UC1);
    circle(mask, Point(x, y), int(r * 0.7), Scalar(255), -1);
    Scalar mean_val = mean(gray, mask);
    if (mean_val[0] < 80) return "black";
    if (mean_val[0] > 110) return "white";
    return "white";
}

vector<tuple<int, int, string>> detectPieces(Mat& warped_img, int grid_lines = 13, int board_size = 600) {
    float spacing = static_cast<float>(board_size) / (grid_lines - 1);
    Mat gray, blurred;
    cvtColor(warped_img, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    vector<Point> grid_points;
    for (int i = 0; i < grid_lines; i++) {
        for (int j = 0; j < grid_lines; j++) {
            int x = static_cast<int>(j * spacing);
            int y = static_cast<int>(i * spacing);
            grid_points.emplace_back(x, y);
            circle(warped_img, Point(x, y), 2, Scalar(0, 255, 0), -1);
        }
    }

    vector<Vec3f> circles;
    HoughCircles(blurred, circles, HOUGH_GRADIENT, 1.2, spacing * 0.8, 100, 18, 18, 24);
    vector<tuple<int, int, string>> pieces;

    for (auto& circle : circles) {
        int x = cvRound(circle[0]);
        int y = cvRound(circle[1]);
        int r = cvRound(circle[2]);
        Point nearest;
        float min_dist = FLT_MAX;

        for (auto& gp : grid_points) {
            float dist = pow(x - gp.x, 2) + pow(y - gp.y, 2);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = gp;
            }
        }

        if (min_dist < pow(spacing * 0.4, 2)) {
            string color = detectPieceColor(gray, x, y, r);
            int row = static_cast<int>(round(nearest.y / spacing));
            int col = static_cast<int>(round(nearest.x / spacing));
            if (row >= 0 && row < grid_lines && col >= 0 && col < grid_lines) {
                pieces.emplace_back(row, col, color);
                circle(warped_img, Point(x, y), r, Scalar(0, 0, 255), 2);
                circle(warped_img, Point(x, y), 2, Scalar(255, 0, 0), 3);
                putText(warped_img, color == "black" ? "BLACK" : "WHITE", Point(nearest.x + 5, nearest.y - 5),
                        FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
            }
        }
    }
    return pieces;
}

int main() {
    BoardDetector boardDetector;
    GameLogic gameLogic;
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cerr << "Camera not available" << endl;
        return -1;
    }

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat warped = boardDetector.detectBoard(frame);
        if (!warped.empty()) {
            auto pieces = detectPieces(warped);
            for (auto& [row, col, color] : pieces) {
                gameLogic.updateBoard(row, col, color);
            }
            int winner = gameLogic.checkWinner();
            if (winner) {
                string text = (winner == 1) ? "Winner: BLACK" : "Winner: WHITE";
                putText(warped, text, Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 2);
            }
            imshow("Warped Board", warped);
        }

        imshow("Original", frame);
        if (waitKey(1) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
