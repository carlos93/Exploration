import os

input_path = "D:\ROMS\Models\ItemSprites"

# get all file names in the directory
for dir, subdirs, files in os.walk(input_path):
    for f in files:
        # separating the file name and extension
        file_name = f.split('.')[0]
        ext = f.split('.')[-1]
        new_name = file_name.split('item_')[1] + "." + ext
        # new file name
        os.rename(os.path.join(input_path, f), os.path.join(input_path, new_name))
        # print(new_name)