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
    var scoringElement = event.srcElement.parentElement.id.replace(/[0-9]/g, '');
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

const addScoringElement = function(type) {

    console.log(event.srcElement.parentElement);
    var n = 0;
    switch(type) {

        case "obstacle":
            n = ++Obstacle.n;
            break;
        case "intersection":
            n = ++Intersection.n; 
            break;
    }

    const div = document.createElement('div');
    div.id = type + n;
    
    const upperCasedLabel = type.charAt(0).toUpperCase() + type.slice(1) + " " + n;
    div.innerHTML = `
            <label for="${div.id}">${upperCasedLabel}: </label>
            <input type="checkbox" name="${div.id}" onclick="clickScoringBox()">
            <br>
            `;

    event.srcElement.parentElement.appendChild(div);
}

const removeScoringElement = function(type) {

    var n = 0;
    var scoreValue;
    switch(type) {

        case "obstacle":
            n = Obstacle.n--;
            scoreValue = Obstacle.scoreValue;
            break;
        case "intersection":
            n = Intersection.n--;
            scoreValue = Intersection.scoreValue;
            break;
    }

    var div = document.getElementById(type+n);
    var checkbox = div.getElementsByTagName('input');

    if(checkbox[0].checked)
        incrementScore(-scoreValue);

    event.srcElement.parentElement.removeChild(div);

}

const messageSupervisor = function(msg) {
    window.robotWindow.send(msg);
}

window.onload = function() {
    window.robotWindow = webots.window("scoring_window");
    window.robotWindow.receive = null;
}
