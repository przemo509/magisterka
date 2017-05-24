import bpy  # biblioteki Blendera
import sys

argv = sys.argv
print("=====> All args:")
print(argv)

argv = argv[argv.index("--") + 1:]  # get all args after "--"
print("=====> My args:")
print(argv)

# Przepisanie parametrów do zmiennych o znaczących nazwach:
densityFilePath = argv[0]
densResX = int(argv[1])
densResY = int(argv[2])
densResZ = int(argv[3])
imgResX = int(argv[4])
imgResY = int(argv[5])
imgResPercentage = int(argv[6])
rayTracingStepSize = float(argv[7])
threads = int(argv[8])
configFilePath = argv[9]
additionalTextsToDisplay = argv[10:]

bpy.context.scene.objects.active = bpy.data.objects['domain']
bpy.data.textures["Tex"].voxel_data.filepath = densityFilePath
bpy.data.textures["Tex"].voxel_data.resolution[0] = densResX
bpy.data.textures["Tex"].voxel_data.resolution[1] = densResY
bpy.data.textures["Tex"].voxel_data.resolution[2] = densResZ
bpy.context.scene.objects.active.active_material.volume.step_size = rayTracingStepSize

bpy.context.scene.render.resolution_x = imgResX
bpy.context.scene.render.resolution_y = imgResY
bpy.context.scene.render.resolution_percentage = imgResPercentage

bpy.context.scene.render.threads = threads

bpy.context.scene.objects.active = bpy.data.objects['Text']
bpy.ops.object.editmode_toggle()
bpy.ops.font.text_paste_from_file(filepath=configFilePath)
for text in additionalTextsToDisplay:
    bpy.ops.font.line_break()
    bpy.ops.font.text_insert(text=text)
bpy.ops.object.editmode_toggle()
