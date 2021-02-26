from numbers import Number

LM = None
RM = None
finger1 = None
leftFrontMotor = None
confirmGreen = None
colorVal = None
finger2 = None
arm2 = None
leftBackMotor = None
color = None
rightFrontMotor = None
leftColor = None
leftGreen = None
rightBackMotor = None
arm = None
arm3 = None
rightColor = None
frontColor = None
frontDist = None
rightGreen = None
arm4 = None
leftDist = None
temp = None

# Describe this function...
def grabBall():
  global LM, RM, finger1, leftFrontMotor, confirmGreen, colorVal, finger2, arm2, leftBackMotor, color, rightFrontMotor, leftColor, leftGreen, rightBackMotor, arm, arm3, rightColor, frontColor, frontDist, rightGreen, arm4, leftDist, temp
  finger1.setVelocity((2 / 100.0) * finger1.getMaxVelocity())
  finger2.setVelocity((2 / 100.0) * finger2.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 160:
      break
  finger1.setVelocity((0 / 100.0) * finger1.getMaxVelocity())
  finger2.setVelocity((0 / 100.0) * finger2.getMaxVelocity())
  arm3.setVelocity((40 / 100.0) * arm3.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 500:
      break
  arm3.setVelocity((0 / 100.0) * arm3.getMaxVelocity())
  arm4.setVelocity((40 / 100.0) * arm4.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 900:
      break
  arm4.setVelocity((0 / 100.0) * arm4.getMaxVelocity())
  finger1.setVelocity(((-2) / 100.0) * finger1.getMaxVelocity())
  finger2.setVelocity(((-2) / 100.0) * finger2.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 160:
      break
  finger1.setVelocity((0 / 100.0) * finger1.getMaxVelocity())
  finger2.setVelocity((0 / 100.0) * finger2.getMaxVelocity())
  arm2.setVelocity(((-40) / 100.0) * arm2.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 485:
      break
  arm2.setVelocity((0 / 100.0) * arm2.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 10000:
      break
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 800:
      break

# Describe this function...
def getVictim():
  global LM, RM, finger1, leftFrontMotor, confirmGreen, colorVal, finger2, arm2, leftBackMotor, color, rightFrontMotor, leftColor, leftGreen, rightBackMotor, arm, arm3, rightColor, frontColor, frontDist, rightGreen, arm4, leftDist, temp
  setMotor(0, 0)
  arm2.setVelocity((70 / 100.0) * arm2.getMaxVelocity())
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 1800:
      break
  arm2.setVelocity((0 / 100.0) * arm2.getMaxVelocity())
  setMotor(20, 20)
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 2000:
      break
  print('a')
  while myRobot.step(timeStep) != -1 and (leftDist.getValue()) > 90:
    if gyroEnable:
      updateGyro()
    print(leftDist.getValue())
  print('b')
  setMotor(-40, 40)
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 460:
      break
  setMotor(20, 20)
  while myRobot.step(timeStep) != -1 and (frontDist.getValue()) > 30:
    if gyroEnable:
      updateGyro()
    print(frontDist.getValue())
  setMotor(-33, -33)
  initTime = myRobot.getTime()
  while myRobot.step(timeStep) != -1:
    if (myRobot.getTime() - initTime) * 1000.0 > 200:
      break
  print('c')
  setMotor(0, 0)
  grabBall()

# Describe this function...
def setMotor(LM, RM):
  global finger1, leftFrontMotor, confirmGreen, colorVal, finger2, arm2, leftBackMotor, color, rightFrontMotor, leftColor, leftGreen, rightBackMotor, arm, arm3, rightColor, frontColor, frontDist, rightGreen, arm4, leftDist, temp
  leftFrontMotor.setVelocity((LM / 100.0) * leftFrontMotor.getMaxVelocity())
  rightFrontMotor.setVelocity((RM / 100.0) * rightFrontMotor.getMaxVelocity())

# black: 0, white: 1
# left (1s place): 0, 1
# front (2s place): 0, 2
# right (4s place): 0, 4
# left green: 8
# right green: 9
def setColor():
  global LM, RM, finger1, leftFrontMotor, confirmGreen, colorVal, finger2, arm2, leftBackMotor, color, rightFrontMotor, leftColor, leftGreen, rightBackMotor, arm, arm3, rightColor, frontColor, frontDist, rightGreen, arm4, leftDist, temp
  colorVal = 0
  color = getLSColor(leftColor.getImageArray())
  leftGreen = color[0 + 1] / (color[-1 + 1] + 0.1)
  if color[0 + 1] > 145:
    colorVal = (colorVal if isinstance(colorVal, Number) else 0) + 1
  color = getLSColor(frontColor.getImageArray())
  if color[0 + 1] > 145:
    colorVal = (colorVal if isinstance(colorVal, Number) else 0) + 2
  color = getLSColor(rightColor.getImageArray())
  rightGreen = color[0 + 1] / (color[-1 + 1] + 0.1)
  if color[0 + 1] > 145:
    colorVal = (colorVal if isinstance(colorVal, Number) else 0) + 4
  if leftGreen > 1.2:
    colorVal = 8
  if rightGreen > 1.2:
    if colorVal != 8:
      colorVal = 9
    else:
      colorVal = 10


from controller import Robot
from controller import GPS
from controller import LightSensor
from controller import Motor
from controller import PositionSensor
from controller import Gyro
import math

kernel_size = 10
internal_angle = 0.0

def getLSColor(camImg):
 global kernel_size
 rgb = [0] * 3
 for x in range(0,kernel_size):
  for y in range(0,kernel_size):
   for i in range(0,3):
    rgb[i] += camImg[x][y][i]
 for i in range(0,3):
  rgb[i] = int(rgb[i]/(kernel_size*kernel_size))
 return rgb
def getLSGray(camImg):
 global kernel_size
 gray = 0
 for x in range(0,kernel_size):
  for y in range(0,kernel_size):
   for i in range(0,3):
    gray += camImg[x][y][i]
 gray = int(gray/(3*kernel_size*kernel_size))
 return gray

#updates angle variable according to angular velocity from gyro
#angleCurrent = anglePast + integral of angular velocity over one timeStep since last updated angle
#should be called every time main loop repeats
def updateGyro():
 global internal_angle
 internal_angle += (timeStep / 1000.0) * (gyro.getValues())[1]

#returns current angle of robot relative to starting angle
#angle does not drop to 0 after exceeding 360
#angle % 360 will yield relative angle with maximum 360
def getAngle():
 global internal_angle
 return internal_angle * 180.0 / 3.1415

def getEncoders(posSensor):
  global encCount
  encCount[posSensor] = posSensor.getValue() / 3.1415 * 180.0
  if encCount[posSensor] != encCount[posSensor]:
    encCount[posSensor] = 0
  return False

def getObjAng(coord):
    ang = math.degrees(math.atan(coord[0]/math.fabs(coord[2])))
    return ang

myRobot = Robot()
timeStep = 64
encObj = {}
lastTimeReset = 0
gyroEnable = False
encCount = {}
lastEncReset = {}
myRobot.step(timeStep)

confirmGreen = 0
leftFrontMotor = myRobot.getDevice("left wheel")
encObj[leftFrontMotor] = leftFrontMotor.getPositionSensor()
leftFrontMotor.setPosition(float("inf"))
leftFrontMotor.setVelocity(0)
encObj[leftFrontMotor].enable(timeStep)
encCount[leftFrontMotor] = 0
lastEncReset[encObj[leftFrontMotor]] = 0

rightFrontMotor = myRobot.getDevice("right wheel")
encObj[rightFrontMotor] = rightFrontMotor.getPositionSensor()
rightFrontMotor.setPosition(float("inf"))
rightFrontMotor.setVelocity(0)
encObj[rightFrontMotor].enable(timeStep)
encCount[rightFrontMotor] = 0
lastEncReset[encObj[rightFrontMotor]] = 0

arm = myRobot.getDevice("scoop")
encObj[arm] = arm.getPositionSensor()
arm.setPosition(float("inf"))
arm.setVelocity(0)
encObj[arm].enable(timeStep)
encCount[arm] = 0
lastEncReset[encObj[arm]] = 0

leftColor = myRobot.getDevice('leftColor')
leftColor.enable(timeStep)
rightColor = myRobot.getDevice('rightColor')
rightColor.enable(timeStep)
frontColor = myRobot.getDevice('frontColor')
frontColor.enable(timeStep)
frontDist = myRobot.getDevice('frontDist')
frontDist.enable(timeStep)
leftDist = myRobot.getDevice('leftDist')
leftDist.enable(timeStep)
arm.setVelocity((70 / 100.0) * arm.getMaxVelocity())
initTime = myRobot.getTime()
while myRobot.step(timeStep) != -1:
  if (myRobot.getTime() - initTime) * 1000.0 > 3000:
    break
arm.setVelocity((0 / 100.0) * arm.getMaxVelocity())
while myRobot.step(timeStep) != -1 and True:
  if gyroEnable:
    updateGyro()
  setColor()
  temp = getLSColor(frontColor.getImageArray())
  if temp[-1 + 1] / (temp[0 + 1] + 0.01) > 1.2:
    getVictim()
  if colorVal == 0 or colorVal == 1 or colorVal == 4:
    setMotor(20, 20)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 80:
        break
    temp = getLSColor(frontColor.getImageArray())
    if temp[0 + 1] < 130:
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 420:
          break
  elif colorVal == 3:
    setMotor(40, -10)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 70:
        break
  elif colorVal == 6:
    setMotor(-10, 40)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 70:
        break
  elif colorVal == 5 or colorVal == 7:
    setMotor(20, 20)
  elif colorVal == 8:
    if confirmGreen == 8:
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 500:
          break
      setMotor(-20, 20)
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 600:
          break
      confirmGreen = 0
    else:
      confirmGreen = 8
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 50:
          break
  elif colorVal == 9:
    if confirmGreen == 9:
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 500:
          break
      setMotor(20, -20)
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 600:
          break
      confirmGreen = 0
    else:
      confirmGreen = 9
      initTime = myRobot.getTime()
      while myRobot.step(timeStep) != -1:
        if (myRobot.getTime() - initTime) * 1000.0 > 50:
          break
  elif colorVal == 10:
    setMotor(-50, 50)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 800:
        break
    confirmGreen = 0
  if (frontDist.getValue()) < 10:
    setMotor(-50, 50)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 400:
        break
    setMotor(50, 30)
    temp = getLSColor(frontColor.getImageArray())
    while myRobot.step(timeStep) != -1 and temp[0 + 1] > 140:
      if gyroEnable:
        updateGyro()
      temp = getLSColor(frontColor.getImageArray())
    setMotor(20, 20)
    initTime = myRobot.getTime()
    while myRobot.step(timeStep) != -1:
      if (myRobot.getTime() - initTime) * 1000.0 > 400:
        break
while myRobot.step(timeStep) != -1 and True:
  if gyroEnable:
    updateGyro()
  print(str('L: ') + str((getLSColor(leftColor.getImageArray()))))
  print(str('R: ') + str((getLSColor(rightColor.getImageArray()))))
  print(str('F: ') + str((getLSColor(frontColor.getImageArray()))))
