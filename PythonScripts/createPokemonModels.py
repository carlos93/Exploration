import bpy
import os
from pathlib import Path
import functools
from multiprocessing import Pool
import json
from enum import IntEnum, unique

@unique
class ExportType(IntEnum):
    ScarletViolet = 1
    PLA = 2
    PLZA = 3
    SwSh = 4


is_shiny = False
export_colors = False
colors = []
export_type : ExportType = ExportType.PLA
directory_swsh = "D:\ROMS\Models\Pokemon SW SH"
directory_plZA = "D:\ROMS\Models\Pokemon Legends ZA Models"
directory_pla = "D:\ROMS\Models\Pokemon LA Models"
directory_sv = "D:\ROMS\Models\Pokémon SCVI Base + DLC Model Dump"
output = "D:\ROMS\Models\output"

def export_model(entry: int, input: str, output: str):

    fixed_entry = f"{entry:04}"

    model: str = ""
    animations = []

    if export_type == ExportType.SwSh:
        parent_directory = f"{input}\pm{fixed_entry}_00"

        if not os.path.exists(parent_directory):
            print(f"Path dont exist {parent_directory}")
            return
        
        model, animations = find_model_and_animations(parent_directory, model, animations)

        suffix = "_rare" if is_shiny else ""
        parse_info(model, animations, f"{output}/{parent_directory}{suffix}", parent_directory, colors)
    else:
        parent_directory = f"{input}\pm{fixed_entry}"

        if not os.path.exists(parent_directory):
            print(f"Path dont exist {parent_directory}")
            return
        
        for item_name in os.listdir(parent_directory):
            
            model = ""
            animations = []

            folder_name = os.path.join(parent_directory, item_name)
            print(item_name)
            if not os.path.isdir(folder_name):
                continue

            model, animations = find_model_and_animations(folder_name, model, animations)

            suffix = "_rare" if is_shiny else ""
            parse_info(model, animations, f"{output}/{item_name}{suffix}", item_name, colors)
    

def find_model_and_animations(folder_name, model, animations):
    # iterate over files in that directory
    for filename in os.listdir(folder_name):
        f = os.path.join(folder_name, filename)
        # checking if it is a file
        if os.path.isfile(f):
            if f.endswith("trmdl") or f.endswith("gfbmdl"):
                if export_type == ExportType.SwSh and f.split(".")[0].endswith("_rare"):
                    print(f"Skipping rare model: {f}")
                else:
                    print(f"Model: {f}")
                    model = f
            elif f.endswith("tranm") or f.endswith("gfbanm"):
                print(f"Anim: {f}")
                animations.append(f)
        elif os.path.isdir(f):
            model, animations = find_model_and_animations(f, model, animations)
    
    return model, animations


def parse_info(model, animations, output_path, item_name, colors):
    if model.endswith(".gfbmdl"):
        bpy.ops.import_scene.gfmdl(filepath=model)
    else:
        bpy.ops.import_scene.trmdl(filepath=model, rare=is_shiny)

    if export_colors:
        shiny = is_shiny
        create_texture_colors_data(item_name, colors, shiny)
        
        # remove old model and materials
        bpy.ops.wm.read_homefile(use_empty=True)
        bpy.ops.outliner.orphans_purge()
        
        shiny = not is_shiny
        bpy.ops.import_scene.trmdl(filepath=model, rare=shiny)

        create_texture_colors_data(item_name, colors, shiny)
    else:
        for anim in animations:
            bpy.ops.import_scene.gfbanm(filepath=anim)

        actions = bpy.data.actions
        for action in actions:
            if action.name.find("walk") != -1 or action.name.find("run") != -1 or action.name.find("turn") != -1:
                for fcurve in action.fcurves.values():
                    if fcurve.data_path.find("origin") != -1 and (fcurve.data_path.find("location") != -1 or fcurve.data_path.find("rotation_quaternion") != -1):
                        action.fcurves.remove(fcurve)
            #if is_pkmn_legends_arceus:
            for fcurve in action.fcurves.values():
                if (fcurve.data_path.find("origin") != -1 or fcurve.data_path.find("tongue")) and fcurve.data_path.find("scale") != -1:
                    action.fcurves.remove(fcurve)
            
        # export to GLFT
        bpy.ops.export_scene.gltf(filepath=f"{output_path}", export_unused_images=True, export_unused_textures=True, export_image_quality=100)
        print(f"Exported to {output_path}")

    # remove old model and materials
    bpy.ops.wm.read_homefile(use_empty=True)
    bpy.ops.outliner.orphans_purge()


