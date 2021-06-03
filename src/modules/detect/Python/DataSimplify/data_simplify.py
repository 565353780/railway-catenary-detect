import shutil
import os

class DataSimplify:
    def __init__(self, root_path, target_path, target_dataset_num, workers_num):
        self.root_path = root_path
        self.target_path = target_path
        self.target_dataset_num = target_dataset_num
        self.workers_num = workers_num

        self.source_dataset_num = 0
        self.unmarked_dataset_num = 0

        self.source_file_path = []
        self.unmarked_file_path = []

        self.dataset_num_for_each_worker = 0
        self.res_dataset_num = 0
        self.work_path = self.target_path + "work_station/"

        self.use_all_unmarked_dataset = False

    def create_dataset_struct(self, path_to_root=""):
        if not os.path.exists(self.target_path):
            os.makedirs(self.target_path)
        dir_list = os.listdir(self.root_path + path_to_root)
        json_file_created = False
        for dir in dir_list:
            if os.path.isdir(self.root_path + path_to_root + dir):
                if not os.path.exists(self.target_path + path_to_root + dir):
                    os.makedirs(self.target_path + path_to_root + dir)
                self.create_dataset_struct(path_to_root + dir + "/")
            elif not json_file_created:
                if ".jpg" in dir:
                    if not os.path.exists(self.target_path + path_to_root + "/Annotations"):
                        os.makedirs(self.target_path + path_to_root + "/Annotations")
                    if not os.path.exists(self.target_path + path_to_root + "/json"):
                        os.makedirs(self.target_path + path_to_root + "/json")
                    json_file_created = True

    def get_file_list(self, path_to_root=""):
        file_list = os.listdir(self.root_path + path_to_root)
        for file in file_list:
            if os.path.isdir(self.root_path + path_to_root + file):
                self.get_file_list(path_to_root + file + "/")
            elif "." in file and file.split(".")[1] == "jpg":
                self.source_file_path.append(path_to_root + file)
                self.source_dataset_num += 1
                if not os.path.exists(self.root_path + path_to_root + "json/" + file[:-3] + "json"):
                    self.unmarked_file_path.append(path_to_root + file)
                    self.unmarked_dataset_num += 1
                
                print("\rfound", self.source_dataset_num, "images...  ", end="")
    
    def get_absolute_file_path_list(self):
        self.source_dataset_num = 0
        self.unmarked_dataset_num = 0

        self.source_file_path = []
        self.unmarked_file_path = []

        self.get_file_list()

        print()

        print("Dataset search finished. Now we have", self.source_dataset_num, "images total;", self.unmarked_dataset_num, "images still unmarked.")

        if self.unmarked_dataset_num <= self.target_dataset_num:
            print("We don't have enough unmarked images, and they'll all be used.")
            self.use_all_unmarked_dataset = True

    def create_work_station(self):
        for i in range(self.workers_num):
            if not os.path.exists(self.work_path + str(i)):
                os.makedirs(self.work_path + str(i))
        
        if self.use_all_unmarked_dataset:
            self.dataset_num_for_each_worker = int(self.unmarked_dataset_num / self.workers_num)
            self.res_dataset_num = self.unmarked_dataset_num % self.workers_num
        else:
            self.dataset_num_for_each_worker = int(self.target_dataset_num / self.workers_num)
            self.res_dataset_num = self.target_dataset_num % self.workers_num
        
        print("Work delivered finished. We have", self.workers_num, "workers, and each worker has", self.dataset_num_for_each_worker, "images to mark.")

        if self.res_dataset_num > 0:
            print("There are", self.res_dataset_num, "images more, which will be marked randomly by workers.")


    def copy_unmarked_dataset(self):
        skip_num = int(self.unmarked_dataset_num / self.dataset_num_for_each_worker / self.workers_num)
        current_unmarked_dataset_index = 0
        total_work_num = min(self.workers_num * self.dataset_num_for_each_worker + self.res_dataset_num, self.target_dataset_num)

        for i in range(self.workers_num):
            current_work_path = self.work_path + str(i) + "/"
            with open(self.work_path + str(i) + ".txt", "w") as f:
                for j in range(self.dataset_num_for_each_worker):
                    current_file_path = self.unmarked_file_path[current_unmarked_dataset_index]
                    current_file_name = current_file_path.split("/")[len(current_file_path.split("/")) - 1]
                    current_file_path = current_file_path.split(current_file_name)[0]
                    shutil.copyfile(self.root_path + self.unmarked_file_path[current_unmarked_dataset_index], current_work_path + str(j) + ".jpg")
                    f.write(str(j) + "|chLi|" + self.root_path + "|chLi|" + current_file_path + "|chLi|" + current_file_name + "\n")
                    current_unmarked_dataset_index += skip_num
                    print("\rCopying files :", i * self.dataset_num_for_each_worker + j + 1, "/", total_work_num, "...  ", end="")
        
        current_unmarked_dataset_index -= skip_num - 1
        for i in range(self.res_dataset_num):
            current_work_path = self.work_path + str(i) + "/"
            with open(self.work_path + str(i) + ".txt", "a+") as f:
                j = self.dataset_num_for_each_worker
                current_file_path = self.unmarked_file_path[current_unmarked_dataset_index]
                current_file_name = current_file_path.split("/")[len(current_file_path.split("/")) - 1]
                current_file_path = current_file_path.split(current_file_name)[0]
                shutil.copyfile(self.root_path + self.unmarked_file_path[current_unmarked_dataset_index], current_work_path + str(j) + ".jpg")
                f.write(str(j) + "|chLi|" + self.root_path + "|chLi|" + current_file_path + "|chLi|" + current_file_name + "\n")
                current_unmarked_dataset_index += 1
                print("\rCopying files :", (self.workers_num - 1) * self.dataset_num_for_each_worker + j + 2, "/", total_work_num, "...  ", end="")

        print()


    def process(self):
        self.get_absolute_file_path_list()

        # self.create_work_station()

        # self.copy_unmarked_dataset()


if __name__ == "__main__":
    # data_simplify = DataSimplify("G:/chLi/tiedaobu/2C_mask/", "G:/public/tiedaobu/2C_mask/", 25000, 5)
    data_simplify = DataSimplify("G:/chLi/tiedaobu/2C_mask/", "G:/public/tiedaobu/2C/", 25000, 5)

    data_simplify.process()