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

def placeTile(widget):
    global tiles
    filename = filedialog.askopenfilename()
    if filename != '':
        ind = int(widget.winfo_x() / tileImgSize + widget.winfo_y() / tileImgSize * numCol) - 1
        tiles[ind] = getImg(filename).resize((100, 100))
        tilePhotos[ind] = getPhoto(tiles[ind])
        map[ind].tileNum = int(filename[filename.rfind('/') + 1 : -4])
        widget['image'] = tilePhotos[ind]

def rotateTile(event, widget):
    ind = int(widget.winfo_x() / tileImgSize + widget.winfo_y() / tileImgSize * numCol) - 1
    if tiles[ind] != 0:
        tiles[ind] = tiles[ind].rotate(-90)
        tilePhotos[ind] = getPhoto(tiles[ind])
        map[ind].dir = (map[ind].dir + 1) % 4
        widget['image'] = tilePhotos[ind]

def placeBlank(event, widget):
    ind = int(widget.winfo_x() / tileImgSize + widget.winfo_y() / tileImgSize * numCol) - 1
    tiles[ind] = getImg('../tiles/0.png').resize((100, 100))
    tilePhotos[ind] = getPhoto(tiles[ind])
    map[ind].tileNum = 0
    widget['image'] = tilePhotos[ind]

