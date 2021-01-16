var scoreDisplay = document.getElementById("score");

var score = 0;

var Obstacle = {scoreValue: 15, n: 3};
var Intersection = {scoreValue: 15, n: 3};

const incrementScore = function(val) {

    score += val;
    
    var text = "Total Score: " + score;
    scoreDisplay.innerText = text;
}

const clickScoringBox = function() {

    //strip off numbers from the name of the scoring element
    var scoringElement = event.srcElement.id.replace(/[0-9]/g, '');
    var toggledOn = event.srcElement.checked;
    var scoreValue = 0;

    switch(scoringElement) {

        case "obstacle":
            scoreValue = Obstacle.scoreValue;
            break;
        case "intersection":
            scoreValue = Intersection.scoreValue;
            break;
        default:
            console.log("Error: unrecognized scoring element" + scoringElement);
            break;
    }

    //Negate the score if the checkbox was toggled off
    if(toggledOn == false) scoreValue = -scoreValue;
    incrementScore(scoreValue);
}

window.onload = function() {
    window.robotWindow = webots.window("scoring_window");
    window.robotWindow.receive = null;
}
