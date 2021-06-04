import torch
import numpy as np
import time
import os
# import argparse

import cv2

from LapNet import LAPNet
from loss import DiscriminativeLoss
from create_dataset import createDataset
from logger import Logger
from torch.nn import DataParallel
from collections import OrderedDict
from torch.nn.parameter import Parameter

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

class LapNet_Test:

    def __init__(self):
        # torch.cuda.set_device(args.gpu_idx)
        torch.cuda.set_device(0)

        # self.INPUT_CHANNELS = 3
        # self.OUTPUT_CHANNELS = 2
        # self.LEARNING_RATE = args.lr #1e-5
        # self.BATCH_SIZE = args.batch_size #20
        # self.NUM_EPOCHS = args.epoch #100
        # self.LOG_INTERVAL = 20
        # self.INS_CH = 32
        # self.SIZE = [args.img_size[0], args.img_size[1]] #[224, 224]
        # self.NUM_WORKERS = args.num_workers #20

        self.INPUT_CHANNELS = 3
        self.OUTPUT_CHANNELS = 2
        self.LEARNING_RATE = 3e-4
        self.BATCH_SIZE = 32
        self.NUM_EPOCHS = 10000000000000
        self.LOG_INTERVAL = 20
        self.INS_CH = 32
        self.SIZE = [1024,512]
        self.NUM_WORKERS = 32

        self.root_path = os.getcwd() + '/../src/Python/LapNet'

        # dataset_path = args.dataset_path
        self.model = LAPNet(input_ch=self.INPUT_CHANNELS, output_ch=self.OUTPUT_CHANNELS,internal_ch = 8).cuda()
        self.optimizer = torch.optim.Adam(self.model.parameters(), lr=self.LEARNING_RATE, betas=(0.9, 0.99), amsgrad=True)
        self.start_epoch = -1
        # chkpt_filename = "LapNet_chkpt_default"+"_GPU"+str(args.gpu_idx)+".pth"
        current_file_list = os.listdir(self.root_path + '/trained_model')
        current_epoch_num = -1
        for file_name in current_file_list:
            print("filename:",file_name)
            self.output(["filename:",file_name])
            if 'LapNet_chkpt_better_epoch' in file_name:
                temp_epoch_num = int(file_name.split('_')[3].split('h')[1])
                if temp_epoch_num > current_epoch_num:
                    current_epoch_num = temp_epoch_num
        # chkpt_filename = self.root_path + '/trained_model/'+"LapNet_chkpt_better_epoch" + str(current_epoch_num) + "_GPU" + str(args.gpu_idx) + ".pth"
        chkpt_filename = self.root_path + '/trained_model/'+"LapNet_chkpt_better_epoch" + str(current_epoch_num) + "_GPU" + str(0) + ".pth"
        if not os.path.exists(self.root_path + '/trained_model'):
            os.mkdir(self.root_path + '/trained_model')
        if os.path.isfile(chkpt_filename):
            checkpoint = torch.load(chkpt_filename)
            self.start_epoch = checkpoint['epoch']
            print("Found Checkpoint file",chkpt_filename,".")
            self.output(["Found Checkpoint file",chkpt_filename,"."])
            print("The checkpoint was saved at epoch",checkpoint['epoch'],".")
            self.output(["The checkpoint was saved at epoch",str(checkpoint['epoch']),"."])
            print("Training stats is reset form epoch",self.start_epoch)
            self.output(["Training stats is reset form epoch",str(self.start_epoch)])
            # if(args.optimizer_reset != 1):
            if(100 != 1):
                self.optimizer.load_state_dict(checkpoint['optimizer'])
            else:
                print("Optimizer State Reset.")
                self.output(["Optimizer State Reset."])
            self.model.load_state_dict(checkpoint['net'])
            self.load_state_dict(self.model, self.state_dict(self.model))
        # scheduler = torch.optim.lr_scheduler.MultiStepLR(optimizer, milestones=[20,30,400,1000,6000,7000,8000], gamma=0.9)

    def output(self, string):
        with open(self.root_path + '/temp_output.txt', 'a+') as f:
            for str_ in string:
                f.write(str_)
            f.write('\n')
        
        return

    def state_dict(self, model, destination=None, prefix='', keep_vars=False):
        own_state = model.module if isinstance(model, torch.nn.DataParallel) \
            else model
        if destination is None:
            destination = OrderedDict()
        for name, param in own_state._parameters.items():
            if param is not None:
                destination[prefix + name] = param if keep_vars else param.data
        for name, buf in own_state._buffers.items():
            if buf is not None:
                destination[prefix + name] = buf
        for name, module in own_state._modules.items():
            if module is not None:
                self.state_dict(module, destination, prefix + name + '.', keep_vars=keep_vars)
        return destination

    def load_state_dict(self, model, state_dict, strict=True):
        own_state = model.module.state_dict() if isinstance(model, torch.nn.DataParallel) \
            else model.state_dict()
        for name, param in state_dict.items():
            if name in own_state:
                if isinstance(param, Parameter):
                    # backwards compatibility for serialized parameters
                    param = param.data
                try:
                    own_state[name].copy_(param)
                except Exception:
                    raise RuntimeError('While copying the parameter named {}, '
                                        'whose dimensions in the model are {} and '
                                        'whose dimensions in the checkpoint are {}.'
                                        .format(name, own_state[name].size(), param.size()))
            elif strict:
                raise KeyError('unexpected key "{}" in state_dict'
                                .format(name))
        if strict:
            missing = set(own_state.keys()) - set(state_dict.keys())
            if len(missing) > 0:
                raise KeyError('missing keys in state_dict: "{}"'.format(missing))
            

    def test(self):

        self.model.eval()
        
        img_in_path = os.getcwd() + "/../Server_DataBase/message_set/lapnet_eval.jpg"
        img_out_path = os.getcwd() + "/../Server_DataBase/message_set/lapnet_eval_out.jpg"

        img_send_finished = os.getcwd() + "/../Server_DataBase/message_set/lapnet_eval_finish.txt"

        test_stopped = os.getcwd() + "/../Server_DataBase/message_set/lapnet_test_stopped.txt"

        while True:
            if os.path.exists(test_stopped):
                os.remove(test_stopped)
                return

            if not os.path.exists(img_send_finished):
                continue

            os.remove(img_send_finished)

            train_dataset = createDataset(img_in_path, size=self.SIZE)
            train_dataloader = torch.utils.data.DataLoader(dataset=train_dataset, batch_size=24, pin_memory=True,
                                                        shuffle=False, num_workers=0)
            
            img = list(enumerate(train_dataloader))[0][1]

            img_tensor = torch.tensor(img).cuda()

            # Predictions
            sem_pred = self.model(img_tensor)

            # sem_pred=torch.floor(sem_pred)
            seg_map = torch.squeeze(sem_pred, 0).cpu().detach().numpy()

            seg_show = seg_map[1]
            # (h,w)=seg_show.shape

            # result_img = np.zeros((h, w))
            # for i in range(result_img.shape[0]):
            #     for j in range(result_img.shape[1]):
            #         if seg_show[i][j] > -2:
            #             result_img[i][j] = 255

            _, seg_show2 = cv2.threshold(seg_show + 1, 0, 0, cv2.THRESH_TOZERO)
            seg_show2 = cv2.normalize(seg_show2, seg_show2, 0, 1, cv2.NORM_MINMAX)
            seg_show2 = cv2.convertScaleAbs(seg_show2, seg_show2, 255)
            result_img = cv2.applyColorMap(seg_show2, cv2.COLORMAP_MAGMA)

            cv2.imwrite(img_out_path, result_img)

            os.remove(img_in_path)


