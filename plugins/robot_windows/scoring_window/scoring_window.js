var scoreDisplay = document.getElementById("score");
var modal = document.getElementById("myModal");
var modalMessage = document.getElementById("modalMessage");

var score = 0;

var Obstacle = {scoreValue: 15, n: 3};
var Intersection = {scoreValue: 15, n: 3};

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

function addScoringElement(event) {

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

function messageSupervisor(msg) {
    window.robotWindow.send(msg);
}

window.onload = function() {
    window.robotWindow = webots.window("scoring_window");
    window.robotWindow.receive = null;

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

    var closeModalButtons = document.getElementsByClassName("closeButton");
    for(i = 0; i < closeModalButtons.length; i++) {
        closeModalButtons[i].addEventListener('click', hideModal);
    }
    window.addEventListener('click', function(event) {

        if(event.target == modal) {

            hideModal();
        }
    });
}