def create_texture_colors_data(item_name, colors, shiny):
    shiny_name = "_rare" if shiny else ""
    for mat in bpy.data.materials:
        if "body" in mat.name \
        or"eye"in mat.name \
        or "tra" in mat.name \
        or "highlight" in mat.name \
        or "fire" in mat.name \
        or "smoke" in mat.name \
        or "boby" in mat.name: # boby is a mistake in the original files for golbat
            has_base_color = 'BaseColor' in bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs
            data = {
                "Name" : f"{item_name}_{mat.name}{shiny_name}",
                "BaseColor" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColor'].default_value[0] if has_base_color else '1.0'}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColor'].default_value[1] if has_base_color else '1.0'}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColor'].default_value[2] if has_base_color else '1.0'}",
                    "A": "1.0",
                },
                "BaseColorLayer1" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer1'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer1'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer1'].default_value[2]}",
                    "A": "1.0",
                },
                "BaseColorLayer2" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer2'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer2'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer2'].default_value[2]}",
                    "A": "1.0",
                },
                "BaseColorLayer3" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer3'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer3'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer3'].default_value[2]}",
                    "A": "1.0",
                },
                "BaseColorLayer4" : {
                    "R": f"{1.0 if bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[0] == 12312312.0 else bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[0]}",
                    "G": f"{1.0 if bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[1] == 12312312.0 else bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[1]}",
                    "B": f"{1.0 if bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[2] == 12312312.0 else bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['BaseColorLayer4'].default_value[2]}",
                    "A": "1.0",
                },
                "EmmColorLayer1" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer1'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer1'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer1'].default_value[2]}",
                    "A": "1.0",
                },
                "EmmColorLayer2" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer2'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer2'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer2'].default_value[2]}",
                    "A": "1.0",
                },
                "EmmColorLayer3" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer3'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer3'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer3'].default_value[2]}",
                    "A": "1.0",
                },
                "EmmColorLayer4" : {
                    "R": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer4'].default_value[0]}",
                    "G": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer4'].default_value[1]}",
                    "B": f"{bpy.data.materials[mat.name].node_tree.nodes['Group'].inputs['EmissionColorLayer4'].default_value[2]}",
                    "A": "1.0",
                },
            }

            colors.append(data)

def get_directory(export_type: ExportType) -> str:
    if export_type == ExportType.ScarletViolet:
        return directory_sv
    elif export_type == ExportType.PLA:
        return directory_pla
    elif export_type == ExportType.PLZA:
        return directory_plZA
    elif export_type == ExportType.SwSh:
        return directory_swsh
    return ""

def get_output_colors(export_type: ExportType) -> str:
    if export_type == ExportType.ScarletViolet:
        return f"{output}_colors.json"
    elif export_type == ExportType.PLA:
        return f"{output}_colors_pla.json"
    elif export_type == ExportType.PLZA:
        return f"{output}_colors_plZA.json"
    elif export_type == ExportType.SwSh:
        return f"{output}_colors_swsh.json"
    return ""

def is_valid_pokemon_to_export(id: int, export_type: ExportType) -> bool:
    # pla_pokemons = [41, 42, 46, 47, 63, 64, 65, 66, 67, 68, 77, 78, 95, 108, 114, 122, 169, 175, 176, 201, 208, 226]
    pla_pokemons = [201]
    plza_pokemons = [13, 14, 15, 16, 17, 18, 95, 115, 120, 121, 127, 142, 208]
    swsh_pokemons = [10]
    if export_type == ExportType.ScarletViolet:
        return True
    elif export_type == ExportType.PLA:
        return id in pla_pokemons
    elif export_type == ExportType.PLZA:
        return id in plza_pokemons
    elif export_type == ExportType.SwSh:
        return id in swsh_pokemons
    
    return False

def main():
    for i in range(1, 252):
        # if export_colors and ((i == 128 and not is_pkmn_legends_arceus) or (is_pkmn_legends_arceus and i not in pla_pokemons)):
        #     continue
        if not is_valid_pokemon_to_export(i, export_type):
            continue

        export_model(i, get_directory(export_type), output)

    if export_colors:
        output_colors = get_output_colors(export_type)
        with open(output_colors, "w+", encoding="utf-8") as file:
            json.dump(colors, file, indent=4)
            print(f"Exported {output_colors}")
            file.close()

    print("...FINISHED...")

main()
