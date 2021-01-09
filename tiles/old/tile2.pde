int LINE_WIDTH = 60;
int CMToPix = 30;

void lineByCM(int x1, int y1, int x2, int y2){//converts cm location to pixel location
  line(CMToPix*x1,CMToPix*y1,CMToPix*x2,CMToPix*y2);
}

void setup() {
  size(900, 900);
  background(255);
  strokeCap(SQUARE);
  int i = 7;
  switch(i){
    case 1:
      fill(0);
      noStroke();
      rect(0, height/2, height, LINE_WIDTH);
      break;
    case 2: 
      stroke(0);
      strokeWeight(LINE_WIDTH);
      line(-20,height/2-20,height/2+20,height+20);
      line(height/2-20,-20,height+20,height/2+20);
      break;
    case 3: 
      stroke(0);
      strokeWeight(LINE_WIDTH);
      line(0,height/2,CMToPix*5,height/2);
      line(CMToPix*10,height/2,CMToPix*20,height/2);
      line(CMToPix*25,height/2,CMToPix*30,height/2);
      break;
    case 4: 
      noFill();
      stroke(0);
      strokeWeight(LINE_WIDTH);
      ellipse(height,height,height,height);
      break;
    case 5: 
      noFill();
      stroke(0);
      strokeWeight(LINE_WIDTH);
      strokeCap(ROUND);
      line(0,height/2,CMToPix*5,height/2);
      line(CMToPix*25,height/2,CMToPix*30,height/2);
      arc(CMToPix*10,height/2,CMToPix*10,CMToPix*10,0,PI);
      arc(CMToPix*20,height/2,CMToPix*10,CMToPix*10,PI,PI*2);
      break;
    case 6: 
      noFill();
      stroke(0);
      strokeWeight(LINE_WIDTH);
      strokeCap(ROUND);
      line(0,CMToPix*15,CMToPix*7,CMToPix*20);
      line(CMToPix*7,CMToPix*20,CMToPix*23,CMToPix*10);
      line(CMToPix*23,CMToPix*10,CMToPix*30,CMToPix*15);
      break;
    case 7: 
      noFill();
      stroke(0);
      strokeWeight(LINE_WIDTH);
      strokeCap(PROJECT);
      lineByCM(15,30,15,22);
      lineByCM(15,22,8,22);
      lineByCM(8,22,8,7);
      lineByCM(8,7,22,7);
      lineByCM(22,7,22,15);
      lineByCM(22,15,30,15);
      break;  
  } 
  save("tile" + i + ".png");
}

void draw() {
}
