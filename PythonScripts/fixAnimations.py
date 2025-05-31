import bpy

actions = bpy.data.actions
needRemove = False
for action in actions:
    for fcurve in action.fcurves.values():
        if fcurve.data_path.find("origin") != -1 and (fcurve.data_path.find("location") != -1 or fcurve.data_path.find("rotation_quaternion") != -1):
            action.fcurves.remove(fcurve)

