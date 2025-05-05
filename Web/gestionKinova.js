robot = [];
zoneRobot;
robotCount = 1;
function init()
{
    zoneRobot= document.getElementById("zoneRobot");
}

function AjoutRobot()
{
    // Créer le conteneur principal pour le robot
    let robot = document.createElement('div');

    // Ajouter un titre "Hi" au conteneur du robot
    let title = document.createElement('h2');
    title.textContent = 'Robot #' + robotCount;
    robot.appendChild(title);

    let ipTitle = document.createElement('h4');
    ipTitle.textContent = 'IP: ';
    robot.appendChild(ipTitle);

    // Ajouter un champ d'entrée pour l'IP
    let ip = document.createElement("input");
    ip.name = "ip";
    ip.setAttribute('type', 'text');
    ipTitle.appendChild(ip);

    // let scaleTitle = document.createElement('h4');
    // scaleTitle.textContent = 'Scale: ';
    // robot.appendChild(scaleTitle);

    // Ajouter un champ d'entrée pour l'échelle
    // let scale = document.createElement("input");
    // scale.setAttribute('type', 'number');
    // scale.setAttribute('value', 'default');
    // scaleTitle.appendChild(scale);

    // Créer un conteneur pour les commandes
    let commands = document.createElement('div');
    commands.setAttribute('id', 'commands'); // Utiliser 'id' au lieu de 'name'
    let command = document.createElement('h3');
    command.textContent = "Commande (Le nombre de moteur et l'angle) : ";
    command.setAttribute('id', 'command');
    commands.appendChild(command);

    // Ajouter un bouton pour ajouter des lignes "commands"
    let addCommandButton = document.createElement("button");
    addCommandButton.textContent = "Ajouter une commande";
    addCommandButton.addEventListener("click", () => {
        // Créer une nouvelle ligne de commande
        let newCommand = document.createElement("div");
        newCommand.textContent = "Commande #" + (commands.childElementCount) + ": "; // Compter le nombre d'éléments enfants pour le numéro de commande
        newCommand.classList.add("command-line");

        let xInput = document.createElement("input");
        xInput.setAttribute("type", "number");
        xInput.setAttribute("placeholder", "Moteurs");
        newCommand.appendChild(xInput);

        let yInput = document.createElement("input");
        yInput.setAttribute("type", "number");
        yInput.setAttribute("placeholder", "Angle");
        newCommand.appendChild(yInput);

        // let iInput = document.createElement("input");
        // iInput.setAttribute("type", "number");
        // iInput.setAttribute("placeholder", "I");
        // newCommand.appendChild(iInput);

        let drawCheckbox = document.createElement("input");
        drawCheckbox.setAttribute("type", "checkbox");
        newCommand.appendChild(drawCheckbox);

        // Ajouter la nouvelle ligne de commande au conteneur "commands"
        commands.appendChild(newCommand);
    });



    // Ajouter un bouton "finis" pour chaque robot Ajouter une fonction pour que lorsqu'on appuie on dégrise les valeurs "vitesse" "pause" et "cancel" et on grise les valeurs 
    // "IP" "Scale" ainsi que les commandes
    let finishButton = document.createElement("button");
    finishButton.textContent = "Finis";
    let currentRobotCount = robotCount; // Capturer la valeur actuelle de robotCount
    finishButton.addEventListener("click", () => {
        alert("Configuration du Robot #" + currentRobotCount + " terminée !");
        GriserElement(); // Appeler la fonction GriserElement
    });

    // Ajouter le bouton "Ajouter une commande" au conteneur du robot
    robot.appendChild(addCommandButton); // <-- Ajouté au conteneur du robot
    // Ajouter le conteneur des commandes au robot
    robot.appendChild(commands);
    // Ajouter le bouton "finis" au conteneur du robot
    robot.appendChild(finishButton);


    // Ajouter le robot à la zone principale
    zoneRobot.appendChild(robot); // <-- Le conteneur du robot est ajouté à zoneRobot

    // +1 au nombre de robots
    robotCount++;
}

