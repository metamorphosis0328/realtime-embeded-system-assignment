#include <iostream>
#include <cmath>
using namespace std;

// Constant for PI and helper to convert radians to degrees
const double PI = 3.14159265358979323846;\inline double rad2deg(double rad) { return rad * 180.0 / PI; }

// Structure to hold the three servo angles
struct ServoAngles {
    double base;     // Base rotation (yaw) servo angle in degrees
    double shoulder; // Shoulder elevation servo angle in degrees
    double elbow;    // Elbow servo angle in degrees
};

/**
 * Compute the servo angles for a given target coordinate (x, y)
 * @param x  Target X coordinate in cm (in base frame)
 * @param y  Target Y coordinate in cm (in base frame)
 * @param L1 Length of the first arm segment in cm
 * @param L2 Length of the second arm segment in cm
 * @return   ServoAngles containing base, shoulder, and elbow angles in degrees
 */
ServoAngles computeServoAngles(double x, double y, double L1, double L2) {
    ServoAngles ang;
    // 1) Compute the base yaw angle to point toward (x, y)
    double baseRad = atan2(y, x);

    // 2) Compute planar distance from arm base to target point
    double d = sqrt(x*x + y*y);
    if (d > L1 + L2) {
        cerr << "Error: target is out of reach! (d = " << d << " cm)" << endl;
        // Clamp to maximum reach
        d = L1 + L2;
    }

    // 3) Use law of cosines to compute link angles
    // angle_b: between first link and line from base to target\    
    double angle_b = acos((L1*L1 + d*d - L2*L2) / (2.0 * L1 * d));
    // angle_c: internal elbow angle between link1 and link2
    double angle_c = acos((L1*L1 + L2*L2 - d*d) / (2.0 * L1 * L2));

    // 4) Convert radian results to degrees
    ang.base     = rad2deg(baseRad);
    ang.shoulder = rad2deg(angle_b);
    // Elbow angle defined as the external opening angle = PI - angle_c
    ang.elbow    = rad2deg(PI - angle_c);

    return ang;
}

int main() {
    // Example: target from vision system (e.g., column=6, row=5 -> x=6*2cm, y=5*2cm)
    double x = 6 * 2.0;
    double y = 5 * 2.0;

    // Set your actual arm segment lengths here
    double L1 = 10.0;  // First link length in cm
    double L2 = 10.0;  // Second link length in cm

    // Compute the servo angles
    ServoAngles ang = computeServoAngles(x, y, L1, L2);

    // Output results
    cout << "Target (x, y) = (" << x << " cm, " << y << " cm)" << endl;
    cout << "Servo angles -> base: " << ang.base 
         << "бу, shoulder: " << ang.shoulder 
         << "бу, elbow: " << ang.elbow << "бу" << endl;
    return 0;
}

