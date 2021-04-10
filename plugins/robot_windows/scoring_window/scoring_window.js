var scoreDisplay = document.getElementById("score");
var modal = document.getElementById("myModal");
var modalMessage = document.getElementById("modalMessage");

var score = 0;
var numLOP = 0;
var LOPind = 0;
var level = 0;
var kitLevel = 0;
var kitMultiply = 1;
var evacMultiply = 1;
var evacLack = 0;
var evacCheck = false;

var Obstacle = {scoreValue: 15, n: 2};
var Intersection = {scoreValue: 15, n: 3};
var Gap = {scoreValue: 10, n: 2};
var Speedbump = {scoreValue: 5, n: 2};
var Seesaw = {scoreValue: 10, n: 2};
var Checkpoint = {scoreValue: 1, n: 3};
var Victim = {scoreValue: [1.2, 1.4]};
var RescueKit = {scoreValue: [1.1, 1.3, 1.2, 1.6]}

var LOP = [];
var checkpointDist = [];

function hideModal() {

    modal.style.display="none";

    var modalConfirmButton = document.getElementById("modalConfirm");
    var modalConfirmButtonClone = modalConfirmButton.cloneNode(true);
    modalConfirmButton.parentNode.replaceChild(modalConfirmButtonClone, modalConfirmButton);
}

function incrementScore(val) {

    score += val;
    
    var text = "Total Score: " + score;
    scoreDisplay.innerText = text;
}

function clickScoringBox(event) {

    //strip off numbers from the name of the scoring element
    var scoringElement = event.srcElement.parentElement.id.replace(/[0-9]/g, '');
    var toggledOn = event.srcElement.checked;
    var scoreValue = 0;
    var cpNum;

    switch(scoringElement) {

        case "obstacle":
            scoreValue = Obstacle.scoreValue;
            break;
        case "intersection":
            scoreValue = Intersection.scoreValue;
            break;
        case "gap":
            scoreValue = Gap.scoreValue;
            break;
        case "checkpoint":
            cpNum = parseInt(event.srcElement.name.slice(-1)) - 1;
            if (LOP[cpNum] < 3)
                scoreValue = checkpointDist[cpNum] * ((2 - LOP[cpNum]) * 2 + 1)
            break;
        case "speed bump":
            scoreValue = Speedbump.scoreValue;
            break;
        case "ramp":
            scoreValue = Seesaw.scoreValue;
            break;
        case "kitLevel":
            kitLevel = document.getElementsByName('kitLevel')[0].value - 1;
            break;
        case "kit":
            kitMultiply = RescueKit.scoreValue[level * 2 + kitLevel];
            break;
        case "submitFinal":
            console.log('Final submit');
            break;
        case "level":
        case "livevictim":
        case "deadvictim":
        case "exitBonus":
            break;
        default:
            console.log("Error: unrecognized scoring element" + scoringElement);
            break;
    }

    //Negate the score if the checkbox was toggled off
    if(toggledOn == false) scoreValue = -scoreValue;
    incrementScore(scoreValue);
}

function addScoringElement(event) {
    var suffix = "";
    var n = 0;
    var type = event.srcElement.parentElement.id;
    switch(type) {

        case "obstacle":
            n = ++Obstacle.n;
            break;
        case "intersection":
            n = ++Intersection.n; 
            break;
        case "gap":
            n = ++Gap.n;
            break;
        case "checkpoint":
            suffix += " (0 Lack of Progress)";
            n = ++Checkpoint.n;
            break;
        case "speed bump":
            n = ++Speedbump.n;
            break;
        case "ramp":
            n = ++Seesaw.n;
    }

    const div = document.createElement('div');
    div.id = type + n;

    const upperCasedLabel = type.charAt(0).toUpperCase() + type.slice(1) + " " + n + suffix;
    div.innerHTML = `
            <label for="${div.id}">${upperCasedLabel}: </label>
            <input type="checkbox" name="${div.id}">
            <br>
            `;


    div.getElementsByTagName("input")[0].addEventListener('click', clickScoringBox);
    event.srcElement.parentElement.appendChild(div);
}

function removeScoringElement(event) {

    modalMessage.innerHTML = "Are you sure you want to remove <b>" + event.srcElement.parentElement.id + "</b> field?";
    modal.style.display="block"; 
    document.getElementById("modalConfirm").addEventListener('click', function() {

        removeScoringElementConfirmed(event);
        hideModal();
    });
}

