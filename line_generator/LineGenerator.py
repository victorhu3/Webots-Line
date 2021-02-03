checkpoints = []
print('\nTo create world from an existing map, run LineGeneratorFromMap.py instead')
numRow = int(input('Number of rows: '))
numCol = int(input('Number of col: '))
worldName = input('Name of world to be created (exclude .wbt): ')
mapName = input('Name of map to be created from your inputs (exclude .wbt): ')

print('\nFor tile numbers: Tile 0 = top left corner, Tile 1 = 1 tile to the right, etc.')
header = open('worldHeader.txt', 'r')
file = open('../worlds/' + worldName + '.wbt', 'w')
file.write(header.read())
mapFile = open('maps/' + mapName + '.txt','w')
mapFile.write('Maintain current formatting when making modifications. Sample formatting for entries:\n')
mapFile.write('   0:3e where 0 is the column number, 3 is the tile number, and e is the orientation\n')
mapFile.write('   2-4:5n is equivalent to 2:5n,3:5n,4:5n\n\n' + str(numRow) + 'X' + str(numCol) + '\n')


print('\nOpen the Webots-Line\\tiles folder')
print('Each image has a number for its name')
print('Enter the number to select which image to use')
input('Press enter to begin...')

tileSize = 0.3 #in meters
dir = {}
imgNum = 0
walls = []
generateBounds = False#to create walls that bound the field
manualWalls = False#manually input location of walls 
boundWallHeight = 0.15

#evacNWTile = input('row and column of top left tile of evac room in format row,col (first row/col is row/col 0):').split(',')
#evacSETile = input('row and column of bottom right tile of evac room in format row,col:').split(',')



orientation = ''
dir['N'] = dir['n'] = 0
dir['E'] = dir['e'] = 4.71
dir['S'] = dir['s'] = 3.15
dir['W'] = dir['w'] = 1.57
file.write('DEF Tiles Group {\n  children [\n    Solid {\n      translation')
file.write(' ' + str(numCol / 2 * 0.3 - 0.15) + ' 0 ' + str(numRow / 2 * 0.3 - 0.15) + '\n')
file.write('      boundingObject Plane {\n')
file.write('        size ' + str(numCol * 0.3) + ' ' + str(numRow * 0.3) + '\n      }\n    }\n')

#variables for map storage
for x in range(numRow):
    mapFile.write('Row ' + str(x) + ': ')
    prevImg = 0
    prevImgOr = ''
    prevIdx = -1
    for y in range(numCol):
        print('\nTile ' + str(x * numCol + y) + ' at (' + str(x) + ',' + str(y) + '):')
        imgNum = input('Enter image number (Press enter for blank tile): ')
        if (imgNum != ''):
            imgNum = int(imgNum)
            orientation = input('Enter orientation (N, E, S, W): ')
            if prevIdx == -1:#first image of column
                prevIdx = y 
                prevImg = imgNum
                prevImgOr = orientation
            else:
                if (prevImg != imgNum) or (prevImgOr != orientation):
                    if y - 1 == prevIdx:
                        mapFile.write(str(prevIdx) + ':' + str(prevImg) + str(prevImgOr) + ', ')
                    else:#condensing formatting for repeated same image needed
                        mapFile.write(str(prevIdx) + '-' + str(y - 1) + ':' + str(prevImg) + str(prevImgOr) + ', ')
                    prevIdx = y 
                    prevImg = imgNum
                    prevImgOr = orientation
            wallOrientations = []
            if manualWalls:
                wallStr = input('Enter wall location(s), separated by commas (N, E, S, W), or leave blank for no wall:')
                wallOrientations = wallStr.split(',')
                for wallOrientation in wallOrientations:
                    if(wallOrientation == 'N' or wallOrientation == 'S' or wallOrientation == 'W' or wallOrientation == 'E'):
                        walls.append([x * numCol + y,wallOrientation,0.15])#location, orientation, height
            if generateBounds:
                if (not ('N' in wallOrientations)) and x == 0:
                    walls.append([x * numCol + y,'N',boundWallHeight])
                if (not ('S' in wallOrientations)) and x == numRow-1:
                    walls.append([x * numCol + y,'S',boundWallHeight])
                if (not ('W' in wallOrientations)) and y == 0:
                    walls.append([x * numCol + y,'W',boundWallHeight])
                if (not ('E' in wallOrientations)) and y == numRow-1:
                    walls.append([x * numCol + y,'E',boundWallHeight])
        else:
            imgNum = '0'
            orientation = 'N'
        file.write('    Solid {\n      translation')
        file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')    #translation
        file.write('      rotation 0 1 0 ' + str(dir[orientation]) + '\n')        #rotation
        file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
        file.write(str(imgNum) + '.png\"\n')        #imgNum
        file.write('              ]\n            }\n          }\n          geometry Plane {\n')
        file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
        file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n    }\n')
    if y == prevIdx:
        mapFile.write(str(prevIdx) + ':' + str(prevImg) + str(prevImgOr) + '\n')
    else:#condensing formatting for repeated same image needed
        mapFile.write(str(prevIdx) + '-' + str(y) + ':' + str(prevImg) + str(prevImgOr) + '\n')
