import cv2
import numpy as np
import json

class ArmCoordinateConverter:
    def __init__(self):
        self.calibrated = False
        self.M = None

    def calibrate(self, image_pts, real_pts):
        self.M = cv2.getPerspectiveTransform(np.float32(image_pts), np.float32(real_pts))
        self.calibrated = True
        np.save("calibration_matrix.npy", self.M)
        print("✅ Calibration complete and saved.")

    def load_calibration(self, path="calibration_matrix.npy"):
        try:
            self.M = np.load(path)
            self.calibrated = True
            print("✅ Loaded saved calibration matrix.")
        except FileNotFoundError:
            print("⚠️ No saved calibration found. Please calibrate.")

    def logical_to_arm(self, row, col):
        if not self.calibrated:
            raise Exception("Not calibrated!")
        pt = np.array([[[col, row]]], dtype='float32')
        mapped = cv2.perspectiveTransform(pt, self.M)
        return mapped[0][0][0], mapped[0][0][1]

# 全局变量
clicked_points = []

def on_mouse_click(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN and len(clicked_points) < 4:
        clicked_points.append((x, y))
        print(f"🖱️ Clicked point {len(clicked_points)}: ({x}, {y})")

def main():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("❌ Cannot open camera")
        return

    converter = ArmCoordinateConverter()
    converter.load_calibration()

    # 如果未标定，执行手动点击 + 实际坐标输入
    if not converter.calibrated:
        print("📌 Please click 4 points on the object (e.g. corners of a sheet)")
        cv2.namedWindow("Calibration")
        cv2.setMouseCallback("Calibration", on_mouse_click)

        while len(clicked_points) < 4:
            ret, frame = cap.read()
            if not ret:
                break
            for pt in clicked_points:
                cv2.circle(frame, pt, 5, (0, 0, 255), -1)
            cv2.imshow("Calibration", frame)
            if cv2.waitKey(1) & 0xFF == 27:
                print("Calibration cancelled.")
                cap.release()
                cv2.destroyAllWindows()
                return

        cv2.destroyAllWindows()
        print("\n🧭 Now enter the real-world coordinates (in mm) for each point you clicked.")
        real_pts = []
        for i in range(4):
            x = float(input(f"Real-world X for point {i+1}: "))
            y = float(input(f"Real-world Y for point {i+1}: "))
            real_pts.append((x, y))

        converter.calibrate(clicked_points, real_pts)

    # 显示图像并映射鼠标点击到机械臂坐标
    def on_click_map(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            arm_x, arm_y = converter.logical_to_arm(y, x)
            print(f"🖱️ Clicked ({x},{y}) → Arm: X={arm_x:.2f} mm, Y={arm_y:.2f} mm")

    print("\n📍 Calibration ready. Click anywhere to get arm coordinates. Press ESC to exit.")
    cv2.namedWindow("Mapped View")
    cv2.setMouseCallback("Mapped View", on_click_map)

    while True:
        ret, frame = cap.read()
        if not ret:
            break
        cv2.imshow("Mapped View", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

main()