def Main_LapNet_Test():
    # logger = Logger('./logslite'+str(args.gpu_idx))

    # parser = argparse.ArgumentParser(description="Train model")

    # parser.add_argument('--dataset-path', default='Dataset/train_dataset')
    # parser.add_argument('--lr', type=float, default=3e-4, help='learning rate')
    # parser.add_argument('--batch-size', type=int, default=32, help='batch size')
    # parser.add_argument('--img-size', type=int, nargs='+', default=[1024,512], help='image resolution: [width height]')
    # parser.add_argument('--epoch', type=int, default=10000000000000)
    # parser.add_argument('--gpu-idx',type = int,default= 0, help='using gpu(idx)')
    # parser.add_argument('--optimizer-reset', type=int, default=100)
    # parser.add_argument('--num-workers', type=int, default=1)

    # args = parser.parse_args()

    # if os.getcwd()[-6:] != "LapNet":
    #     os.chdir(os.getcwd() + '/../src/Python/LapNet')
    
    with open(os.getcwd() + '/../src/Python/LapNet/temp_train_mode.txt', 'w') as f:
        f.write('True')

    print('in ok')
    lapnet_test = LapNet_Test()
    print('init ok')

    lapnet_test.test()
    print('test ok')

    return

if __name__ == "__main__":
    Main_LapNet_Test()
