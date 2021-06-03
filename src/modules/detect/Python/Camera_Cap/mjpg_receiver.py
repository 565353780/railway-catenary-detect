import cv2
import requests
import numpy as np
import time
import os

class MJPG_Receiver:
    def __init__(self, ip, port, camera_idx, cap_time, cap_video_path):
        self.ip = ip
        self.port = port
        self.camera_idx = camera_idx
        self.cap_time = cap_time
        self.cap_video_path = os.path.abspath(cap_video_path)

        self.url_path = 'http://' + self.ip + ':' + self.port + '/stream.mjpg'

        if not os.path.exists(self.cap_video_path):
            os.makedirs(self.cap_video_path)
    
    def local_index(self):
        localtime = time.localtime(time.time())
        time_idx = str(localtime.tm_year) + '_' \
                + str(localtime.tm_mon) + '_' \
                + str(localtime.tm_mday) + '_' \
                + str(localtime.tm_hour) + '_' \
                + str(localtime.tm_min) + '_' \
                + str(localtime.tm_sec)
        return time_idx
    
    def get_url_msg(self):
        req = requests.get(self.url_path)
        obj = req.content.decode('utf-8')

        # img_width = int(obj.split('width=\"')[1].split('\"')[0])
        # img_height = int(obj.split('height=\"')[1].split('\"')[0])

        print(obj)
        
    def receive_mjpg(self):
        
        file_title = self.cap_video_path + "/Camera" + str(self.camera_idx) + "_"

        cap = cv2.VideoCapture(self.url_path)

        img_width = int(cap.get(3))
        img_height = int(cap.get(4))
        fps = cap.get(5)

        if img_width == 0:
            print("Reading web camera failed.System is stopped.Please try again.")
            with open(os.getcwd() + "/../Server_DataBase/message_set/detect_people_img_stopped.txt", "w") as f:
                f.write("True")
            if os.path.exists(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt"):
                os.remove(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt")
            return

        print("camera connected.")

        fourcc = cv2.VideoWriter_fourcc('M','P','4','2')

        videowriter = cv2.VideoWriter(file_title + self.local_index() + ".avi", fourcc, fps, (img_width, img_height))
        
        time_start = time.time()

        while True:
            if os.path.exists(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt"):
                os.remove(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt")
                return
            ret, frame = cap.read()
            if ret == True:
                # cv2.imshow('Video', frame)
                if time.time() - time_start < self.cap_time:
                    videowriter.write(frame)
                else:
                    videowriter.release()
                    videowriter = cv2.VideoWriter(file_title + self.local_index() + ".avi", fourcc, fps, (img_width, img_height))
                    time_start = time.time()

                if os.path.exists(os.getcwd() + '/../Server_DataBase/message_set/detect_people_img_updated.txt'):
                    if os.path.exists(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt"):
                        os.remove(os.getcwd() + '/../Server_DataBase/message_set/detect_people_img_updated.txt')
                        os.remove(os.getcwd() + "/../Server_DataBase/message_set/detect_people_camera_stopped.txt")
                        return
                    continue
                
                cv2.imwrite('../Server_DataBase/message_set/detect_people.jpg', frame)

                with open('../Server_DataBase/message_set/detect_people_img_updated.txt', 'w') as f:
                    f.write("True")

                if cv2.waitKey(1) == 27:
                        exit(0)

    def receive_socket(self):
        r = requests.get(self.url_path, auth=('user', 'password'), stream=True)
        if(r.status_code == 200):
            bytes = bytes()
            for chunk in r.iter_content(chunk_size=1024):
                bytes += chunk
                a = bytes.find(b'\xff\xd8')
                b = bytes.find(b'\xff\xd9')
                if a != -1 and b != -1:
                    jpg = bytes[a:b+2]
                    bytes = bytes[b+2:]
                    i = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                    cv2.imshow('i', i)
                    if cv2.waitKey(1) == 27:
                        exit(0)
        else:
            print("Received unexpected status code {}".format(r.status_code))

if __name__ == "__main__":
    mjpg_receiver = MJPG_Receiver('192.168.0.203', '8085', 0, 300, '../Cap_Video')
    mjpg_receiver.receive_mjpg()
