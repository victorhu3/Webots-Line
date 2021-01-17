checkpoints = []
numRow = int(input('Number of Rows: '))
numCol = int(input('Number of Col: '))
worldName = input('Name of World (exclude .wbt): ')

print('\nFor tile numbers: Tile 0 = top left corner, Tile 1 = 1 tile to the right, etc.')
numCheckpoint = int(input('\nNumber of checkpoints: '))
header = open('worldHeader.txt', 'r')
file = open('../worlds/' + worldName + '.wbt', 'w')
file.write(header.read())

file.write('DEF Checkpoints Group {\n  children [\n')
for x in range(numCheckpoint):
    tile = int(input('Checkpoint ' + str(x + 1) + ' tile number: '))
    direction = input('Checkpoint ' + str(x + 1) + ' direction: ')
    file.write('    Solid {\n')
    file.write('      translation ' + str(int(tile % numCol) * 0.3 - 0.12) + ' 0 ' + str(int(tile / numCol) * 0.3 - 0.12) + '\n')
    file.write('      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 1 0.666667 0\n            }\n          }\n          geometry Cylinder {\n            height 0.02\n            radius 0.03\n          }\n        }\n      ]\n')
    file.write('      name \"checkpoint' + str(x) + '\"\n')
    file.write('      description \"' + direction.upper() + '\"\n    }\n')
file.write('  ]\n}\n')

print('\nOpen the Webots-Line\\tiles folder')
print('Each image has a number for its name')
print('Enter the number to select which image to use')
input('Press enter to begin...')

tileSize = 0.3 #in meters
dir = {}
imgNum = 0
orientation = ''
dir['N'] = dir['n'] = 0
dir['E'] = dir['e'] = 4.71
dir['S'] = dir['s'] = 3.15
dir['W'] = dir['w'] = 1.57
file.write('DEF Tiles Group {\n  children [\n    Solid {\n      translation')
file.write(' ' + str(numCol / 2 * 0.3 - 0.15) + ' 0 ' + str(numRow / 2 * 0.3 - 0.15) + '\n')
file.write('      boundingObject Plane {\n')
file.write('        size ' + str(numCol * 0.3) + ' ' + str(numRow * 0.3) + '\n      }\n    }\n')
for x in range(numRow):
    for y in range(numCol):
        print('\nTile #' + str(x * numCol + y) + ':')
        imgNum = input('Enter image number (Press enter for blank tile): ')
        if (imgNum != ''):
            imgNum = int(imgNum)
            orientation = input('Enter orientation (N, E, S, W): ')
        else:
            imgNum = '0'
            orientation = 'N'

        file.write('    Solid {\n      translation')
        file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')	#translation
        file.write('      rotation 0 1 0 ' + str(dir[orientation]) + '\n')		#rotation
        file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
        file.write(str(imgNum) + '.png\"\n')		#imgNum
        file.write('              ]\n            }\n          }\n          geometry Plane {\n')
        file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
        file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n    }\n')
file.write('  ]\n}')

file.close()
header.close()

#      boundingObject Shape {\n        geometry Plane {\n          size 0.3 0.3\n        }\n      }\n    