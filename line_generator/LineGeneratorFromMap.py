print('\nTo create world without an existing map, run LineGenerator.py instead')
mapName = input('Name of map to be used to create the world (exclude .txt): ')
worldName = input('Name of world to be created (exclude .wbt): ')

header = open('worldHeader.txt', 'r')
file = open('../worlds/' + worldName + '.wbt', 'w')
file.write(header.read())
mapFile = open('maps/' + mapName + '.txt','r')

#initial splitting of map into commands
map = mapFile.read()
map = map.split('\n')
if map[len(map)-1] == '':
    map = map[0:len(map)-1]
i = 0
while i < len(map):
    if len(map[i]) < 5 or map[i][0:3] != 'Row':#get rid of extraneous rows
        del map[i]
        continue
    map[i] = map[i].replace(' ','')
    map[i] = map[i][5:len(map[i])]
    map[i] = map[i].split(',')
    i += 1

#calc # of rows and columns
numRow = len(map)
numCol = 0
lastElem = len(map[0])-1
if map[0][lastElem].find('-') != -1:
    dashLoc = map[0][lastElem].find('-')
    colonLoc = map[0][lastElem].find(':')
    numCol = int(map[0][lastElem][dashLoc+1:colonLoc])
else:
    colonLoc = map[0][lastElem].find(':')
    numCol = int(map[0][lastElem][0:colonLoc])

#split condensed commands
for i in range(0,len(map)):
    for j in range(0,len(map[i])):
        if map[i][j].find('-') != -1:
            dashLoc = map[i][j].find('-')
            colonLoc = map[i][j].find(':')
            startCol = int(map[i][j][0:dashLoc])
            endCol = int(map[i][j][dashLoc+1:colonLoc])
            tileNumOr = map[i][j][(colonLoc+1):len(map[i][j])]
            for k in range(0,endCol - startCol):
                currCol = startCol + k
                map[i].append(str(currCol) + ':' + tileNumOr)
            map[i][j] = str(endCol) + ':' + tileNumOr
    
#print(numCol)    
#print(map)

tileSize = 0.3 #in meters
dir = {}
imgNum = 0
walls = []
generateBounds = False#to create walls that bound the field
manualWalls = False#manually input location of walls 
boundWallHeight = 0.15

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
    for colIdx in range(len(map[x])):
        #extract info from command
        colonLoc = map[x][colIdx].find(':')
        y = int(map[x][colIdx][0:colonLoc])
        imgNum = int(map[x][colIdx][(colonLoc+1):(len(map[x][colIdx])-1)])
        orientation = map[x][colIdx][(len(map[x][colIdx])-1):len(map[x][colIdx])]
        wallOrientations = []
        if generateBounds:
            if (not ('N' in wallOrientations)) and x == 0:
                walls.append([x * numCol + y,'N',boundWallHeight])
            if (not ('S' in wallOrientations)) and x == numRow-1:
                walls.append([x * numCol + y,'S',boundWallHeight])
            if (not ('W' in wallOrientations)) and y == 0:
                walls.append([x * numCol + y,'W',boundWallHeight])
            if (not ('E' in wallOrientations)) and y == numRow-1:
                walls.append([x * numCol + y,'E',boundWallHeight])
        file.write('    Solid {\n      translation')
        file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')    #translation
        file.write('      rotation 0 1 0 ' + str(dir[orientation]) + '\n')        #rotation
        file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
        file.write(str(imgNum) + '.png\"\n')        #imgNum
        file.write('              ]\n            }\n          }\n          geometry Plane {\n')
        file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
        file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n    }\n')
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

print('World has been created based on the map \n')

mapFile.close()
file.close()
header.close()