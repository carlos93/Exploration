import os 

path = "C:/Projects/Exploration/Content/Models/"
for i in range(11, 39):
    folder = f"pm0201_{i:02}_00"
    if not os.path.exists(path+folder):
        os.makedirs(path+folder)
