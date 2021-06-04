#include "DetectServerThread.h"

DetectServerThread::DetectServerThread(QObject *parent)
{
    QString network_type = "4c";//chLi

    QString class_num = "";
    QString model_component = "detect";

    if(network_type == "4c")
    {
        class_num = "32";
        model_component = "jueyuanzi";
    }
    if(network_type == "2c")
    {
        class_num = "2";
    }

    QString base_path = "/home/abaci/chLi/FilterSocket/Server_DataBase/train_dataset/darknet_dataset/";

    QString model_base_path = "/home/abaci/chLi/FilterSocket/Linux/darknet-master/backup/";

    std::string yolov3_cfg = (base_path + "yolov3_test_" + network_type + ".cfg").toStdString();

    std::string yolov3_weights = (model_base_path + "yolov3_train_" + network_type + "_" + model_component + "_" + class_num + "class.backup").toStdString();

    std::string coco_cfg = (base_path + "coco_" + network_type + ".data").toStdString();

    if(network_type == "4c")
    {
        use_resnet_ = true;
    }

#ifdef Linux
    darknet_detector = new DarknetDetector(yolov3_cfg, yolov3_weights, coco_cfg);
#endif

    label_list_.clear();

//    QFile json_file("configs/label_config.json");
//    if(json_file.open(QFile::ReadOnly))
//    {
//        QByteArray json_array = json_file.readAll();

//        json_file.close();

//        QJsonDocument json_doc = QJsonDocument::fromJson(json_array);

//        QJsonObject json_Obj = json_doc.object();

//        QJsonArray label_array = json_Obj.value("Area").toObject().value("labelDict").toArray()[0].toObject().value("options").toArray();
//    }

    for(int i = 0; i < class_num.toInt(); ++i)
    {
        label_list_.append(QString::number(i));
    }

    qDebug() << "init model finished!" <<label_list_;

    http_api_ = new GCL::HttpApi(this);
}

DetectServerThread::~DetectServerThread()
{

}

void ConnectedData::fromQImage(const QImage &img)
{
    this->info.img_width_ = img.width();
    this->info.img_height_ = img.height();
    this->info.img_format_ = 3;

//    this->data_.reserve(this->img_width_*this->img_height_*3);
    this->data_.resize(this->info.img_width_*this->info.img_height_*3);
    for(int i=0; i < img.width(); i++)
    {
        for(int j=0; j < img.height(); j++)
        {
            QRgb rgb = img.pixel(i,j);
            this->data_[3 * (j * img.width() + i) + 0] = qRed(rgb);
            this->data_[3 * (j * img.width() + i) + 1] = qGreen(rgb);
            this->data_[3 * (j * img.width() + i) + 2] = qBlue(rgb);

        }
    }
}

void ConnectedData::toQImage(QImage &img) const
{
    img = QImage(this->info.img_width_,this->info.img_height_, QImage::Format_RGB888);

    for(int i=0; i < img.width(); i++)
    {
        for(int j=0; j < img.height(); j++)
        {
            int r = this->data_[3 * (j * img.width() + i) + 0];
            int g = this->data_[3 * (j * img.width() + i) + 1];
            int b = this->data_[3 * (j * img.width() + i) + 2];

            img.setPixel(i,j,qRgb(r,g,b));
        }
    }
}