function RefreshRobot()
{
    
}

async function GriserElement()
{
    // Récupérer tous les éléments de la zoneRobot
    let elements = zoneRobot.children;

    // Parcourir chaque élément et le désactiver
    for (let i = 0; i < elements.length; i++) {
        let element = elements[i];
        let inputs = element.querySelectorAll('input, button');
        inputs.forEach(input => input.disabled = true); // Désactiver les champs d'entrée et les boutons
    }
    degriserStats(); // Appeler la fonction pour dégriser les statistiques
}

async function degriserStats() {
    // Vérifiez si "stats" existe déjà, sinon créez-le
    let stats = document.getElementById('stats');
    if (!stats) {
        stats = document.createElement('div');
        stats.setAttribute('id', 'stats'); // Ajoutez un ID pour le retrouver facilement
        stats.setAttribute('class', 'stats-container'); // Classe pour le style
        document.body.appendChild(stats); // Ajoutez-le au document (ou à un autre conteneur)
    }

    // Boutons pour "Vitesse", "Pause" et "Annuler"
    let speedBox = document.createElement('div');
    speedBox.setAttribute('class', 'stat-box');
    let speedLabel = document.createElement('input');
    speedLabel.setAttribute('type', 'number');
    speedLabel.setAttribute('placeholder', 'Vitesse');
    speedLabel.textContent = 'Vitesse: ';
    let speedValue = document.createElement('span');
    speedBox.appendChild(speedLabel);
    speedBox.appendChild(speedValue);
    stats.appendChild(speedBox);

    // Ajouter un listener pour détecter la touche "Enter"
    speedLabel.addEventListener('keydown', (event) => {
        if (event.key === 'Enter') {
            handleSpeedEnter(speedLabel.value); // Appeler une fonction avec la valeur saisie
        }
    });

    // Fonction appelée lorsqu'on appuie sur "Enter"
    function handleSpeedEnter(value) {
        alert('Vitesse définie à : ' + value); // Exemple d'action
    }

    let pauseCancelContainer = document.createElement('div');
    pauseCancelContainer.setAttribute('class', 'pause-cancel-container'); // Classe pour le style

    let pauseBox = document.createElement('div');
    pauseBox.setAttribute('class', 'stat-box');
    let pauseLabel = document.createElement('label');
    pauseLabel.textContent = 'Pause: ';
    let pauseCheckbox = document.createElement('input');
    pauseCheckbox.setAttribute('type', 'checkbox');
    pauseBox.appendChild(pauseLabel);
    pauseBox.appendChild(pauseCheckbox);

    let cancelBox = document.createElement('div');
    cancelBox.setAttribute('class', 'stat-box');
    let cancelLabel = document.createElement('label');
    cancelLabel.textContent = 'Annuler: ';
    let cancelCheckbox = document.createElement('input');
    cancelCheckbox.setAttribute('type', 'checkbox');
    cancelBox.appendChild(cancelLabel);
    cancelBox.appendChild(cancelCheckbox);

    // Ajouter les conteneurs "Pause" et "Annuler" côte à côte
    pauseCancelContainer.appendChild(pauseBox);
    pauseCancelContainer.appendChild(cancelBox);
    stats.appendChild(pauseCancelContainer);

    let title = document.createElement('h3');
    title.textContent = 'Statistiques';
    stats.appendChild(title);
}

async function callAPI(methode, params) 
{
    const url = 'http://localhost/API.php';

    const response = await fetch(url+params, {
        method: methode,
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(params)
    });

    if (!response.ok) {
        throw new Error('Erreur lors de l\'appel à l\'API : ' + response.statusText);
    }

    return response.json();
}

// async function ()
// {

// }

// POST: API.php/Robots/ip (avec body)
async function CreationRobot() 
{

}

init();