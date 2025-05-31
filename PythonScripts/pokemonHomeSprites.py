import urllib.request

from PIL import Image

output = "D:/ROMS/Models/FrontSprites/"

for i in range(150, 1026):
    file_name = f"pm{i:04}_00_00_00_big.png"
    urllib.request.urlretrieve(f"https://projectpokemon.org/images/sprites-models/sv-sprites-home/{i:04}.png", file_name)
    img = Image.open(file_name)
    img.save(f"{output}{file_name}")
