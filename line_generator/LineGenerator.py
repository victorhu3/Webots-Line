numRow = int(input('Number of Rows: '))
numCol = int(input('Number of Col: '))
worldName = input('Name of World (exclude .wbt): ')
print('\nOpen the Webots-Line\tiles folder')
print('Each image has a number for its name')
print('Enter the number to select which image to use')
print('Tile 0 = top left corner, Tile 1 = move 1 tile to the right, etc.')
input('Press enter to begin...')

header = open('worldHeader.txt', 'r')
file = open('../worlds/' + worldName + '.wbt', 'w')
file.write(header.read())

tileSize = 0.3 #in meters
dir = {}
imgNum = 0
orientation = ''
dir['N'] = dir['n'] = 0
dir['E'] = dir['e'] = 4.71
dir['S'] = dir['s'] = 3.15
dir['W'] = dir['w'] = 1.57
for x in range(numRow):
    for y in range(numCol):
        print('\nTile #' + str(x * numCol + y) + ':')
        imgNum = input('Enter image number (Press enter for empty tile): ')
        if (imgNum != ''):
            imgNum = int(imgNum)
            orientation = input('Enter orientation (N, E, S, W): ')

            file.write('    Solid {\n      translation')
            file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')	#translation
            file.write('      rotation 0 1 0 ' + str(dir[orientation]) + '\n')		#rotation
            file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
            file.write(str(imgNum) + '.png\"\n')		#imgNum
            file.write('              ]\n            }\n          }\n          geometry Plane {\n')
            file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
            file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n      boundingObject Shape {\n        geometry Plane {\n          size 0.3 0.3\n        }\n      }\n    }\n')
file.write('  ]\n}')

file.close()
header.close()