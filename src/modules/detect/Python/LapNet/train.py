import torch
import numpy as np
import time
import os
# import argparse

import cv2

from LapNet import LAPNet
from loss import DiscriminativeLoss
from cvt_dataset import CvtDataset
from logger import Logger
from torch.nn import DataParallel
from collections import OrderedDict
from torch.nn.parameter import Parameter

class LapNet_Train:

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

        if not os.path.exists('temp_output.txt'):
            with open('temp_output.txt', 'w') as f:
                pass

        # dataset_path = args.dataset_path
        dataset_path = 'Dataset/train_dataset'
        train_dataset = CvtDataset(dataset_path, size=self.SIZE)
        self.train_dataloader = torch.utils.data.DataLoader(dataset=train_dataset, batch_size=self.BATCH_SIZE, pin_memory=True, shuffle=True, num_workers=self.NUM_WORKERS)
        #model = SegNet(input_ch=INPUT_CHANNELS, output_ch=OUTPUT_CHANNELS).cuda() 
        self.model = LAPNet(input_ch=self.INPUT_CHANNELS, output_ch=self.OUTPUT_CHANNELS,internal_ch = 8).cuda()
        self.optimizer = torch.optim.Adam(self.model.parameters(), lr=self.LEARNING_RATE, betas=(0.9, 0.99), amsgrad=True)
        self.start_epoch = -1
        # chkpt_filename = "LapNet_chkpt_default"+"_GPU"+str(args.gpu_idx)+".pth"
        current_file_list = os.listdir(os.getcwd()+'/trained_model')
        current_epoch_num = -1
        for file_name in current_file_list:
            print("filename:",file_name)
            self.output(["filename:",file_name])
            if 'LapNet_chkpt_better_epoch' in file_name:
                temp_epoch_num = int(file_name.split('_')[3].split('h')[1])
                if temp_epoch_num > current_epoch_num:
                    current_epoch_num = temp_epoch_num
        # chkpt_filename = os.getcwd()+'/trained_model/'+"LapNet_chkpt_better_epoch" + str(current_epoch_num) + "_GPU" + str(args.gpu_idx) + ".pth"
        chkpt_filename = os.getcwd()+'/trained_model/'+"LapNet_chkpt_better_epoch" + str(current_epoch_num) + "_GPU" + str(0) + ".pth"
        if not os.path.exists(os.getcwd()+'/trained_model'):
            os.mkdir(os.getcwd()+'/trained_model')
        if os.path.isfile(chkpt_filename):
            checkpoint = torch.load(chkpt_filename)
            self.start_epoch = checkpoint['epoch']
            print("Found Checkpoint file",chkpt_filename,".")
            self.output(["Found Checkpoint file",chkpt_filename,"."])
            print("The checkpoint was saved at epoch",checkpoint['epoch'],".")
            self.output(["The checkpoint was saved at epoch",str(checkpoint['epoch']),"."])
            print("Taining stats is reset form epoch",self.start_epoch)
            self.output(["Taining stats is reset form epoch",str(self.start_epoch)])
            # if(args.optimizer_reset != 1):
            if(100 != 1):
                self.optimizer.load_state_dict(checkpoint['optimizer'])
            else:
                print("Optimizer State Reset.")
                self.output(["Optimizer State Reset."])
            self.model.load_state_dict(checkpoint['net'])
            self.load_state_dict(self.model, self.state_dict(self.model))
        # scheduler = torch.optim.lr_scheduler.MultiStepLR(optimizer, milestones=[20,30,400,1000,6000,7000,8000], gamma=0.9)
        # 

        self.criterion_ce = torch.nn.CrossEntropyLoss()
        # criterion_mse = torch.nn.MSELoss(reduce=True, size_average=True)
        # criterion_disc = DiscriminativeLoss(delta_var=0.5,
        #                                     delta_dist=1.5,
        #                                     norm=2,
        #                                     usegpu=True)

    def output(self, string):
        with open('temp_output.txt', 'a+') as f:
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
            

    def train(self):
        # refer from : https://github.com/Sayan98/pytorch-segnet/blob/master/src/train.py
        is_better = True
        prev_loss = float('inf')

        # print("Found",torch.cuda.device_count(),"GPU(s).","Using GPU(s) form idx:",args.gpu_idx)
        # self.output(["Found",str(torch.cuda.device_count()),"GPU(s).","Using GPU(s) form idx:",str(args.gpu_idx)])
        print("Found",torch.cuda.device_count(),"GPU(s).","Using GPU(s) form idx:", 0)
        self.output(["Found",str(torch.cuda.device_count()),"GPU(s).","Using GPU(s) form idx:", str(0)])
        
        #model = DataParallel(cpu_model)
        # model = cpu_model.cuda()
        #device = torch.device("cuda")
        # model = torch.nn.DataParallel(model) #= model.cuda()
        self.model.train()

        last_better_epoch = self.start_epoch

        for epoch in range(max(0, self.start_epoch),self.NUM_EPOCHS):

            with open('temp_train_mode.txt', 'r') as f:
                info = f.readline()

                if info == "False":
                    return

            t_start = time.time()
            loss_f = []
            for batch_idx, (imgs, sem_labels) in enumerate(self.train_dataloader):
                #os.system("clear")

                loss = 0
                

                img_tensor = torch.tensor(imgs).cuda()
                sem_tensor = torch.tensor(sem_labels).cuda()
                # ins_tensor = torch.tensor(ins_labels).cuda()
                # Init gradients
                self.optimizer.zero_grad()
                img_inpt = np.array(np.transpose(torch.squeeze(img_tensor[0],0).cpu().detach().numpy(), (1,2,0)) ,dtype=np.uint8)
                # Predictions
                sem_pred = self.model(img_tensor)
                # sem_pred=torch.floor(sem_pred)
                seg_map = torch.squeeze(sem_pred,0).cpu().detach().numpy()
                # ins_map = torch.squeeze(ins_pred,0).cpu().detach().numpy()
                
                # Discriminative Loss
                # disc_loss = criterion_disc(ins_pred, ins_tensor, [INS_CH] * len(img_tensor))/6400.5

                # CrossEntropy Loss

                ce_loss = self.criterion_ce(sem_pred.permute(0,2,3,1).contiguous().view(-1,self.OUTPUT_CHANNELS),
                                    sem_tensor.view(-1))
                # print(
                # np.shape(sem_pred.permute(0,2,3,1).contiguous().view(-1,OUTPUT_CHANNELS)[:,1]),
                # np.shape(sem_tensor.view(-1).float())
                # )
                # mse = criterion_mse(sem_pred.permute(0,2,3,1).contiguous().view(-1,OUTPUT_CHANNELS)[:,1],sem_tensor.view(-1).float())/1000

                loss = ce_loss #+ disc_loss
                    
                loss.backward()
                self.optimizer.step()

                loss_f.append(loss.cpu().data.numpy())
                print('\rTrain Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                        epoch, batch_idx * len(imgs), len(self.train_dataloader.dataset),
                        100. * batch_idx / len(self.train_dataloader), loss.item()),end='')
                self.output(['Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                        epoch, batch_idx * len(imgs), len(self.train_dataloader.dataset),
                        100. * batch_idx / len(self.train_dataloader), loss.item())])
                # seg_show = seg_map[0][1]


                # _, seg_show2 = cv2.threshold(seg_show+1, 0, 0,cv2.THRESH_TOZERO)
                # seg_show = cv2.normalize(seg_show,seg_show,0,1,cv2.NORM_MINMAX)
                # seg_show2 = cv2.normalize(seg_show2,seg_show2,0,1,cv2.NORM_MINMAX)
                # seg_show = cv2.convertScaleAbs(seg_show,seg_show,255)
                # seg_show2 = cv2.convertScaleAbs(seg_show2,seg_show2,255)
                # cv2.imshow("seg_pred",cv2.addWeighted(img_inpt,0.5,cv2.applyColorMap(seg_show,cv2.COLORMAP_JET),0.5,0))
                # cv2.imshow("colormap",cv2.applyColorMap(seg_show,cv2.COLORMAP_JET))
                # cv2.imshow("segthresh",cv2.applyColorMap(seg_show2,cv2.COLORMAP_MAGMA))
                # for i in range(32):
                #     ins_show = ins_map[0][i]
                #     ins_show = cv2.normalize(ins_show,ins_show,0,1,cv2.NORM_MINMAX)
                #     ins_show = cv2.convertScaleAbs(ins_show,ins_show,255)
                #     cv2.imshow("insmap"+str(i),cv2.applyColorMap(ins_show,cv2.COLORMAP_OCEAN))
                


                # cv2.imshow("img_inpt",img_inpt)
                # if cv2.waitKey(1) == 27:
                #     print("Saving current chkpt...")
                #     state = {'net':state_dict(model), 'optimizer':optimizer.state_dict(), 'epoch':epoch}
                #     torch.save(state, chkpt_filename)
                #     exit()
                    
                #Tensorboard
                # if batch_idx % LOG_INTERVAL == 0:
                #     print("log at train idx:",batch_idx,end='\r')
                #     info = {'loss': loss.item(), 'ce_loss': ce_loss.item(), 'epoch': epoch}
                #     for tag, value in info.items():
                #         logger.scalar_summary(tag, value, batch_idx + 1)
                #     # 2. Log values and gradients of the parameters (histogram summary)
                #     for tag, value in model.named_parameters():
                #         tag = tag.replace('.', '/')
                #         logger.histo_summary(tag, value.data.cpu().numpy(), batch_idx + 1)
                #           # logger.histo_summary(tag + '/grad', value.grad.data.cpu().numpy(), batch_idx + 1)
                    
                # if batch_idx % 100 == 0:
                #     torch.save(model.state_dict(), "Lap"+str(args.gpu_idx)+".pth")
                #     print("\t\tModel Saved.")
                    # # 3. Log training images (image summary)
                    # info = {'images': img_tensor.view(-1, 3, SIZE[1], SIZE[0])[:BATCH_SIZE].cpu().numpy(),
                    #         'labels': sem_tensor.view(-1, SIZE[1], SIZE[0])[:BATCH_SIZE].cpu().numpy(),
                    #         'sem_preds': sem_pred.view(-1, 2, SIZE[1], SIZE[0])[:BATCH_SIZE,1].data.cpu().numpy(),
                    #         'ins_preds': ins_pred.view(-1, SIZE[1], SIZE[0])[:BATCH_SIZE*5].data.cpu().numpy()}

                    # for tag, images in info.items():
                    #     logger.image_summary(tag, images, batch_idx + 1)
                
            dt = time.time() - t_start
            is_better = np.mean(loss_f) < prev_loss
            
            state = {'net':self.state_dict(self.model), 'optimizer':self.optimizer.state_dict(), 'epoch':epoch}
            # torch.save(state, os.getcwd()+"/trained_model/"+"LapNet_chkpt_epoch"+str(epoch)+"_GPU"+str(args.gpu_idx)+".pth")

            if is_better:
                prev_loss = np.mean(loss_f)
                print("\t\tBest Model.")
                self.output(["Best Model."])
                # torch.save(state, os.getcwd()+"/trained_model/"+"LapNet_chkpt_better_epoch"+str(epoch)+"_GPU"+str(args.gpu_idx)+".pth")
                # if last_better_epoch < epoch:
                #     if last_better_epoch > 0:
                #         os.remove(os.getcwd()+"/trained_model/"+"LapNet_chkpt_better_epoch"+str(last_better_epoch)+"_GPU"+str(args.gpu_idx)+".pth")
                #     last_better_epoch = epoch
                torch.save(state, os.getcwd()+"/trained_model/"+"LapNet_chkpt_better_epoch"+str(epoch)+"_GPU"+str(0)+".pth")
                if last_better_epoch < epoch:
                    if last_better_epoch > -1:
                        os.remove(os.getcwd()+"/trained_model/"+"LapNet_chkpt_better_epoch"+str(last_better_epoch)+"_GPU"+str(0)+".pth")
                    last_better_epoch = epoch
                
            print("Epoch #{}\tLoss: {:.8f}\t Time: {:2f}s, Lr: {:2f}".format(epoch+1, np.mean(loss_f), dt, self.optimizer.param_groups[0]['lr']))
            self.output(["Epoch #{}\tLoss: {:.8f}\t Time: {:2f}s, Lr: {:2f}".format(epoch+1, np.mean(loss_f), dt, self.optimizer.param_groups[0]['lr'])])


def Main_LapNet_Train():
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

    if os.getcwd()[-6:] != "LapNet":
        os.chdir(os.getcwd() + '/../src/Python/LapNet')
        # os.chdir(os.getcwd() + '/LapNet')
    
    with open('temp_train_mode.txt', 'w') as f:
        f.write('True')

    print('in ok')
    lapnet_train = LapNet_Train()
    print('init ok')

    lapnet_train.train()
    print('train ok')

    return

if __name__ == "__main__":
    Main_LapNet_Train()
