import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageTk

class Tile:
    def __init__(self, tileNum, dir): #dir: 0=N, 1=E, 2=S, 3=W
        self.tileNum = tileNum
        self.dir = dir

def getImg(filePath):
    return Image.open(filePath)

def getPhoto(img):
    return ImageTk.PhotoImage(img)

def getGrid(x, y):
    return int((x - topLeftTile.winfo_x()) / tileImgSize + (y - topLeftTile.winfo_y()) / tileImgSize * numCol)

def placeTile(widget):
    global tiles
    filename = filedialog.askopenfilename()
    if filename != '':
        ind = getGrid(widget.winfo_x(), widget.winfo_y())
        tiles[ind] = getImg(filename).resize((100, 100))
        tilePhotos[ind] = getPhoto(tiles[ind])
        tileMap[ind].tileNum = int(filename[filename.rfind('/') + 1 : -4])
        tileMap[ind].dir = 0
        widget['image'] = tilePhotos[ind]

def rotateTile(event, widget):
    ind = getGrid(widget.winfo_x(), widget.winfo_y())
    if tiles[ind] != 0:
        tiles[ind] = tiles[ind].rotate(-90)
        tilePhotos[ind] = getPhoto(tiles[ind])
        tileMap[ind].dir = (tileMap[ind].dir + 1) % 4
        widget['image'] = tilePhotos[ind]

def placeBlank(event, widget):
    ind = getGrid(widget.winfo_x(), widget.winfo_y())
    tiles[ind] = getImg('../tiles/0.png').resize((100, 100))
    tilePhotos[ind] = getPhoto(tiles[ind])
    tileMap[ind].tileNum = 0
    widget['image'] = tilePhotos[ind]

def showInstructions():
    global instructionWindow, instructionText
    instructionWindow = tk.Toplevel(root)
    instruction = """
    Link to YouTube tutorial (written tutorial below):

    1. Left click on each grid to select a tile. To select a tile, navigate to the 
    /Webots-Line/tile folder using the file explorer that pops up and click 
    on the tile you want.

    2. Right click on each grid to rotate the selected tile 90 degrees 
    counterclockwise.

    3. Middle click on a grid to select a blank tile.

    Note: When entering tile numbers, the top left corner is tile number 0,
    the tile to its right is tile number 1, and etc. (Left-right, up-down order).

    4. To specify tiles with obstacles, type the tile numbers with obstacles into 
    the text box indicated separated by spaces. E.g. "1 3 18 19".

    5. To specify tiles with checkpoints, enter the following information in the
    specified form: "tile number,direction of entry,number of tiles since last 
    checkpoint". Add an asterisk to the end of the checkpoint entry for the last
    checkpoint before the evacuation room. Note that there are no spaces in one 
    checkpoint entry. Only separate different entries with a space. E.g. "2,E,3 5,N,12*".

        a. The "tile number" is the number of the tile the checkpoint is on.
        b. The "direction of entry" is the direction the robot enters the checkpoint 
    tile from. North=N, East=E, South = S, West = W.
        c. The "number of tiles since last checkpoint" is the number of tiles the
    robot has traveled since it arrived at the previous checkpoint (or since the 
    beginning if first checkpoint). This is only for scoring purposes and can be
    omitted (e.g. "2,E* 5,N").

    6. To specify tiles with speed bumps, enter the following information in the
    specified form: "tile number,direction of speed bump". Use the letters 'H' and
    'V' to specify horizontal or vertical for the speed bump direction. E.g. "4,H 12,V".

    7. To specify tiles with ramps, enter the following information in the specified form:
    "tile number,direction of entry". The direction of entry is what direction the robot
    will enter the tile from. Use 'N', 'E', 'S', or 'W' to denote north, east, south, west.
    E.g. "16,N 23,W".
                """
                    
    instructionText = tk.Text(instructionWindow, height=300, width=300)
    instructionText.pack()
    instructionText.insert(tk.END, instruction)