#ifdef Linux
std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::detect(image im, float thresh, float hier_thresh, float nms)
{
    int num = 0;

    int *pnum = &num;

    network_predict_image(net, im);

    detection *dets = get_network_boxes(net, im.w, im.h, thresh, hier_thresh, nullptr, 0, pnum);

    num = pnum[0];

    if (nms)
    {
        do_nms_obj(dets, num, meta.classes, nms);
    }

    std::vector<std::pair<char *, std::vector<float>>> res;

    for(int j = 0; j < num; ++j)
    {
        for(int i = 0; i < meta.classes; ++i)
        {
            if(dets[j].prob[i] > 0)
            {
                box b = dets[j].bbox;

                std::pair<char *, std::vector<float>> temp_data;

                temp_data.first = meta.names[i];

                temp_data.second.emplace_back(dets[j].prob[i]);
                temp_data.second.emplace_back(b.x - b.w / 2.0);
                temp_data.second.emplace_back(b.y - b.h / 2.0);
                temp_data.second.emplace_back(b.w);
                temp_data.second.emplace_back(b.h);
                temp_data.second.emplace_back(i);

                res.emplace_back(temp_data);
            }
        }
    }

    if(res.size() > 1)
    {
        for(int i = 0; i < res.size() - 1; ++i)
        {
            for(int j = i + 1; j < res.size(); ++j)
            {
                if(res[i].second[0] < res[j].second[0])
                {
                    std::pair<char *, std::vector<float>> exchange_data = res[i];

                    res[i] = res[j];
                    res[j] = exchange_data;
                }
            }
        }
    }

    free_image(im);

    free_detections(dets, num);

    return res;
}

std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::detect(char *img, float thresh, float hier_thresh, float nms)
{
    image im = load_image_color(img, 0, 0);

    return detect(im, thresh, hier_thresh, nms);
}

std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::detect(float *img, int w, int h, int c, float thresh, float hier_thresh, float nms)
{
    image im;
    im.w = w;
    im.h = h;
    im.c = c;

    int im_size = w * h * c;

    im.data = new float[im_size];

    memcpy(im.data, img, im_size * sizeof(float));

    return detect(im, thresh, hier_thresh, nms);
}

DarknetDetector::DarknetDetector(const std::string &yolov3_cfg, const std::string &yolov3_weights, const std::string &coco_cfg)
{
    const char *c_str_yolov3_cfg = yolov3_cfg.c_str();
    const char *c_str_yolov3_weights = yolov3_weights.c_str();
    const char *c_str_coco_cfg = coco_cfg.c_str();

    char *str_yolov3_cfg = new char[strlen(c_str_yolov3_cfg) + 1];
    char *str_yolov3_weights = new char[strlen(c_str_yolov3_weights) + 1];
    char *str_coco_cfg = new char[strlen(c_str_coco_cfg) + 1];

    strcpy(str_yolov3_cfg, c_str_yolov3_cfg);
    strcpy(str_yolov3_weights, c_str_yolov3_weights);
    strcpy(str_coco_cfg, c_str_coco_cfg);

    cuda_set_device(gpu_index);

    net = load_network(str_yolov3_cfg, str_yolov3_weights, 0);

    meta = get_metadata(str_coco_cfg);
}

DarknetDetector::~DarknetDetector()
{
    delete net;
}

std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::getDarknetResult(image img, float thresh, float hier_thresh, float nms)
{
    return detect(img, thresh, hier_thresh, nms);
}

std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::getDarknetResult(char *img, float thresh, float hier_thresh, float nms)
{
    return detect(img, thresh, hier_thresh, nms);
}

std::vector<std::pair<char *, std::vector<float>>> DarknetDetector::getDarknetResult(float *img, int w, int h, int c, float thresh, float hier_thresh, float nms)
{
    return detect(img, w, h, c, thresh, hier_thresh, nms);
}