file.write('  ]\n}')

numCheckpoint = int(input('\n\nNumber of checkpoints: '))
file.write('DEF Checkpoints Group {\n  children [\n')
for x in range(numCheckpoint):
    tile = int(input('Checkpoint ' + str(x + 1) + ' tile number: '))
    direction = input('Checkpoint ' + str(x + 1) + ' direction: ')
    if x == 0:
        tileBetween = str(input('Input the number of tiles between start tile and checkpoint 1: '))
    else:
        tileBetween = str(input('Input the number of tiles between checkpoints ' + str(x) + ' and ' + str(x + 1) + ': '))

    file.write('    Solid {\n')
    file.write('      translation ' + str(int(tile % numCol) * 0.3 - 0.12) + ' 0 ' + str(int(tile / numCol) * 0.3 - 0.12) + '\n')
    file.write('      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 1 0.666667 0\n            }\n          }\n          geometry Cylinder {\n            height 0.02\n            radius 0.03\n          }\n        }\n      ]\n')
    file.write('      name \"checkpoint' + str(x) + '\"\n')
    file.write('      description \"' + direction.upper() + tileBetween + '\"\n    }\n')
file.write('  ]\n}\n')

numObs = int(input('\n\nNumber of obstacles: '))
file.write('DEF Obstacles Group {\n  children [\n')
for x in range(numObs):
    tile = int(input('Obstacle ' + str(x + 1) + ' tile number: '))
    file.write('    Solid {\n')
    file.write('      translation ' + str(int(tile % numCol) * 0.3) + ' 0.06 ' + str(int(tile / numCol) * 0.3) + '\n')
    file.write('      scale 1.3 1.3 1.3\n      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 0.2 0.2 0.8\n            }\n          }\n          geometry Cylinder {\n            height 0.1\n            radius 0.05\n          }\n        }\n      ]\n')
    file.write('      name \"obstacle' + str(x + 1) + '\"\n')
    file.write('      boundingObject Shape {\n        appearance Appearance {\n          material Material {\n            diffuseColor 0.2 0.2 0.8\n          }\n        }\n        geometry Cylinder {\n          height 0.1\n          radius 0.05\n        }\n      }\n      physics Physics {\n        mass 7\n      }\n    }\n')
for y in range(len(walls)):
    file.write('    Wall {\n')
    if walls[y][1] == 'N':
        file.write('      translation ' + str(int(walls[y][0] % numCol) * 0.3) + ' 0 ' + str(int(walls[y][0] / numCol) * 0.3 - 0.15) + '\n')
    elif walls[y][1] == 'S':
        file.write('      translation ' + str(int(walls[y][0] % numCol) * 0.3) + ' 0 ' + str(int(walls[y][0] / numCol) * 0.3 + 0.15) + '\n')
    elif walls[y][1] == 'W':
        file.write('      translation ' + str(int(walls[y][0] % numCol) * 0.3 - 0.15) + ' 0 ' + str(int(walls[y][0] / numCol) * 0.3) + '\n')
    else:
        file.write('      translation ' + str(int(walls[y][0] % numCol) * 0.3 + 0.15) + ' 0 ' + str(int(walls[y][0] / numCol) * 0.3) + '\n')
    if(walls[y][1] == 'N' or walls[y][1] == 'S'):
        file.write('      rotation 0 1 0 0 \n')
    else:
        file.write('      rotation 0 1 0 -1.57 \n')
    file.write('      size 0.3 ' + str(walls[y][2]) + ' 0.01\n      appearance Roughcast {\n        colorOverride 1 1 1\n        textureTransform TextureTransform {\n          scale 1 2.4\n        }\n      }\n    }\n')
file.write(' ]\n}\n')

mapFile.close()
file.close()
header.close()

print('\nTo modify the existing world\'s map, go to the maps folder, select and modify the appropriate map, and then run LineGeneratorFromMap.py')
