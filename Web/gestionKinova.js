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
    ipTitle.textContent = 'ip: ';
    robot.appendChild(ipTitle);

    // Ajouter un champ d'entrée pour l'IP
    let ip = document.createElement("input");
    ip.name = "ip";
    ip.setAttribute('type', 'text');
    ipTitle.appendChild(ip);

    let scaleTitle = document.createElement('h4');
    scaleTitle.textContent = 'scale: ';
    robot.appendChild(scaleTitle);

    // Ajouter un champ d'entrée pour l'échelle
    let scale = document.createElement("input");
    scale.setAttribute('type', 'number');
    scale.setAttribute('value', 'default');
    scaleTitle.appendChild(scale);

    // Créer un conteneur pour les commandes
    let commands = document.createElement('div');
    commands.setAttribute('id', 'commands'); // Utiliser 'id' au lieu de 'name'
    let command = document.createElement('h4');
    command.textContent = 'Commande : ';
    command.setAttribute('id', 'command');
    commands.appendChild(command);

    // Ajouter un bouton pour ajouter des lignes "commands"
    let addCommandButton = document.createElement("button");
    addCommandButton.textContent = "Ajouter une commande";
    addCommandButton.addEventListener("click", () => {
        // Créer une nouvelle ligne de commande
        let newCommand = document.createElement("div");
        newCommand.textContent = "Commande #" + (commands.childElementCount) + " "; // Compter le nombre d'éléments enfants pour le numéro de commande
        newCommand.classList.add("command-line");

        let xInput = document.createElement("input");
        xInput.setAttribute("type", "number");
        xInput.setAttribute("placeholder", "X");
        newCommand.appendChild(xInput);

        let yInput = document.createElement("input");
        yInput.setAttribute("type", "number");
        yInput.setAttribute("placeholder", "Y");
        newCommand.appendChild(yInput);

        let iInput = document.createElement("input");
        iInput.setAttribute("type", "number");
        iInput.setAttribute("placeholder", "I");
        newCommand.appendChild(iInput);

        let drawCheckbox = document.createElement("input");
        drawCheckbox.setAttribute("type", "checkbox");
        newCommand.appendChild(drawCheckbox);

        // Ajouter la nouvelle ligne de commande au conteneur "commands"
        commands.appendChild(newCommand);
    });



    // Ajouter un bouton "finis" pour chaque robot
    let finishButton = document.createElement("button");
    finishButton.textContent = "Finis";
    let currentRobotCount = robotCount; // Capturer la valeur actuelle de robotCount
    finishButton.addEventListener("click", () => {
        alert("Configuration du Robot #" + currentRobotCount + " terminée !");
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