QJsonObject DetectServerThread::darknet_process(QJsonObject img_Obj)
{
    QString img_path = img_Obj["Imagepath"].toString();

    if(!dir_.exists(img_path))
    {
        qDebug() << "DetectServerThread::darknet_process : image doesn't exist";
        return empty_output_Obj;
    }

    QString PicID = img_Obj["PicID"].toString();

    QImage img_in(img_path);

    ConnectedData data;

    data.fromQImage(img_in);

    float *image = new float[data.info.img_height_ * data.info.img_width_ * data.info.img_format_];

    for(int j=0; j < data.info.img_height_; j++)
    {
        for(int i=0; i < data.info.img_width_; i++)
        {
            // pixel(i,j)
            int color = data.getRed(i, j);
            if(color < 0)
            {
                color += 255;
            }
            image[j * data.info.img_width_ + i] = float(color) / 255.0;
        }
    }
    for(int j=0; j < data.info.img_height_; j++)
    {
        for(int i=0; i < data.info.img_width_; i++)
        {
            // pixel(i,j)
            int color = data.getGreen(i, j);
            if(color < 0)
            {
                color += 255;
            }
            image[data.info.img_height_ * data.info.img_width_ + j * data.info.img_width_ + i] = float(color) / 255.0;
        }
    }
    for(int j=0; j < data.info.img_height_; j++)
    {
        for(int i=0; i < data.info.img_width_; i++)
        {
            // pixel(i,j)
            int color = data.getBlue(i, j);
            if(color < 0)
            {
                color += 255;
            }
            image[2 * data.info.img_height_ * data.info.img_width_ + j * data.info.img_width_ + i] = float(color) / 255.0;
        }
    }

    // process

    clock_t start = clock();
    std::vector<std::pair<char *, std::vector<float>>> result = darknet_detector->getDarknetResult(image, data.info.img_width_, data.info.img_height_, data.info.img_format_);
    delete(image);
    std::cout << int((clock() - start)/1000) << " ms" << std::endl;

    QJsonArray detections_Array;

    for(int i=0; i < result.size(); i++)
    {
        int label = int(result[i].second[5]);

        if(use_resnet_)
        {
            if(label < 1 || label > 5)
            {
                continue;
            }
        }

        float x1 = float(result[i].second[1] / data.info.img_width_);
        float y1 = float(result[i].second[2] / data.info.img_height_);
        float x2 = float((result[i].second[1] + result[i].second[3]) / data.info.img_width_);
        float y2 = float((result[i].second[2] + result[i].second[4]) / data.info.img_height_);

        QJsonObject detection_Obj;

        if(use_resnet_)
        {
            float expand_times = 0.4;

            float new_x1 = fmax(0.0, x1 - expand_times * (x2 - x1));
            float new_x2 = fmin(1.0, x2 + expand_times * (x2 - x1));
            float new_y1 = fmax(0.0, y1 - expand_times * (y2 - y1));
            float new_y2 = fmin(1.0, y2 + expand_times * (y2 - y1));

            QJsonObject input_to_resnet_Obj;
            input_to_resnet_Obj.insert("image_file", img_path);
            input_to_resnet_Obj.insert("x1", new_x1);
            input_to_resnet_Obj.insert("x2", new_x2);
            input_to_resnet_Obj.insert("y1", new_y1);
            input_to_resnet_Obj.insert("y2", new_y2);
            input_to_resnet_Obj.insert("id", "0");

            QImage img_save = img_in.copy(int(new_x1 * data.info.img_width_), int(new_y1 * data.info.img_height_), int((new_x2 - new_x1) * data.info.img_width_), int((new_y2 - new_y1) * data.info.img_height_));

            if(!dir_.exists("./img_save/"))
            {
                dir_.mkpath("./img_save/");
            }

            img_save.save("./img_save/" + QString::number(current_img_idx_) + ".jpg");

            ++current_img_idx_;

            QJsonDocument doc_to_resnet = QJsonDocument(input_to_resnet_Obj);

            QByteArray bytes_to_resnet = doc_to_resnet.toJson();

            QJsonObject result_json = http_api_->post("http://0.0.0.0:2006/predict", bytes_to_resnet);

            detection_Obj.insert("Label", result_json.value("result").toString().toInt());
        }
        else
        {
            detection_Obj.insert("Label", label_list_[int(result[i].second[5])].toInt());
        }

        detection_Obj.insert("type", QString("bbox"));
        detection_Obj.insert("x1", x1);
        detection_Obj.insert("y1", y1);
        detection_Obj.insert("x2", x2);
        detection_Obj.insert("y2", y2);
        detection_Obj.insert("ID", QString::number(i));

        QJsonArray polygon_array;

        QJsonArray point1_array;
        point1_array.append(x1);
        point1_array.append(y1);
        polygon_array.append(point1_array);

        QJsonArray point2_array;
        point2_array.append(x1);
        point2_array.append(y2);
        polygon_array.append(point2_array);

        QJsonArray point3_array;
        point3_array.append(x2);
        point3_array.append(y2);
        polygon_array.append(point3_array);

        QJsonArray point4_array;
        point4_array.append(x2);
        point4_array.append(y1);
        polygon_array.append(point4_array);

        detection_Obj.insert("polygon", polygon_array);

        detections_Array.append(detection_Obj);
    }

    QJsonObject output_Obj;
    output_Obj.insert("PicID", PicID);

    output_Obj.insert("detections", detections_Array);

    return output_Obj;
}
#endif

