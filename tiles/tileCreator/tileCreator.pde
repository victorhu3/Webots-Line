int LINE_WIDTH = 60;//in pixels
int CMToPix = 30;
boolean save = true;

void lineByCM(int x1, int y1, int x2, int y2){//converts cm location to pixel location
  line(CMToPix*x1,CMToPix*y1,CMToPix*x2,CMToPix*y2);
}

void createGreen(float x, float y){//x and y locations in cm; denote top left corner of square
  fill(45,152,47);
  noStroke();
  rect(CMToPix*x,CMToPix*y,CMToPix*2.5,CMToPix*2.5);
}

void setup() {
  size(900, 900);
  //regular line cases: 0-10
  //intersection cases: 20-33
  //special tiles: 50
  for(int i = 0; i <= 10; i++){
    background(255);
    strokeCap(SQUARE);
    noFill();
    stroke(0);
    strokeWeight(4);
    lineByCM(0,0,0,30);
    lineByCM(0,30,30,30);
    lineByCM(30,30,30,0);
    lineByCM(30,0,0,0);
    strokeWeight(CMToPix*2);
    switch(i){
      case 0:
        break;
      case 1:
        line(0, height/2, height, height/2);
        break;
      case 2: 
        line(-20,height/2-20,height/2+20,height+20);
        line(height/2-20,-20,height+20,height/2+20);
        break;
      case 3: 
        line(0,height/2,CMToPix*5,height/2);
        line(CMToPix*10,height/2,CMToPix*20,height/2);
        line(CMToPix*25,height/2,CMToPix*30,height/2);
        break;
      case 4: 
        ellipse(height,height,height,height);
        break;
      case 5: 
        strokeCap(ROUND);
        line(0,height/2,CMToPix*5,height/2);
        line(CMToPix*25,height/2,CMToPix*30,height/2);
        arc(CMToPix*10,height/2,CMToPix*10,CMToPix*10,0,PI);
        arc(CMToPix*20,height/2,CMToPix*10,CMToPix*10,PI,PI*2);
        break;
      case 6: 
        strokeCap(ROUND);
        line(0,CMToPix*15,CMToPix*7,CMToPix*20);
        line(CMToPix*7,CMToPix*20,CMToPix*23,CMToPix*10);
        line(CMToPix*23,CMToPix*10,CMToPix*30,CMToPix*15);
        break;
      case 7: 
        strokeCap(PROJECT);
        lineByCM(15,30,15,22);
        lineByCM(15,22,8,22);
        lineByCM(8,22,8,7);
        lineByCM(8,7,22,7);
        lineByCM(22,7,22,15);
        lineByCM(22,15,30,15);
        break;  
      case 8: 
        strokeCap(ROUND);
        lineByCM(0,15,11,25);
        lineByCM(11,25,19,5);
        lineByCM(19,5,30,15);
        break;
      case 9: 
        strokeCap(PROJECT);
        lineByCM(0,15,15,15);
        lineByCM(15,15,15,30);
        break;
      case 10: 
        strokeCap(PROJECT);
        arc(CMToPix*30,CMToPix*30,CMToPix*30,CMToPix*30,PI,2*PI);
        break;
      case 20: 
        lineByCM(0,15,30,15);
        lineByCM(15,15,15,30);
        break;
      case 21: 
        lineByCM(0,15,30,15);
        lineByCM(15,15,15,30);
        createGreen(16,16);
        break;
      case 22: 
        lineByCM(0,15,30,15);
        lineByCM(15,15,15,30);
        createGreen(11.5,16);
        break;
      case 23: 
        lineByCM(0,15,30,15);
        lineByCM(15,15,15,30);
        createGreen(16,16);
        createGreen(11.5,16);
        break;
      case 24: 
        lineByCM(0,15,30,15);
        lineByCM(15,0,15,30);
        break;
      case 25: 
        lineByCM(0,15,30,15);
        lineByCM(15,0,15,30);
        createGreen(16,16);
        break;
      case 26: 
        lineByCM(0,15,30,15);
        lineByCM(15,0,15,30);
        createGreen(16,16);
        createGreen(11.5,11.5);
        break;
      case 27: 
        lineByCM(0,15,30,15);
        lineByCM(15,0,15,30);
        createGreen(16,16);
        createGreen(11.5,16);
        break;
      case 28: 
        lineByCM(0,15,30,15);
        lineByCM(15,0,15,30);
        createGreen(16,16);
        createGreen(11.5,16);
        createGreen(16,11.5);
        break;
      case 29: 
        rect(CMToPix*6, CMToPix*6, CMToPix*18, CMToPix*18,CMToPix*5);
        lineByCM(0,15,6,15);
        lineByCM(24,15,30,15);
        break;
      case 30: 
        rect(CMToPix*6, CMToPix*6, CMToPix*18, CMToPix*18,CMToPix*5);
        lineByCM(0,15,6,15);
        lineByCM(24,15,30,15);
        createGreen(2.5,16);
        createGreen(25,16);
        break;
      case 31: 
        rect(CMToPix*6, CMToPix*6, CMToPix*18, CMToPix*18,CMToPix*5);
        lineByCM(0,15,6,15);
        lineByCM(24,15,30,15);
        strokeWeight(LINE_WIDTH+2);
        stroke(255);
        lineByCM(12,24,18,24);
        createGreen(2.5,16);
        createGreen(25,16);
        break;
      case 32: 
        rect(CMToPix*6, CMToPix*6, CMToPix*18, CMToPix*18,CMToPix*5);
        lineByCM(15,24,15,30);
        lineByCM(24,15,30,15);
        createGreen(16,25);
        createGreen(25,16);
        break;
      case 33: 
        rect(CMToPix*6, CMToPix*6, CMToPix*18, CMToPix*18,CMToPix*5);
        lineByCM(15,24,15,30);
        lineByCM(24,15,30,15);
        createGreen(11.5,25);
        createGreen(25,11.5);
        break;
      case 50:
        lineByCM(15,0,15,30);
        stroke(168,37,37);
        strokeWeight(CMToPix*5);
        lineByCM(0,0,30,0);
        break;
      default:
        continue;
    } 
    if(save)
      save("tile" + i + ".png");
  }
}

void draw() {
}