def makeWorld():
    path = ''
    tileChange = [-numCol, 1, numCol, -1]
    intersections = [11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24]
    gaps = [3, 10, 28, 29, 30]
    scoringInd = [0, 0, 0, 0, 0, 0]
    scoringElem = [[], [], [], [], [], [], []] #intersections, seesaws, obstacles, gaps, ramp, speed bump, checkpoints (checkpoint not indcluded in output string)
    intersection = 0
    seesaw = 1
    obstacle = 2
    gap = 3
    ramp = 4
    speedbump = 5
    #index 0 refers to 0.png, index 1 refers to 1.png, etc.
    #the exit direction when entering N, E, S, W, respectively
    tilePath = [[-1, -1, -1, -1], #0
        [-1, W, -1, E],
        [E, N, W, S],
        [-1, W, -1, E],
        [-1, S, E, -1],
        [-1, W, -1, E],
        [-1, W, -1, E],
        [-1, S, E, -1],
        [-1, W, -1, E],
        [-1, -1, W, S],
        [-1, W, -1, E], #10
        [-1, W, -1, E],
        [-1, S, E, E],
        [-1, W, W, S],
        [-1, S, S, S],
        [S, W, N, E],
        [S, S, E, E],
        [W, S, E, N],
        [S, S, S, S],
        [E, E, S, S],
        [-1, -1, -1, -1], #20
        [-1, W, -1, E],
        [-1, W, -1, E],
        [-1, S, E, -1],
        [-1, S, E, -1],
        [-1, -1, -1, -1],
        [-1, -1, -1, -1],
        [S, S, S, S],
        [-1, W, -1, E],
        [-1, W, -1, E],
        [-1, -1, W, S]] #30

    ind = 0
    curDir = E
    dirAngle = [0, 4.71, 3.14, 1.57]
    header = open('worldHeader.txt', 'r')
    file = open('../worlds/' + worldName + '.wbt', 'w')
    file.write(header.read())

    i = 0
    ind = 0
    obstacleList = str(obstacleEntry.get())
    file.write('DEF Obstacles Group {\n  children [')
    while i < len(obstacleList):
        j = i + 1
        while j != len(obstacleList) and obstacleList[j] != ' ':
            j = j + 1
        obsTileNum = int(obstacleList[i:j])
        scoringElem[obstacle].append(obsTileNum)
        i = j + 1
        file.write('    Solid {\n')
        file.write('      translation ' + str(int(obsTileNum % numCol) * 0.3) + ' 0.06 ' + str(int(obsTileNum / numCol) * 0.3) + '\n')
        file.write('      scale 1.3 1.3 1.3\n      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 0.2 0.2 0.8\n            }\n          }\n          geometry Cylinder {\n            height 0.1\n            radius 0.05\n          }\n        }\n      ]\n')
        file.write('      name \"obstacle' + str(ind + 1) + '\"\n')
        file.write('      boundingObject Shape {\n        appearance Appearance {\n          material Material {\n            diffuseColor 0.2 0.2 0.8\n          }\n        }\n        geometry Cylinder {\n          height 0.1\n          radius 0.05\n        }\n      }\n      physics Physics {\n        mass 7\n      }\n    }\n')
        ind = ind + 1
    file.write(' ]\n}\n')

    i = 0
    ind = 0
    speedbumpList = str(speedbumpEntry.get())
    speedbumpRot = {
        'H': ['0.707', '-0.707', '0', '-3.14'],
        'V': ['-0.577', '0.577', '-0.577', '2.09']
    }
    file.write('DEF Speedbumps Group {\n  children [\n')
    while i < len(speedbumpList):
        j = i + 1
        while j != len(speedbumpList) and speedbumpList[j] != ',':
            j = j + 1
        sbNum = int(speedbumpList[i:j])
        sbDir = speedbumpList[j + 1].upper()
        scoringElem[speedbump].append(sbNum)
        i = j + 3
        file.write('    speedbump {\n')
        file.write('      translation ' + str(int(sbNum % numCol) * 0.3) + ' 0 ' + str(int(sbNum / numCol) * 0.3) + '\n')
        file.write('      rotation ' + speedbumpRot[sbDir][0] + ' ' + speedbumpRot[sbDir][1] + ' ' + speedbumpRot[sbDir][2] + ' ' + speedbumpRot[sbDir][3] + '\n')
        file.write('      name \"speedbump' + str(ind) + '\"\n    }\n')
        ind = ind + 1
    file.write('  ]\n}\n')

    i = 0
    ind = 0
    seesawList = str(seesawEntry.get())
    seesawRot = {
        'N': '-3.14',
        'E': '1.57',
        'S': '0',
        'W': '-1.57'
    }
    file.write('DEF Seesaw Group {\n  children [\n')
    while i < len(seesawList):
        j = i + 1
        while j != len(seesawList) and seesawList[j] != ',':
            j = j + 1
        ssNum = int(seesawList[i:j])
        ssDir = seesawList[j + 1].upper()
        scoringElem[seesaw].append(ssNum)
        i = j + 3
        file.write('    seesaw {\n')
        file.write('      translation ' + str(int(ssNum % numCol) * 0.3) + ' 0 ' + str(int(ssNum / numCol) * 0.3) + '\n')
        file.write('      rotation 0 1 0 ' + seesawRot[ssDir] + '\n')
        file.write('      name \"seesaw' + str(ind) + '\"\n    }\n')
        ind = ind + 1
    file.write('  ]\n}\n')
    print('World ' + worldName + '.wbt Generated')

    evacEntrance = -1
    ind = 0
    path += str(numRow) + ',' + str(numCol) + ',' + ';'
    path += '0,'
    pathList = [0]
    while curDir != -1:
        if tileMap[ind].tileNum == 26:
            evacEntrance = len(pathList) - 1
            for i in range(len(tileMap)):
                if tileMap[i].tileNum == 27:
                    ind = i
                    path += str(ind) + ','
                    pathList.append(ind)
                    curDir = tileMap[ind].dir
                    break
        else:
            if ind != 0:
                curDir = tilePath[tileMap[ind].tileNum][(curDir - tileMap[ind].dir + 6) % 4]
            if curDir != -1:
                if ind != 0:
                    curDir = (curDir + tileMap[ind].dir) % 4
                ind += tileChange[curDir]
                path += str(ind) + ','
                pathList.append(ind)
        if intersections.count(tileMap[ind].tileNum) > 0:
            scoringElem[intersection].append(ind)
        if gaps.count(tileMap[ind].tileNum) > 0:
            scoringElem[gap].append(ind)
        breakVar = False
        for i in range(len(scoringElem)):
            if i != intersection and i != gap:
                for j in range(len(scoringElem[i])):
                    if ind == scoringElem[i][j]:
                        tmp = scoringElem[i][j]
                        scoringElem[i][j] = scoringElem[i][scoringInd[i]]
                        scoringElem[i][scoringInd[i]] = tmp
                        scoringInd[i] = scoringInd[i] + 1
                        breakVar = True
                        break
            if breakVar:
                break
    path += ';'
    for i in range(len(scoringElem)):
        for j in range(len(scoringElem[i])):
            path += str(scoringElem[i][j]) + ','
        path += ';'
    path += ';,,;,;,;,;'
    print(path)

    file.write('DEF Tiles Group {\n  children [\n    Solid {\n')
    file.write('      description \"' + path + '\"      translation')
    file.write(' ' + str(numCol / 2 * 0.3 - 0.15) + ' 0 ' + str(numRow / 2 * 0.3 - 0.15) + '\n')
    file.write('      boundingObject Plane {\n')
    file.write('        size ' + str(numCol * 0.3) + ' ' + str(numRow * 0.3) + '\n      }\n    }\n')
    for x in range(numRow):
        for y in range(numCol):
            ind = x * numCol + y
            file.write('    Solid {\n      translation')
            file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')	#translation
            file.write('      rotation 0 1 0 ' + str(dirAngle[tileMap[ind].dir]) + '\n')		#rotation
            file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
            file.write(str(tileMap[ind].tileNum) + '.png\"\n')		#imgNum
            file.write('              ]\n            }\n          }\n          geometry Plane {\n')
            file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
            file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n    }\n')
    file.write('  ]\n}')

    i = 0
    ind = 0
    preEvac = 0
    checkpointList = str(checkpointEntry.get())
    prevCheck = -1
    file.write('DEF Checkpoints Group {\n  children [\n')
    print(pathList)
    while i < len(checkpointList):
        j = i + 1
        while checkpointList[j] != ',':
            j = j + 1
        checkpointNum = int(checkpointList[i:j])
        checkpointDir = checkpointList[j + 1]
        i = j + 3
        checkInd = pathList.index(checkpointNum)
        checkpointDist = checkInd - prevCheck
        prevCheck = checkInd
        if preEvac == 0 and i < len(checkpointList): #************************************ find evac exit
            k = i
            while checkpointList[k] != ',':
                k = k + 1
            print(int(checkpointList[i:k]), pathList.index(int(checkpointList[i:k])), evacEntrance)
            if checkInd <= evacEntrance and pathList.index(int(checkpointList[i:k])) > evacEntrance:
                preEvac = preEvac + 1
        file.write('    Solid {\n')
        file.write('      translation ' + str(int(checkpointNum % numCol) * 0.3 - 0.12) + ' 0 ' + str(int(checkpointNum / numCol) * 0.3 - 0.12) + '\n')
        file.write('      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 1 0.666667 0\n            }\n          }\n          geometry Cylinder {\n            height 0.02\n            radius 0.03\n          }\n        }\n      ]\n')
        file.write('      name \"checkpoint' + str(ind) + '\"\n')
        file.write('      description \"' + checkpointDir.upper() + str(checkpointDist))
        if preEvac == 2:
            file.write('-')
            preEvac = preEvac + 1
        if preEvac == 1:
            file.write('*')
            preEvac = preEvac + 1
        file.write('\"\n    }\n')
        ind = ind + 1
    file.write('  ]\n}\n')

