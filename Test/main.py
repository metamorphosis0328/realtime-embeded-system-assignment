import cv2
import numpy as np

class ArmCoordinateConverter:
    def __init__(self):
        self.calibrated = False

    def calibrate(self, board_pts, real_pts):
        self.M = cv2.getPerspectiveTransform(np.float32(board_pts), np.float32(real_pts))
        self.calibrated = True

    def logical_to_arm(self, row, col):
        if not self.calibrated:
            raise Exception("Not calibrated!")
        pt = np.array([[[col, row]]], dtype='float32')
        mapped = cv2.perspectiveTransform(pt, self.M)
        return mapped[0][0][0], mapped[0][0][1]

def detect_chessboard(img, pattern_size=(12, 12)):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, pattern_size)
    if not ret:
        return None, None
    corners = corners.reshape(-1, 2)
    board_pts = [
        corners[0],
        corners[pattern_size[0]-1],
        corners[-1],
        corners[-pattern_size[0]]
    ]
    return corners, board_pts

def move_robot_to(x, y):
    print(f"Moving robot to ({x:.1f}, {y:.1f})")
    input("Press Enter when ready (simulation)")

def get_current_arm_position():
    x = float(input("Enter current arm X position (mm): "))
    y = float(input("Enter current arm Y position (mm): "))
    return x, y

def move_arm_and_get_position(image_corner):
    x_pixel, y_pixel = image_corner
    print(f"Image corner pixel position: {x_pixel:.1f}, {y_pixel:.1f}")
    move_robot_to(x_pixel, y_pixel)
    x_mm, y_mm = get_current_arm_position()
    return (x_mm, y_mm)

def main():
    cap = cv2.VideoCapture(0)
    converter = ArmCoordinateConverter()
    pattern_size = (12, 12)
    board_pts = None
    real_pts = []

    print("Waiting for chessboard detection...")

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        corners, detected_pts = detect_chessboard(frame, pattern_size)

        if corners is not None:
            frame = cv2.drawChessboardCorners(frame, pattern_size, corners, True)
            if board_pts is None and len(detected_pts) == 4:
                board_pts = detected_pts
                print("Chessboard corners detected! Press 'c' to calibrate.")

        cv2.imshow("Chessboard Detection", frame)
        key = cv2.waitKey(1) & 0xFF

        if key == ord('c') and board_pts is not None:
            print("Starting calibration...")
            for i in range(4):
                pos = move_arm_and_get_position(board_pts[i])
                real_pts.append(pos)
            converter.calibrate(board_pts, real_pts)
            print("Calibration complete! Click to get mapped coordinates.")

        elif key == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

    if converter.calibrated:
        def on_mouse(event, x, y, flags, param):
            if event == cv2.EVENT_LBUTTONDOWN:
                arm_x, arm_y = converter.logical_to_arm(y, x)
                print(f"Clicked pixel ({x},{y}) → Arm position: X={arm_x:.2f} mm, Y={arm_y:.2f} mm")

        cap = cv2.VideoCapture(0)
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            cv2.imshow("Click to Map to Arm", frame)
            cv2.setMouseCallback("Click to Map to Arm", on_mouse)
            if cv2.waitKey(1) == 27:
                break
        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