function removeScoringElementConfirmed(event) {

    var elementType;
    var type = event.srcElement.parentElement.id;
    switch(type) {

        case "obstacle":
            elementType = Obstacle;
            break;
        case "intersection":
            elementType = Intersection;
            break;
        case "gap":
            elementType = Gap;
            break;
        case "checkpoint":
            elementType = Checkpoint;
            break;
        case "speed bump":
            elementType = Speedbump;
            break;
        case "ramp":
            elementType = Seesaw;
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

function addVictimScore() {
    //var finishLive = document.getElementsByName('finishlive')[0].value;
    var numLive = document.getElementsByName('livevictim')[0].value;
    var numDead = document.getElementsByName('deadvictim')[0].value;
    if (evacMultiply == 1)
        evacMultiply = 0;
    evacMultiply += Victim.scoreValue[level] * numLive;
    if (numLive == 2) {
        evacMultiply += Victim.scoreValue[level] * numDead;
    }
    evacMultiply *= kitMultiply;
}

function markLevel(event) {
    level = document.getElementsByName('level')[0].value - 1;
    messageSupervisor(level);
}

function finalScore(event) {
    var finalMsg = score.toString() + ',' + (60 - (5 * numLOP)).toString() + ',' + evacMultiply.toString() + ',';
    if (document.getElementsByName('exitBonus')[0].value)
        score += 60 - (5 * numLOP);
    score *= evacMultiply;
    finalMsg += score + ',';
    messageSupervisor(finalMsg);
    var cpElem = document.getElementById("score");
    var newLabel = "Total Score: " + score.toFixed(2);
    cpElem.innerHTML = `
        <label id = "${cpElem.id}">${newLabel}</label>
        <br>
        `;
}

function messageSupervisor(msg) {
    window.robotWindow.send(msg);
}

window.onload = function() {
    window.robotWindow = webots.window("scoring_window");
    //window.robotWindow.receive = null;

    document.getElementById("LOPButton").addEventListener('click', function() {
        if (evacCheck)
            evacLack++;
        numLOP++;
        messageSupervisor("L");
        var cpElem = document.getElementById("LOPCount");
        var newLabel = "Total Count: " + numLOP;
        cpElem.innerHTML = `
            <label id = "${cpElem.id}">${newLabel}</label>
            <br>
            `;
        if (LOPind < LOP.length) {
            LOP[LOPind]++;
            cpElem = document.getElementById("checkpoint" + (LOPind + 1).toString());
            newLabel = "Checkpoint " + (LOPind + 1).toString() + " (" + (LOP[LOPind]).toString() + " Lack of Progress)";
            cpElem.innerHTML = `
                <label for="${cpElem.id}">${newLabel}: </label>
                <input type="checkbox" name="${cpElem.id}">
                <br>
                `;
            cpElem.getElementsByTagName("input")[0].addEventListener('click', clickScoringBox);
        }
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

    var closeModalButtons = document.getElementsByClassName("closeButton");
    for(i = 0; i < closeModalButtons.length; i++) {
        closeModalButtons[i].addEventListener('click', hideModal);
    }

    var submitVic = document.getElementsByClassName("submitVic");
    for(i = 0; i < submitVic.length; i++) {
        submitVic[i].addEventListener('click', addVictimScore);
    }

    var submitLevel = document.getElementsByClassName("submitLevel");
    for(i = 0; i < submitLevel.length; i++) {
        submitLevel[i].addEventListener('click', markLevel);
    }

    var submitFinal = document.getElementsByClassName("submitFinal");
    for(i = 0; i < submitLevel.length; i++) {
        submitFinal[i].addEventListener('click', finalScore);
    }

    window.addEventListener('click', function(event) {

        if(event.target == modal) {

            hideModal();
        }
    });
    window.robotWindow.receive = function(msg) {
        if (msg.charAt(0) == 'D'){
            var tmp;
            msg = msg.substring(1);
            while ((tmp = msg.search(",")) > -1) {
                if (tmp == 0)
                    checkpointDist.push(1);
                else
                    checkpointDist.push(parseInt(msg.substring(0, tmp)));
                msg = msg.substring(tmp + 1);
                LOP.push(0);
            }
        }
        if (msg.charAt(0) == 'C') {
            LOPind++;
            var cpNum = parseInt(msg.substring(2));
            if (msg.charAt(1) == '*')
                evacCheck = true;
            else if (msg.charAt(1) == '-')
                evacCheck = false;
            else
                cpNum = parseInt(msg.substring(1));
            if (LOP[cpNum] < 3)
                incrementScore(checkpointDist[cpNum] * ((2 - LOP[cpNum]) * 2 + 1));
        }
        if (msg.charAt(0) == 'S') {
            switch (msg.charAt(1)) {
                case '!':
                    incrementScore(15);
                    break;
                case '@':
                    incrementScore(10);
                    break;
                case '#':
                    incrementScore(5);
                    break;
            }
        }
        if (msg.charAt(0) == 'F')
            finalScore();
        if (msg.charAt(0) == 'L') {
            if (evacCheck)
                evacLack++;
            numLOP++;
            var cpElem = document.getElementById("LOPCount");
            var newLabel = "Total Count: " + numLOP;
            cpElem.innerHTML = `
                <label id = "${cpElem.id}">${newLabel}</label>
                <br>
                `;
            if (LOPind < LOP.length) {
                LOP[LOPind]++;
                cpElem = document.getElementById("checkpoint" + (LOPind + 1).toString());
                newLabel = "Checkpoint " + (LOPind + 1).toString() + " (" + (LOP[LOPind]).toString() + " Lack of Progress)";
                cpElem.innerHTML = `
                    <label for="${cpElem.id}">${newLabel}: </label>
                    <input type="checkbox" name="${cpElem.id}">
                    <br>
                    `;
                cpElem.getElementsByTagName("input")[0].addEventListener('click', clickScoringBox);
            }
        }
    }
}

