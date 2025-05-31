import bpy
import os
from pathlib import Path
import functools
from multiprocessing import Pool

directory_pla = "D:\ROMS\Models\Pokemon LA Models"
directory_sv = "D:\ROMS\Models\Pokémon SCVI Base + DLC Model Dump"
output = "D:\ROMS\Models\output"

def export_model(entry: int, input: str, output: str):

    fixed_entry = f"{entry:04}"
    parent_directory = f"{input}\pm{fixed_entry}"

    if not os.path.exists(parent_directory):
        print(f"Path dont exist {parent_directory}")
        return

    model: str = ""
    animations = []

    for item_name in os.listdir(parent_directory):

        folder_name = os.path.join(parent_directory, item_name)
        print(item_name)
        if not os.path.isdir(folder_name):
            continue

        model, animations = find_model_and_animations(folder_name, model, animations)

        suffix = "_rare" if is_shiny else ""
        parse_info(model, animations, f"{output}/{item_name}{suffix}")
    

def find_model_and_animations(folder_name, model, animations):
    # iterate over files in that directory
    for filename in os.listdir(folder_name):
        f = os.path.join(folder_name, filename)
        # checking if it is a file
        if os.path.isfile(f):
            if f.endswith("trmdl"):
                print(f"Model: {f}")
                model = f
            if f.endswith("tranm"):
                print(f"Anim: {f}")
                animations.append(f)
        elif os.path.isdir(f):
            model, animations = find_model_and_animations(f, model, animations)
    
    return model, animations

def parse_info(model, animations, output_path):
    bpy.ops.custom_import_scene.pokemonscarletviolet(filepath=model, rare=is_shiny)

    for anim in animations:
        bpy.ops.custom_import_anim.gfbanm(filepath=anim)

    actions = bpy.data.actions
    for action in actions:
        if action.name.find("walk") != -1 or action.name.find("run") != -1 or action.name.find("turn") != -1:
            for fcurve in action.fcurves.values():
                if fcurve.data_path.find("origin") != -1 and (fcurve.data_path.find("location") != -1 or fcurve.data_path.find("rotation_quaternion") != -1):
                    action.fcurves.remove(fcurve)
        #if is_pkmn_legends_arceus:
        # for fcurve in action.fcurves.values():
        #     if (fcurve.data_path.find("origin") != -1 or fcurve.data_path.find("tongue")) and fcurve.data_path.find("scale") != -1:
        #         action.fcurves.remove(fcurve)

    # export to GLFT
    bpy.ops.export_scene.gltf(filepath=f"{output_path}", export_unused_images=True, export_unused_textures=True, export_image_quality=100)

    # remove old model and materials
    bpy.ops.wm.read_homefile()


is_pkmn_legends_arceus = False
is_shiny = False
for i in range(88, 90):
    export_model(i, directory_pla if is_pkmn_legends_arceus else directory_sv, output)

# with Pool() as pool:
#     result = pool.map(functools.partial(export_model, input=directory, output=output), range(1, 2))