def showInstructions():
    global instructionWindows, instructionText
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
    checkpoint". Note that there are no spaces in one checkpoint entry. Only 
    separate different entries with a space. E.g. "2,E,3 5,N,12".

        a. The "tile number" is the number of the tile the checkpoint is on.
        b. The "direction of entry" is the direction the robot enters the checkpoint 
    tile from. North=N, East=E, South = S, West = W.
        c. The "number of tiles since last checkpoint" is the number of tiles the
    robot has traveled since it arrived at the previous checkpoint (or since the 
    beginning if first checkpoint). This is only for scoring purposes and can be
    omitted.
                """
                    
    instructionText = tk.Text(instructionWindow, height=300, width=300)
    instructionText.pack()
    instructionText.insert(tk.END, instruction)

def makeWorld():
    ind = 0
    dirAngle = [0, 4.71, 3.14, 1.57]
    header = open('worldHeader.txt', 'r')
    file = open('../worlds/' + worldName + '.wbt', 'w')
    file.write(header.read())

    file.write('DEF Tiles Group {\n  children [\n    Solid {\n      translation')
    file.write(' ' + str(numCol / 2 * 0.3 - 0.15) + ' 0 ' + str(numRow / 2 * 0.3 - 0.15) + '\n')
    file.write('      boundingObject Plane {\n')
    file.write('        size ' + str(numCol * 0.3) + ' ' + str(numRow * 0.3) + '\n      }\n    }\n')
    for x in range(numRow):
        for y in range(numCol):
            ind = x * numCol + y
            file.write('    Solid {\n      translation')
            file.write(' ' + str(y * tileSize) + ' 0 ' + str(x * tileSize) + '\n')	#translation
            file.write('      rotation 0 1 0 ' + str(dirAngle[map[ind].dir]) + '\n')		#rotation
            file.write('      children [\n        Shape{\n          appearance Appearance{\n            texture ImageTexture{\n              url[\n                \"../tiles/')
            file.write(str(map[ind].tileNum) + '.png\"\n')		#imgNum
            file.write('              ]\n            }\n          }\n          geometry Plane {\n')
            file.write('            size ' + str(tileSize) + ' ' + str(tileSize) + '\n') #tile size
            file.write('          }\n        }\n      ]\n      name \"solid' + str(x * numCol + y) + '\"\n    }\n')
    file.write('  ]\n}')

    i = 0
    obstacleList = str(obstacleEntry.get())
    file.write('DEF Obstacles Group {\n  children [')
    while i < len(obstacleList):
        j = i + 1
        while j != len(obstacleList) and obstacleList[j] != ' ':
            j = j + 1
        obsTileNum = int(obstacleList[i:j])
        i = j + 1
        file.write('    Solid {\n')
        file.write('      translation ' + str(int(obsTileNum % numCol) * 0.3) + ' 0.06 ' + str(int(obsTileNum / numCol) * 0.3) + '\n')
        file.write('      scale 1.3 1.3 1.3\n      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 0.2 0.2 0.8\n            }\n          }\n          geometry Cylinder {\n            height 0.1\n            radius 0.05\n          }\n        }\n      ]\n')
        file.write('      name \"obstacle' + str(x + 1) + '\"\n')
        file.write('      boundingObject Shape {\n        appearance Appearance {\n          material Material {\n            diffuseColor 0.2 0.2 0.8\n          }\n        }\n        geometry Cylinder {\n          height 0.1\n          radius 0.05\n        }\n      }\n      physics Physics {\n        mass 7\n      }\n    }\n')
    file.write(' ]\n}\n')

    i = 0
    checkpointList = str(checkpointEntry.get())
    file.write('DEF Checkpoints Group {\n  children [\n')
    while i < len(checkpointList):
        j = i + 1
        while checkpointList[j] != ',':
            j = j + 1
        checkpointNum = int(checkpointList[i:j])
        checkpointDir = checkpointList[j + 1]
        checkpointDist = '1'
        k = j + 2
        if k != len(checkpointList) and checkpointList[k] != ' ':
            k = k + 1
            while k != len(checkpointList) and checkpointList[k] != ' ':
                k = k + 1
            checkpointDist = checkpointList[j + 3:k]
        i = k + 1
        file.write('    Solid {\n')
        file.write('      translation ' + str(int(checkpointNum % numCol) * 0.3 - 0.12) + ' 0 ' + str(int(checkpointNum / numCol) * 0.3 - 0.12) + '\n')
        file.write('      children [\n        Shape {\n          appearance Appearance {\n            material Material {\n              diffuseColor 1 0.666667 0\n            }\n          }\n          geometry Cylinder {\n            height 0.02\n            radius 0.03\n          }\n        }\n      ]\n')
        file.write('      name \"checkpoint' + str(x) + '\"\n')
        file.write('      description \"' + checkpointDir.upper() + checkpointDist + '\"\n    }\n')
    file.write('  ]\n}\n')
    print('World ' + worldName + '.wbt Generated')

def enterTiles():
    global worldName, numRow, numCol, mapWindow, mapCanvas, rotationVar, rotationLabel, placeButton, rotateButton, map, obstacleLabel, checkpointLabel, obstacleEntry, checkpointEntry
    offset = 33
    worldName = str(worldNameEntry.get())
    numRow = int(numRowEntry.get())
    numCol = int(numColEntry.get())
    map = [Tile(0, 0) for i in range(numRow * numCol)]

    mapWindow = tk.Toplevel(root)
    mapCanvas = tk.Canvas(mapWindow, width=numCol * tileImgSize, height=(numRow) * tileImgSize + offset + 120)
    mapCanvas.pack()

    x = numCol * tileImgSize / 2
    y = numRow * (tileImgSize + 1) + tileImgSize / 2 + 10
    obstacleLabel = tk.Label(mapWindow, text='Obstacle tiles')
    checkpointLabel = tk.Label(mapWindow, text='Checkpoint tiles')
    mapCanvas.create_window(x - 55, y, window=obstacleLabel)
    mapCanvas.create_window(x - 55, y + 30, window=checkpointLabel)
    obstacleEntry = tk.Entry(mapWindow, justify='center')
    checkpointEntry = tk.Entry(mapWindow, justify='center')
    mapCanvas.create_window(x + 55, y, window=obstacleEntry)
    mapCanvas.create_window(x + 55, y + 30, window=checkpointEntry)

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

tileOptions = 26
tileImgSize = 100
tileSize = 0.3
numRow = 0
numCol = 0
rotation = False

tiles = [0] * tileOptions
tilePhotos = [0] * tileOptions
map = None
rotationVar = None
rotationLabel = None
placeButton = None
rotateButton = None
obstacleLabel = None
checkpointLabel = None
obstacleEntry = None
checkpointEntry = None
instructionWindow = None
instructionButton = None
instructionText = None

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