QJsonObject DetectServerThread::darknet_test(QJsonObject img_Obj)
{
    clock_t t_start = clock();

    QString img_path = img_Obj.value("Imagepath").toString();

    QDir dir_;

    if(!dir_.exists(img_path))
    {
        qDebug() << "DetectServerThread::darknet_test : image doesn't exist";
        return empty_output_Obj;
    }

    qDebug()<<"tmp_obj"<<img_Obj;
    QString PicID = img_Obj["PicID"].toString();

    QJsonArray detections_Array;

    for(int i=0; i < rand()%10 + 1; i++)
    {
        QJsonObject detection_Obj;
        int index=int(rand()%label_list_.size());
        detection_Obj.insert("Label", label_list_[index].toInt());
        qDebug()<<"i,labeli"<<index<<label_list_[index];
        detection_Obj.insert("type", QString("bbox"));

        float x1, y1, x2, y2;

        x1 = (rand()%100 + 1) * 1.0 / 202.0;
        y1 = (rand()%100 + 1) * 1.0 / 202.0;
        x2 = x1 + 0.3;
        y2 = y1 + 0.4;
        detection_Obj.insert("x1", x1);
        detection_Obj.insert("x2", x2);
        detection_Obj.insert("y1", y1);
        detection_Obj.insert("y2", y2);
        detection_Obj.insert("ID", QString::number(i));

        QJsonArray polygon_array;

        QJsonArray point1_array;
        point1_array.append(x1);
        point1_array.append(y1);
        polygon_array.append(point1_array);

        QJsonArray point2_array;
        point2_array.append(x1);
        point2_array.append(y2);
        polygon_array.append(point2_array);

        QJsonArray point3_array;
        point3_array.append(x2);
        point3_array.append(y2);
        polygon_array.append(point3_array);

        QJsonArray point4_array;
        point4_array.append(x2);
        point4_array.append(y1);
        polygon_array.append(point4_array);

        detection_Obj.insert("Polygon", polygon_array);

        detections_Array.append(detection_Obj);
    }

    QJsonObject output_Obj;
    output_Obj.insert("PicID", PicID);

    output_Obj.insert("detections", detections_Array);

    while(clock() - t_start < 150)
    {
        continue;
    }

    qDebug()<<"finish run darknet test";
    return output_Obj;
}

bool DetectServerThread::input(QJsonObject img_Obj)
{
    mutex.lock();
    img_Obj_ = img_Obj;
    mutex.unlock();

    return true;
}

void DetectServerThread::run()
{
    mutex.lock();
#ifdef Linux
    QJsonObject output_Obj = this->darknet_process(img_Obj_);
#endif

#ifdef WIN32
    QJsonObject output_Obj = this->darknet_test(img_Obj_);
#endif
    mutex.unlock();

    emit signalBBoxOutput(output_Obj);
}