def enterTiles():
    global worldName, numRow, numCol, mapWindow, mapCanvas, rotationVar, rotationLabel, placeButton, rotateButton, tileMap, tiles, tilePhotos, obstacleLabel, checkpointLabel, obstacleEntry, checkpointEntry, speedbumpLabel, seesawLabel, speedbumpEntry, seesawEntry, topLeftTile
    offset = 33
    worldName = str(worldNameEntry.get())
    numRow = int(numRowEntry.get())
    numCol = int(numColEntry.get())
    tileMap = [Tile(0, 0) for i in range(numRow * numCol)]
    tiles = [0] * (numRow * numCol)
    tilePhotos = [0] * (numRow * numCol)

    mapWindow = tk.Toplevel(root)
    mapCanvas = tk.Canvas(mapWindow, width=numCol * tileImgSize, height=(numRow) * tileImgSize + offset + 120)
    mapCanvas.pack()

    x = numCol * tileImgSize / 2
    y = numRow * (tileImgSize + 1) + tileImgSize / 2 + 10
    obstacleLabel = tk.Label(mapWindow, text='Obstacle tiles')
    checkpointLabel = tk.Label(mapWindow, text='Checkpoint tiles')
    speedbumpLabel = tk.Label(mapWindow, text='Speed bump tiles')
    seesawLabel = tk.Label(mapWindow, text='Ramp tiles')
    mapCanvas.create_window(x - 180, y, window=obstacleLabel)
    mapCanvas.create_window(x - 180, y + 30, window=checkpointLabel)
    mapCanvas.create_window(x + 80, y, window=speedbumpLabel)
    mapCanvas.create_window(x + 80, y + 30, window=seesawLabel)

    obstacleEntry = tk.Entry(mapWindow, justify='center')
    checkpointEntry = tk.Entry(mapWindow, justify='center')
    speedbumpEntry = tk.Entry(mapWindow, justify='center')
    seesawEntry = tk.Entry(mapWindow, justify='center')
    mapCanvas.create_window(x - 60, y, window=obstacleEntry)
    mapCanvas.create_window(x - 60, y + 30, window=checkpointEntry)
    mapCanvas.create_window(x + 200, y, window=speedbumpEntry)
    mapCanvas.create_window(x + 200, y + 30, window=seesawEntry)

    makeWorldButton = tk.Button(mapWindow, text='Generate World', command=makeWorld)
    mapCanvas.create_window(x, y + 63, window=makeWorldButton)
    instructionButton = tk.Button(mapWindow, text='Click Here for Instructions', command=showInstructions)
    mapCanvas.create_window(x, 17, window=instructionButton)

    for x in range(numRow):
        for y in range(numCol):
            selectTileButton = tk.Button(mapWindow, image=selectTilePhoto)
            selectTileButton['command'] = lambda w=selectTileButton: placeTile(w)
            selectTileButton.bind('<Button-2>', (lambda event=None, widget=selectTileButton: placeBlank(event, widget)))
            selectTileButton.bind('<Button-3>', (lambda event=None, widget=selectTileButton: rotateTile(event, widget)))
            mapCanvas.create_window(y * tileImgSize, offset + x * tileImgSize, anchor=tk.NW, window=selectTileButton)
            if x == 0 and y == 0:
                topLeftTile = selectTileButton

