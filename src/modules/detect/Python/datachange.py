import os.path as osp
import os
import json as js
import numpy as np
from shutil import copyfile
from PIL import Image
import shutil
import xml.etree.ElementTree as ET
from os import getcwd

dataset_path = os.getcwd() + '/../Server_DataBase/train_dataset/darknet_dataset'

# my_labels = ['person', 'rock', 'broom']
# my_labels = ["Sprite", "Coca-Cola", "Fanta_Orange", "Fanta_Apple", "Gatorade_Orange", "Gatorade_Huang", "Gatorade_Blue",
#            "Qin lemon water", "Beauty Juice Orange", "Honey Citron Tea", "Sour plum soup", "Unified Green Tea",
#             "Unified Ice Black Tea", "Oolong Tea", "Lemon U grid", "Jasmine Honey Tea", "Jasmine Tea",
#             "Master Green Tea", "Master Kong Ice Black Tea", "Ome Green Tea", "Osmanthus Sour Plum Soup Drink",
#             "Unification of fresh oranges"]

sets = [('2012', 'train')]

class DataChange:

    def __init__(self, rootpath, source_json_path, source_img_path, my_labels=['0'], use_my_labels=False,
                 need_to_change_image_size=False, image_width=300, image_height=300):
        self.rootpath = rootpath
        self.source_json_path = source_json_path
        self.source_img_path = source_img_path
        self.use_my_labels = use_my_labels
        self.need_to_change_image_size = need_to_change_image_size
        self.image_width = image_width
        self.image_height = image_height
        self.my_labels = my_labels
        self.classes = my_labels.copy()

    def convert(self, size, box):
        dw = 1. / (size[0])
        dh = 1. / (size[1])
        x = (box[0] + box[1]) / 2.0 - 1
        y = (box[2] + box[3]) / 2.0 - 1
        w = box[1] - box[0]
        h = box[3] - box[2]
        x = x * dw
        w = w * dw
        y = y * dh
        h = h * dh
        return (x, y, w, h)


    def convert_annotation(self, year, image_id):
        in_file = open(dataset_path + '/Annotations_/%s.xml' % (image_id))
        out_file = open(dataset_path + '/labels/val2014/%s.txt' % (image_id), 'w')
        tree = ET.parse(in_file)
        root = tree.getroot()
        size = root.find('size')
        w = int(size.find('width').text)
        h = int(size.find('height').text)

        for obj in root.iter('object'):
            difficult = obj.find('difficult').text
            cls = obj.find('name').text
            if cls not in self.classes or int(difficult) == 1:
                continue
            cls_id = self.classes.index(cls)
            xmlbox = obj.find('bndbox')
            b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text),
                 float(xmlbox.find('ymax').text))
            bb = self.convert((w, h), b)
            out_file.write(str(cls_id) + " " + " ".join([str(a) for a in bb]) + '\n')


    def readxml(self, dataset, xml, count, file_name):
        tree = ET.parse(xml)
        root = tree.getroot()
        for child in root:
            if child.tag == "size":
                for s_ch in child:
                    if s_ch.tag == "width":
                        w = s_ch.text
                    elif s_ch.tag == "height":
                        h = s_ch.text
            elif child.tag == "object":
                for s_ch in child:
                    if s_ch.tag == "bndbox":
                        for ss_ch in s_ch:
                            if ss_ch.tag == "xmin":
                                xmin = ss_ch.text
                            elif ss_ch.tag == "ymin":
                                ymin = ss_ch.text
                            elif ss_ch.tag == "xmax":
                                xmax = ss_ch.text
                            elif ss_ch.tag == "ymax":
                                ymax = ss_ch.text
                    else:
                        ca_name = s_ch.text

        dataset.setdefault("images", []).append({
            'file_name': file_name,
            'id': int(count),
            'width': int(w),
            'height': int(h)
        })
        dataset.setdefault("annotations", []).append({
            'image_id': int(count),
            'bbox': [int(xmin), int(ymin), int(xmax) - int(xmin), int(ymax) - int(ymin)],
            'category_id': 6,
            'area': int(w) * int(h),
            'iscrowd': 0,
            'id': int(count),
            'segmentation': []
        })

    def produceImage(self, file_in, file_out):
        image = Image.open(file_in)
        resized_image = image.resize((self.image_width, self.image_height), Image.ANTIALIAS)
        resized_image.save(file_out)

    def data_change(self):
        xmlpath = self.rootpath + '/Annotations_'
        imgpath = self.rootpath + '/JPEGImages'
        imgsetpath = self.rootpath + '/ImageSets'
        txtpath = imgsetpath + '/Main'

        if not osp.exists(xmlpath):
            os.makedirs(xmlpath)
        if not osp.exists(imgpath):
            os.makedirs(imgpath)
        if not osp.exists(imgsetpath):
            os.makedirs(imgsetpath)
        if not osp.exists(txtpath):
            os.makedirs(txtpath)

        json_file_arr = os.listdir(self.source_json_path)
        img_file_arr = os.listdir(self.source_img_path)

        json_arr = []
        img_arr = []

        for name in json_file_arr:
            if '.json' in name:
                json_arr.append(name)
        for name in img_file_arr:
            if '.jpg' in name or '.png' in name:
                img_arr.append(name)

        fixed_file_arr = []
        fixed_file_type = []

        json_arr = sorted(json_arr)
        img_arr = sorted(img_arr)

        for i in range(len(json_arr)):
            json = json_arr[i]
            img = img_arr[i]
            index_last_points_json = json.rfind(".")
            index_last_points_img = img.rfind(".")
            if json[:index_last_points_json] == img[:index_last_points_img]:
                fixed_file_arr.append(json[0:index_last_points_json])
                fixed_file_type.append('.' + img[index_last_points_img + 1:])

        annotation_arr = np.array([])

        for i in range(len(fixed_file_arr)):

            if self.need_to_change_image_size:
                self.produceImage(self.source_img_path + '/' + fixed_file_arr[i] + fixed_file_type[i],
                                  imgpath + '/' + str(i) + fixed_file_type[i])
            else:
                copyfile(self.source_img_path + '/' + fixed_file_arr[i] + fixed_file_type[i],
                         imgpath + '/' + str(i) + fixed_file_type[i])

            f = open(self.source_json_path + '/' + fixed_file_arr[i] + '.json', 'r', encoding='utf-8')
            my_dic = js.load(f)
            annotation_arr = np.append(annotation_arr, (fixed_file_arr[i], my_dic))
            f.close()

            print('\rRead dataset : ' + str(i + 1) + ' / ' + str(len(fixed_file_arr)), end='')

        print('')

        annotation_arr = annotation_arr.reshape(-1, 2)

        f1 = open(txtpath + '/test.txt', 'w')
        f2 = open(txtpath + '/trainval.txt', 'w')
        f3 = open(txtpath + '/person_trainval.txt', 'w')
        f4 = open(txtpath + '/train.txt', 'w')
        f5 = open(txtpath + '/val.txt', 'w')
        f6 = open(self.rootpath + '/ground_truth.txt', 'w')

        if not os.path.exists(dataset_path + '/Main/'):
            os.makedirs(dataset_path + '/Main/')
        f7 = open(dataset_path + '/Main/' + 'train.txt', 'w')

        for i in range(annotation_arr.shape[0]):
            f1.write(annotation_arr[i][0] + '\n')
            f2.write(annotation_arr[i][0] + '\n')
            f3.write(annotation_arr[i][0] + ' 1\n')
            f4.write(str(i) + '\n')
            f5.write(annotation_arr[i][0] + '\n')
            f6.write('\nGROUND TRUTH FOR: ' + annotation_arr[i][0] + '\n')
            f7.write(str(i) + '.jpg\n')
            f = open(xmlpath + '/' + str(i) + '.xml', 'w')
            f.write('<annotation>\n')
            f.write('\t<folder>VOC2007</folder>\n')
            f.write('\t<filename>' + annotation_arr[i][0] + '</filename>\n')
            f.write('\t<size>\n')
            if self.need_to_change_image_size:
                f.write('\t\t<width>%s</width>\n' % self.image_width)
                f.write('\t\t<height>%s</height>\n' % self.image_height)
            else:
                f.write('\t\t<width>%s</width>\n' % annotation_arr[i][1]['Area']['shape'][0])
                f.write('\t\t<height>%s</height>\n' % annotation_arr[i][1]['Area']['shape'][1])
            f.write('\t\t<depth>3</depth>\n')
            f.write('\t</size>\n')
            f.write('\t<segmented>0</segmented>\n')
            if len(annotation_arr[i][1]['Area']['labels']) > 0:
                for j in range(len(annotation_arr[i][1]['Area']['labels'])):
                    f6.write('label: ')
                    f.write('\t<object>\n')
                    if self.use_my_labels:
                        f.write('\t\t<name>%s</name>\n' % self.my_labels[int(annotation_arr[i][1]['Area']['labels'][j][0])].replace("\n", ""))
                    else:
                        f.write('\t\t<name>person</name>\n')
                    f.write('\t\t<pose>Unspecified</pose>\n')
                    f.write('\t\t<truncated>0</truncated>\n')
                    f.write('\t\t<difficult>0</difficult>\n')
                    f.write('\t\t<bndbox>\n')
                    if self.need_to_change_image_size:
                        f6.write('%d' % int(annotation_arr[i][1]['Area']['polygons'][j][0][0] * self.image_width /
                                            annotation_arr[i][1]['Area']['shape'][0]))
                        f6.write(' || ')
                        f6.write('%d' % int(annotation_arr[i][1]['Area']['polygons'][j][0][1] * self.image_width /
                                            annotation_arr[i][1]['Area']['shape'][1]))
                        f6.write(' || ')
                        f6.write('%d' % int(annotation_arr[i][1]['Area']['polygons'][j][2][0] * self.image_width /
                                            annotation_arr[i][1]['Area']['shape'][0]))
                        f6.write(' || ')
                        f6.write('%d' % int(annotation_arr[i][1]['Area']['polygons'][j][2][1] * self.image_width /
                                            annotation_arr[i][1]['Area']['shape'][1]))
                        f6.write(' || ')
                        f6.write(self.my_labels[int(annotation_arr[i][1]['Area']['labels'][j][0])])
                        f6.write('\n')
                        f.write('\t\t\t<xmin>%s</xmin>\n' % int(
                            annotation_arr[i][1]['Area']['polygons'][j][0][0] * self.image_width /
                            annotation_arr[i][1]['Area']['shape'][0]))
                        f.write('\t\t\t<ymin>%s</ymin>\n' % int(
                            annotation_arr[i][1]['Area']['polygons'][j][0][1] * self.image_height /
                            annotation_arr[i][1]['Area']['shape'][1]))
                        f.write('\t\t\t<xmax>%s</xmax>\n' % int(
                            annotation_arr[i][1]['Area']['polygons'][j][2][0] * self.image_width /
                            annotation_arr[i][1]['Area']['shape'][0]))
                        f.write('\t\t\t<ymax>%s</ymax>\n' % int(
                            annotation_arr[i][1]['Area']['polygons'][j][2][1] * self.image_height /
                            annotation_arr[i][1]['Area']['shape'][1]))
                    else:
                        f6.write('%d' % annotation_arr[i][1]['Area']['polygons'][j][0][0])
                        f6.write(' || ')
                        f6.write('%d' % annotation_arr[i][1]['Area']['polygons'][j][0][1])
                        f6.write(' || ')
                        f6.write('%d' % annotation_arr[i][1]['Area']['polygons'][j][2][0])
                        f6.write(' || ')
                        f6.write('%d' % annotation_arr[i][1]['Area']['polygons'][j][2][1])
                        f6.write(' || ')
                        f6.write('person\n')
                        f.write('\t\t\t<xmin>%s</xmin>\n' % annotation_arr[i][1]['Area']['polygons'][j][0][0])
                        f.write('\t\t\t<ymin>%s</ymin>\n' % annotation_arr[i][1]['Area']['polygons'][j][0][1])
                        f.write('\t\t\t<xmax>%s</xmax>\n' % annotation_arr[i][1]['Area']['polygons'][j][2][0])
                        f.write('\t\t\t<ymax>%s</ymax>\n' % annotation_arr[i][1]['Area']['polygons'][j][2][1])
                    f.write('\t\t</bndbox>\n')
                    f.write('\t</object>\n')
            f.write('</annotation>')
            f.close()

            print('\rWrite dataset : ' + str(i + 1) + ' / ' + str(annotation_arr.shape[0]), end='')

        print('')

        f1.close()
        f2.close()
        f3.close()
        f4.close()
        f5.close()
        f6.close()
        f7.close()
    
    def produce_darknet_cfg(self):
        currentPath = os.getcwd()
    
        with open('../Server_DataBase/train_dataset/darknet_dataset/coco.data', 'a+') as f:
            pass
        with open('../Server_DataBase/train_dataset/darknet_dataset/coco.data', 'w') as f:
            f.write("classes= " + str(len(self.my_labels)) + "\n")
            f.write("train  = " + currentPath + "/../Server_DataBase/train_dataset/darknet_dataset/Main/train.txt\n")
            f.write("valid  = " + currentPath + "/../Server_DataBase/train_dataset/darknet_dataset/Main/valid.txt\n")
            f.write("names  = " + currentPath + "/../Server_DataBase/train_dataset/darknet_dataset/coco.names\n")
            f.write("backup = " + currentPath + "/../Linux/darknet-master/backup\n")
            f.write("eval=coco\n")
        
        with open('../Server_DataBase/train_dataset/darknet_dataset/coco.names', 'a+') as f:
            pass
        with open('../Server_DataBase/train_dataset/darknet_dataset/coco.names', 'w') as f:
            for label in self.my_labels:
                f.write(label)
        
        with open('../Server_DataBase/train_dataset/darknet_dataset/yolov3_train.cfg', 'a+') as f_train:
            pass
        with open('../Server_DataBase/train_dataset/darknet_dataset/yolov3_test.cfg', 'a+') as f_test:
            pass
        with open('../Server_DataBase/train_dataset/darknet_dataset/yolov3_train.cfg', 'w') as f_train:
            with open('../Server_DataBase/train_dataset/darknet_dataset/yolov3_test.cfg', 'w') as f_test:
                with open('../src/Python/backup/yolov3_backup.cfg', 'r') as f_backup:
                    meet_tag = False
                    for line in f_backup.readlines():
                        msg = line.split('=')[0]
                        if meet_tag:
                            if msg == 'filters':
                                f_train.write('filters=' + str(3 * (len(self.my_labels) + 5)) + '\n')
                                f_test.write('filters=' + str(3 * (len(self.my_labels) + 5)) + '\n')
                            elif msg == 'classes':
                                f_train.write('classes=' + str(len(self.my_labels)) + '\n')
                                f_test.write('classes=' + str(len(self.my_labels)) + '\n')
                            elif msg == 'batch':
                                f_train.write('batch=64\n')
                                f_test.write('batch=1\n')
                            elif msg == 'subdivisions':
                                f_train.write('subdivisions=64\n')
                                f_test.write('subdivisions=1\n')
                            else:
                                f_train.write(line)
                                f_test.write(line)

                            meet_tag = False
                        else:
                            f_train.write(line)
                            f_test.write(line)

                        if line == '#change!\n':
                            meet_tag = True

