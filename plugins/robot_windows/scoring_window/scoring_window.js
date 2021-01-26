var scoreDisplay = document.getElementById("score");
var score = 0;

var Obstacle = {scoreValue: 15, n: 3};
var Intersection = {scoreValue: 15, n: 3};

const incrementScore = function(val) {

    score += val;
    
    var text = "Total Score: " + score;
    scoreDisplay.innerText = text;
}

const clickScoringBox = function(event) {

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

const addScoringElement = function(event) {

    var n = 0;
    var type = event.srcElement.parentElement.id;
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
            <input type="checkbox" name="${div.id}">
            <br>
            `;


    div.getElementsByTagName("input")[0].addEventListener('click', clickScoringBox);
    event.srcElement.parentElement.appendChild(div);
}

const removeScoringElement = function(event) {

    var elementType;
    var type = event.srcElement.parentElement.id;
    switch(type) {

        case "obstacle":
            elementType = Obstacle;
            break;
        case "intersection":
            elementType = Intersection;
            break;
    }

    if(elementType.n == 0) 
        return;

    var div = document.getElementById(type+elementType.n);
    var checkbox = div.getElementsByTagName('input');

    if(checkbox[0].checked)
        incrementScore(-elementType.scoreValue);

    event.srcElement.parentElement.removeChild(div);
    elementType.n--;
}

window.robotWindow = webots.window("scoring_window");
window.robotWindow.receive = null;

const messageSupervisor = function(msg) {
    window.robotWindow.send(msg);
}

document.getElementById("LOPButton").addEventListener('click', function() {
    messageSupervisor("L");
});

var inputs = document.getElementsByTagName("input");
for(i = 0; i < inputs.length; i++) {
    inputs[i].addEventListener('click', clickScoringBox);
}

var addButtons = document.getElementsByClassName("add");
for(i = 0; i < addButtons.length; i++) {
    addButtons[i].addEventListener('click', addScoringElement);
}

var removeButtons = document.getElementsByClassName("remove");
for(i = 0; i < removeButtons.length; i++) {
    removeButtons[i].addEventListener('click', removeScoringElement);
}