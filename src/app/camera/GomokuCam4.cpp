#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

// Configuration
const int GRID_SIZE = 13;
const int BOARD_PIXEL_SIZE = 600;
const float GRID_PIXEL_SPACING = BOARD_PIXEL_SIZE / (float)(GRID_SIZE - 1);
const float GRID_MM_SPACING = 15.0f;
const Point2f WORLD_ORIGIN(100, 100); // mm

// Order 4 corner points (top-left, top-right, bottom-right, bottom-left)
vector<Point2f> orderPoints(vector<Point> pts) {
    vector<Point2f> rect(4);
    vector<Point2f> fp(4);
    for (int i = 0; i < 4; ++i) fp[i] = Point2f(pts[i].x, pts[i].y);

    float s[4], d[4];
    for (int i = 0; i < 4; ++i) {
        s[i] = fp[i].x + fp[i].y;
        d[i] = fp[i].x - fp[i].y;
    }

    rect[0] = fp[min_element(s, s + 4) - s]; // top-left
    rect[2] = fp[max_element(s, s + 4) - s]; // bottom-right
    rect[1] = fp[min_element(d, d + 4) - d]; // top-right
    rect[3] = fp[max_element(d, d + 4) - d]; // bottom-left
    return rect;
}

// Simple brightness-based piece color detection
string detectPieceColor(Mat& gray, int x, int y, int r) {
    Mat mask = Mat::zeros(gray.size(), CV_8UC1);
    circle(mask, Point(x, y), int(r * 0.7), Scalar(255), -1);
    Scalar meanVal = mean(gray, mask);
    if (meanVal[0] < 80) return "black";
    if (meanVal[0] > 130) return "white";
    return "white";
}

// Convert grid index to real-world coordinates (mm)
Point2f gridToWorld(int row, int col) {
    return Point2f(
        WORLD_ORIGIN.x + col * GRID_MM_SPACING,
        WORLD_ORIGIN.y + row * GRID_MM_SPACING
    );
}

// AI move (simple heuristic)
Point getAIMove(vector<vector<int>>& board) {
    int bestScore = -1;
    Point bestMove(-1, -1);

    auto scoreLine = [](vector<int> line) {
        int count2 = count(line.begin(), line.end(), 2);
        int count0 = count(line.begin(), line.end(), 0);
        int count1 = count(line.begin(), line.end(), 1);
        if (count2 == 5) return 100000;
        if (count2 == 4 && count0 == 1) return 1000;
        if (count2 == 3 && count0 == 2) return 100;
        if (count1 == 4 && count0 == 1) return 900;
        if (count1 == 3 && count0 == 2) return 90;
        return 0;
    };

    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 13; ++j) {
            if (board[i][j] != 0) continue;
            int score = 0;
            for (auto [dx, dy] : vector<pair<int, int>>{{1,0},{0,1},{1,1},{1,-1}}) {
                vector<int> line;
                for (int k = -2; k <= 2; ++k) {
                    int ni = i + k * dx, nj = j + k * dy;
                    if (ni >= 0 && ni < 13 && nj >= 0 && nj < 13) {
                        line.push_back(board[ni][nj]);
                    } else {
                        line.push_back(-1);
                    }
                }
                score += scoreLine(line);
            }
            if (score > bestScore) {
                bestScore = score;
                bestMove = Point(j, i); // (col, row)
            }
        }
    }
    return bestMove;
}

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Failed to open camera." << endl;
        return -1;
    }

    string turn = "black";

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat gray, blurred, edges;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, blurred, Size(7, 7), 0);
        Canny(blurred, edges, 50, 150);

        vector<vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        sort(contours.begin(), contours.end(), [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) > contourArea(b);
        });

        if (!contours.empty()) {
            vector<Point> approx;
            approxPolyDP(contours[0], approx, arcLength(contours[0], true) * 0.02, true);

            if (approx.size() == 4) {
                vector<Point2f> src_pts = orderPoints(approx);
                vector<Point2f> dst_pts = {
                    Point2f(0, 0), Point2f(BOARD_PIXEL_SIZE - 1, 0),
                    Point2f(BOARD_PIXEL_SIZE - 1, BOARD_PIXEL_SIZE - 1),
                    Point2f(0, BOARD_PIXEL_SIZE - 1)
                };

                Mat M = getPerspectiveTransform(src_pts, dst_pts);
                Mat warped;
                warpPerspective(frame, warped, M, Size(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));

                vector<vector<int>> board(13, vector<int>(13, 0));
                Mat grayWarped;
                cvtColor(warped, grayWarped, COLOR_BGR2GRAY);
                GaussianBlur(grayWarped, blurred, Size(5, 5), 0);

                vector<Vec3f> circles;
                HoughCircles(blurred, circles, HOUGH_GRADIENT, 1.2, GRID_PIXEL_SPACING * 0.8,
                             100, 18, 18, 24);

                for (auto c : circles) {
                    int x = cvRound(c[0]), y = cvRound(c[1]), r = cvRound(c[2]);

                    int row = round(y / GRID_PIXEL_SPACING);
                    int col = round(x / GRID_PIXEL_SPACING);

                    if (row >= 0 && row < 13 && col >= 0 && col < 13) {
                        string color = detectPieceColor(grayWarped, x, y, r);
                        board[row][col] = (color == "black") ? 1 : 2;
                        circle(warped, Point(x, y), r, Scalar(0, 0, 255), 2);
                    }
                }

                if (turn == "white") {
                    Point aiMove = getAIMove(board);
                    if (aiMove.x != -1) {
                        Point2f worldPos = gridToWorld(aiMove.y, aiMove.x);
                        cout << "[AI] White move at row=" << aiMove.y
                             << " col=" << aiMove.x
                             << " → World(mm): " << worldPos << endl;

                        // TODO: robotic arm control function here
                        turn = "black";
                    }
                }

                imshow("Warped Board", warped);
            }
        }

        imshow("Original", frame);
        if (waitKey(1) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
