#include "DetectServerThread.h"

DetectServerThread::DetectServerThread(QObject *parent)
{
    QString base_path = "/home/abaci/chLi/FilterSocket/Server_DataBase/train_dataset/darknet_dataset/";

    QString model_base_path = "/home/abaci/chLi/FilterSocket/Linux/darknet-master/backup/";

    std::string yolov3_cfg = (base_path + "yolov3_test.cfg").toStdString();

    std::string yolov3_weights = (model_base_path + "yolov3_train.backup").toStdString();

    std::string coco_cfg = (base_path + "coco.data").toStdString();

#ifdef Linux
    darknet_detector = new DarknetDetector(yolov3_cfg, yolov3_weights, coco_cfg);
#endif

    label_list_.clear();

    QFile json_file("configs/2C_label_config.json");

    if(json_file.open(QFile::ReadOnly))
    {
        QByteArray json_array = json_file.readAll();

        json_file.close();

        QJsonDocument json_doc = QJsonDocument::fromJson(json_array);

        QJsonObject json_Obj = json_doc.object();

        QJsonArray label_array = json_Obj.value("Area").toObject().value("labelDict").toArray()[0].toObject().value("options").toArray();

        for(int i = 0; i < label_array.size(); ++i)
        {
            label_list_.append(label_array[i].toArray()[1].toString());
        }
    }

    qDebug() << "init model finished!" << endl;
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
    QString img_path = img_Obj["imagepath"].toString();

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

//    clock_t start = clock();
    std::vector<std::pair<char *, std::vector<float>>> result = darknet_detector->getDarknetResult(image, data.info.img_width_, data.info.img_height_, data.info.img_format_);
    delete(image);
//    std::cout << int((clock() - start)/1000) << " ms" << std::endl;

    QJsonArray detections_Array;

    for(int i=0; i < result.size(); i++)
    {
        QJsonObject detection_Obj;
        detection_Obj.insert("label", label_list_[int(result[i].second[5])]);
        detection_Obj.insert("type", QString("bbox"));
        detection_Obj.insert("x1", float(result[i].second[1] / data.info.img_width_));
        detection_Obj.insert("x2", float(result[i].second[2] / data.info.img_width_));
        detection_Obj.insert("y1", float((result[i].second[1] + result[i].second[3]) / data.info.img_width_));
        detection_Obj.insert("y2", float((result[i].second[2] + result[i].second[4]) / data.info.img_width_));
        detection_Obj.insert("ID", QString::number(i));

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

    QString img_path = img_Obj.value("imagepath").toString();

    QDir dir_;

    if(!dir_.exists(img_path))
    {
        qDebug() << "DetectServerThread::darknet_test : image doesn't exist";
        return empty_output_Obj;
    }

    QString PicID = img_Obj["PicID"].toString();

    QJsonArray detections_Array;

    for(int i=0; i < rand()%10 + 1; i++)
    {
        QJsonObject detection_Obj;
        detection_Obj.insert("label", label_list_[int(rand()%label_list_.size())]);
        detection_Obj.insert("type", QString("bbox"));
        float x_, y_;

        x_ = (rand()%100 + 1) * 1.0 / 202.0;
        y_ = (rand()%100 + 1) * 1.0 / 202.0;
        detection_Obj.insert("x1", x_);
        detection_Obj.insert("x2", y_);
        detection_Obj.insert("y1", x_ + 0.3);
        detection_Obj.insert("y2", y_ + 0.3);
        detection_Obj.insert("ID", QString::number(i));

        detections_Array.append(detection_Obj);
    }

    QJsonObject output_Obj;
    output_Obj.insert("PicID", PicID);

    output_Obj.insert("detections", detections_Array);

    while(clock() - t_start < 150)
    {
        continue;
    }

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