def Main_DataChange():
    if os.path.exists(dataset_path + '/Annotations_'):
        shutil.rmtree(dataset_path + '/Annotations_')

    if os.path.exists(dataset_path + '/ImageSets'):
        shutil.rmtree(dataset_path + '/ImageSets')

    if os.path.exists(dataset_path + '/JPEGImages'):
        shutil.rmtree(dataset_path + '/JPEGImages')

    if os.path.exists(dataset_path + '/imgs'):
        shutil.rmtree(dataset_path + '/imgs')

    if os.path.exists(dataset_path + '/ground_truth.txt'):
        os.remove(dataset_path + '/ground_truth.txt')

    if os.path.exists(dataset_path + '/annotations/'):
        shutil.rmtree(dataset_path + '/annotations')

    if os.path.exists(dataset_path + '/images/'):
        shutil.rmtree(dataset_path + '/images')

    if os.path.exists(dataset_path + '/labels/'):
        shutil.rmtree(dataset_path + '/labels')

    if os.path.exists(dataset_path + '/Main/'):
        shutil.rmtree(dataset_path + '/Main')

    dataset = {}

    my_labels = []

    with open('../Server_DataBase/train_dataset/darknet_dataset/my_labels.txt', 'r') as f:
        for line in f.readlines():
            my_labels.append(line.replace("\n", ""))

    datachange = DataChange(dataset_path, dataset_path + '/enhance_dataset/', dataset_path + '/enhance_dataset/', my_labels, True)

    datachange.produce_darknet_cfg()
    datachange.data_change()

    print('Change to VOC dataset succeed!')

    wd = getcwd()

    for year, image_set in sets:
        if not os.path.exists(dataset_path + '/labels/val2014/'):
            os.makedirs(dataset_path + '/labels/val2014/')
        image_ids = open(dataset_path + '/ImageSets/Main/%s.txt' % (image_set)).read().strip().split()
        # list_file = open(dataset_path + '/%s.txt' % (image_set), 'w')

        img_num = len(image_ids)
        converted_img_num = 0

        for image_id in image_ids:
            converted_img_num += 1

            # list_file.write(dataset_path + '/JPEGImages/%s.jpg\n' % (image_id))
            datachange.convert_annotation(year, image_id)

            print('\rConvert dataset : ' + str(converted_img_num) + ' / ' + str(img_num), end='')

        print('')
        # list_file.close()

    print('Create json dataset succeed!')

    im_path = dataset_path + "/imgs/"
    trainimg = dataset_path + "/images/val2014/"
    if not os.path.exists(im_path):
        os.makedirs(im_path)
    if not os.path.exists(trainimg):
        os.makedirs(trainimg)
    if not os.path.exists(im_path + 'img0/'):
        os.makedirs(im_path + 'img0/')

    imgs_list = os.listdir(dataset_path + '/JPEGImages/')

    copy_img_num = len(imgs_list)

    copyed_num = 0

    for file in imgs_list:
        copyfile(dataset_path + '/JPEGImages/' + file, dataset_path + '/imgs/img0/' + file)

        copyed_num += 1

        print('\rCopy images : ' + str(copyed_num) + ' / ' + str(copy_img_num), end='')

    print('')

    xmls_list = os.listdir(dataset_path + '/Annotations_/')

    copy_xml_num = len(xmls_list)

    copyed_num = 0

    for file in xmls_list:
        copyfile(dataset_path + '/Annotations_/' + file, dataset_path + '/imgs/img0/' + file)

        copyed_num += 1

        print('\rCopy xmls : ' + str(copyed_num) + ' / ' + str(copy_xml_num), end='')

    print('')

    print('Prepare changing to COCO dataset succeed!')

    cmax = 0
    dirpath = os.listdir(im_path)

    for imgdir in dirpath:

        f1 = os.listdir(trainimg)
        for file in f1:
            cmax = max(cmax, int(file.split(".")[0]))
        count = 1

        total_img_num = 0

        file_list = os.listdir(im_path + imgdir)

        for file in os.listdir(im_path + imgdir):
            if file[-3:] == 'jpg':
                total_img_num += 1

        for file in file_list:

            if file.split(".")[1] == "jpg":
                oldname = os.path.join(im_path + imgdir, file)
                jpgname = os.path.join(trainimg, file)
                shutil.copyfile(oldname, jpgname)
                datachange.readxml(dataset, os.path.join(im_path + imgdir, file.split(".")[0] + ".xml"), count + cmax, file)

                print('\rChange to COCO dataset : ' + str(count) + ' / ' + str(total_img_num), end='')

                count += 1

        print('')

    print('Change to COCO dataset succeed!')

    for i in range(1, len(my_labels) + 1):
        dataset.setdefault("categories", []).append({
            'id': i,
            'name': i,
            'supercategory': 'No'
        })

    folder = os.path.join(dataset_path + '/annotations/')
    if not os.path.exists(folder):
        os.makedirs(folder)
    json_name = os.path.join(folder + 'instances_minival2014.json')
    with open(json_name, 'w') as f:
        js.dump(dataset, f)

    # shutil.rmtree(dataset_path + '/Annotations_')
    # shutil.rmtree(dataset_path + '/ImageSets')
    # shutil.rmtree(dataset_path + '/JPEGImages')
    # shutil.rmtree(dataset_path + '/imgs')
    # os.remove(dataset_path + '/ground_truth.txt')

    path_pictures = dataset_path + '/images/val2014/'

    file_name = os.listdir(path_pictures)

    train_txt_path = dataset_path + '/Main/train.txt'
    valid_txt_path = dataset_path + '/Main/valid.txt'

    with open(train_txt_path, 'w') as f:
        with open(valid_txt_path, "w") as f_valid:
            for i in file_name:
                f.write(path_pictures + i + '\n')
                f_valid.write(path_pictures + i + '\n')

    return

if __name__ == "__main__":
    Main_DataChange()