N = 0
E = 1
S = 2
W = 3

tileOptions = 30
tileImgSize = 100
tileSize = 0.3
numRow = 0
numCol = 0
rotation = False

tiles = None
tilePhotos = None
tileMap = None
rotationVar = None
rotationLabel = None
placeButton = None
rotateButton = None
obstacleLabel = None
checkpointLabel = None
speedbumpLabel = None
seesawLabel = None
obstacleEntry = None
checkpointEntry = None
speedbumpEntry = None
seesawEntry = None
instructionWindow = None
instructionButton = None
instructionText = None
topLeftTile = None

root = tk.Tk()
rootCanvas = tk.Canvas(root, width = 330, height = 100)
rootCanvas.pack()
mapWindow = None
mapCanvas = None

selectTileImg = getImg('GUI/selectTile.png')
selectTilePhoto = getPhoto(selectTileImg)

worldNameLabel = tk.Label(root, text='World Name (exclude .wbt)')
numRowLabel = tk.Label(root, text='Number of Rows')
numColLabel = tk.Label(root, text='Number of Columns')
enterButton = tk.Button(root, text='Enter', command=enterTiles)
rootCanvas.create_window(80, 20, window=worldNameLabel)
rootCanvas.create_window(80, 40, window=numRowLabel)
rootCanvas.create_window(80, 60, window=numColLabel)
rootCanvas.create_window(240, 87, window=enterButton)

worldNameEntry = tk.Entry(root, justify='center')
numRowEntry = tk.Entry(root, justify='center')
numColEntry = tk.Entry(root, justify='center')
rootCanvas.create_window(240, 20, window=worldNameEntry)
rootCanvas.create_window(240, 40, window=numRowEntry)
rootCanvas.create_window(240, 60, window=numColEntry)

tk.mainloop()