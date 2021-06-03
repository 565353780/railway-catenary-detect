import cv2
import os
import numpy as np
from math import sqrt


class DataCalibration:
    def __init__(self):
        self.rectangle = []
        self.rects = []
        self.picture_list = []
        self.max_pixel_num = 200000
        self.resize_scale = 1
        self.current_img_name = ''
        self.image = None
        self.image_copy = None
        self.width_ = -1
        self.height_ = -1
        if not os.path.exists(os.getcwd() + '/../Server_DataBase/train_dataset'):
            os.mkdir(os.getcwd() + '/../Server_DataBase/train_dataset')

    def get_listname(self, path=os.getcwd()):
        file_list = os.listdir(path)
        for i in file_list:
            if i[-3:] == "jpg":
                self.picture_list.append(i)
        return self.picture_list

    def load_image(self, image_name):
        self.current_img_name = image_name
        self.image = cv2.imread(image_name)
        self.height_, self.width_, _ = self.image.shape
        if self.height_ * self.width_ > self.max_pixel_num:
            self.resize_scale = sqrt(datacalibration.max_pixel_num / self.height_ / self.width_)
            self.image = cv2.resize(self.image,
                                    (int(self.width_ * self.resize_scale), int(self.height_ * self.resize_scale)))
            self.height_, self.width_, _ = self.image.shape
        self.image_copy = self.image.copy()

    def draw_circle(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            cv2.circle(self.image, (x, y), 1, (0, 0, 255), -1)
            if len(self.rectangle) < 8:
                self.rectangle.append(x)
                self.rectangle.append(y)
            if len(self.rectangle) == 8:
                rect = []
                for i in range(4):
                    point = [int(self.rectangle[2 * i]), int(self.rectangle[2 * i + 1])]
                    rect.append(point)
                self.rects.append(rect)
                for i in range(4):
                    cv2.line(self.image, tuple(rect[i]), tuple(rect[(i + 1) % 4]), color=(255, 255, 255),
                             thickness=2)
                cv2.imshow(self.current_img_name, self.image)
                self.rectangle = []

    def pop_rects(self):
        self.rects = self.rects[:-1]
        self.image = self.image_copy.copy()
        for rect in self.rects:
            for i in range(4):
                cv2.line(self.image, tuple(rect[i]), tuple(rect[(i + 1) % 4]), color=(255, 255, 255),
                         thickness=2)
        cv2.imshow(self.current_img_name, self.image)

    def generatePNG(self):
        image = np.zeros((self.height_, self.width_, 3), np.uint8)

        for rect in self.rects:
            for i in range(4):
                cv2.line(image, tuple(rect[i]), tuple(rect[(i + 1) % 4]), color=(255, 255, 255),
                         thickness=2)

        rect, image = cv2.threshold(image, 20, 255, cv2.THRESH_BINARY)
        image = cv2.resize(image, (self.width_, self.height_))
        cv2.imwrite(os.getcwd() + '/../Server_DataBase/train_dataset/' + self.current_img_name, self.image_copy)
        cv2.imwrite(os.getcwd() + '/../Server_DataBase/train_dataset/' + self.current_img_name[:-3] + "png", image)
        self.rects = []
        self.rectangle = []


if __name__ == '__main__':
    datacalibration = DataCalibration()
    pic_list = datacalibration.get_listname()

    for picture_name in pic_list:
        datacalibration.load_image(picture_name)

        cv2.namedWindow(picture_name)
        cv2.setMouseCallback(picture_name, datacalibration.draw_circle)

        while True:
            cv2.imshow(picture_name, datacalibration.image)
            key = cv2.waitKey(50) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('z') and len(datacalibration.rects) > 0:
                datacalibration.pop_rects()

        datacalibration.generatePNG()
        cv2.destroyAllWindows()
