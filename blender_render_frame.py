import bpy
import sys

argv = sys.argv
print("=====> All args:")
print(argv)

argv = argv[argv.index("--") + 1:]  # get all args after "--"
print("=====> My args:")
print(argv)

densityFilePath = argv[0]
xRes = int(argv[1])
yRes = int(argv[2])
zRes = int(argv[3])
rayTracingStepSize = float(argv[4])
configFilePath = argv[5]
additionalTextsToDisplay = argv[6:]

bpy.context.scene.objects.active = bpy.data.objects['domain']
bpy.data.textures["Tex"].voxel_data.filepath = densityFilePath
bpy.data.textures["Tex"].voxel_data.resolution[0] = xRes
bpy.data.textures["Tex"].voxel_data.resolution[1] = yRes
bpy.data.textures["Tex"].voxel_data.resolution[2] = zRes
bpy.context.scene.objects.active.active_material.volume.step_size = rayTracingStepSize

bpy.context.scene.objects.active = bpy.data.objects['Text']
bpy.ops.object.editmode_toggle()
bpy.ops.font.text_paste_from_file(filepath=configFilePath)
for text in additionalTextsToDisplay:
    bpy.ops.font.line_break()
    bpy.ops.font.text_insert(text=text)
bpy.ops.object.editmode_toggle()
