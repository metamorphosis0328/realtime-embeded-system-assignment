#include "GomokuVision.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

GomokuVision::GomokuVision(int camera_id, int board_size, int grid_lines)
    : board_size(board_size), grid_lines(grid_lines), spacing((float)board_size / (grid_lines - 1)) {
    cap.open(camera_id);
    if (!cap.isOpened()) {
        throw runtime_error("[Error] Failed to open camera");
    }
    board_state = vector<vector<int>>(grid_lines, vector<int>(grid_lines, 0));
}

GomokuVision::~GomokuVision()
{
    std::cout << "[Vision] Cleaning up camera resources...\n";
    if (cap.isOpened())
    {
        cap.release();
    }
    destroyAllWindows();
}

void GomokuVision::registerCallback(PieceEventCallback* cb) {
    callbacks.push_back(cb);
}

void GomokuVision::run() {
    int frame_count = 0;
    const int FRAME_SKIP = 2;

    while (true) {
                Mat frame;
        cap >> frame;
if (frame.empty()) continue;

        Mat warped = detectBoard(frame);
        if (warped.empty()) continue;
        
        frame_count++;
        if (frame_count % FRAME_SKIP != 0) continue;

                auto pieces = detectPieces(warped);
        std::set<std::pair<int, int>> current_detected;

        for (auto& [row, col, color] : pieces) {
            if (color == "none") continue;

            auto key = std::make_pair(row, col);
            current_detected.insert(key);

            auto& candidate = piece_candidates[key];

            if (candidate.color == color) {
                candidate.frames_confirmed++;
            } else {
                candidate.color = color;
                candidate.frames_confirmed = 1;
            }

            if (candidate.frames_confirmed >= FRAME_THRESHOLD && board_state[row][col] == 0) {
                board_state[row][col] = (color == "black") ? 1 : 2;
                cout << "Confirmed new piece: " << color << " at (" << row << ", " << col << ")" << endl;
                for (auto& cb : callbacks) {
                    cb->onNewPieceDetected(row, col, color);
                }
            }
        }


        for (auto it = piece_candidates.begin(); it != piece_candidates.end();) {
            if (current_detected.find(it->first) == current_detected.end()) {
                it = piece_candidates.erase(it);
            } else {
                ++it;
            }
        }

        // Debugging code
        // imshow("Warped Board", warped);
        // if (waitKey(10) == 27) break;
    }
    cap.release();
    destroyAllWindows();
}

Mat GomokuVision::detectBoard(const Mat& frame) {
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

vector<Point2f> GomokuVision::orderPoints(const vector<Point>& pts) {
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

vector<tuple<int, int, string>> GomokuVision::detectPieces(Mat& warped_img) {
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

    vector<Vec3f> hough_circles;
    HoughCircles(blurred, hough_circles, HOUGH_GRADIENT, 1.2, spacing * 0.4, 100, 20, 18, 24);
    vector<tuple<int, int, string>> pieces;

    for (auto& hough_circle : hough_circles) {
        int x = cvRound(hough_circle[0]);
        int y = cvRound(hough_circle[1]);
        int r = cvRound(hough_circle[2]);
        Point nearest;
        float min_dist = FLT_MAX;

        for (auto& gp : grid_points) {
            float dist = pow(x - gp.x, 2) + pow(y - gp.y, 2);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = gp;
            }
        }

        if (min_dist < pow(spacing * 0.25, 2)) {
            string color = detectPieceColor(warped_img, x, y, r);
            int row = static_cast<int>(round(nearest.y / spacing));
            int col = static_cast<int>(round(nearest.x / spacing));

            // Map camera coordinate to robot coordinate (reverse direction)
            int mapped_row = grid_lines - 1 - row;
            int mapped_col = grid_lines - 1 - col;

            if (mapped_row >= 0 && mapped_row < grid_lines &&
                mapped_col >= 0 && mapped_col < grid_lines && color != "none") {
                pieces.emplace_back(mapped_row, mapped_col, color);
                Scalar drawColor = (color == "black") ? Scalar(0, 0, 0) : Scalar(255, 255, 255);
                circle(warped_img, Point(x, y), r, drawColor, 2);
                putText(warped_img, color, Point(nearest.x + 5, nearest.y - 5), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 255, 255), 1);
            }
        }
    }
    return pieces;
}

std::string GomokuVision::detectPieceColor(const cv::Mat& bgr, int x, int y, int r) {
    Mat gray;
    cvtColor(bgr, gray, COLOR_BGR2GRAY);
    Mat mask = Mat::zeros(gray.size(), CV_8UC1);
    circle(mask, Point(x, y), int(r * 0.7), Scalar(255), -1);
    Scalar mean_val = mean(gray, mask);
    float intensity = mean_val[0];
    if (intensity < 90) return "black";
    if (intensity > 110) return "white";
    return "none";
}
