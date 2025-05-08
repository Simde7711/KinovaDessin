// On définit la variable zoneRobot pour qu'elle soit égale à l'élément HTML avec l'id "zoneRobot"
// On définit la variable robot pour qu'elle soit égale à un tableau vide
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

    // Créer un conteneur pour les commandes
    let commands = document.createElement('div');
    commands.setAttribute('id', 'commands'); // Utiliser 'id' au lieu de 'name'
    let command = document.createElement('h3');
    command.textContent = "Commande (Le nombre de moteur et l'angle) : ";
    command.setAttribute('id', 'command');
    commands.appendChild(command);
    
    let commandsDiv = document.createElement('div');
    commandsDiv.setAttribute('id', 'commandsDiv');
    commands.appendChild(commandsDiv);

    // Ajouter 6 lignes de commandes par défaut
    for (let i = 0; i < 6; i++) {
        let newCommand = document.createElement("div");

        newCommand.textContent = "Moteur #" + (commands.childElementCount) + ": "; // Utiliser childElementCount pour le numéro du moteur
        newCommand.classList.add("command-line");
        
        // Créer un champ d'entrée pour l'angle
        let angleInput = document.createElement("input");
        angleInput.setAttribute("type", "number");
        angleInput.setAttribute("placeholder", "Angle");
        newCommand.appendChild(angleInput);

        commandsDiv.appendChild(newCommand);
    }



    // Ajouter un bouton "finis" pour chaque robot Ajouter une fonction pour que lorsqu'on appuie on dégrise les valeurs "vitesse" "pause" et "cancel" et on grise les valeurs 
    // "IP" "Scale" ainsi que les commandes
    let finishButton = document.createElement("button");
    finishButton.textContent = "Finis";
    let currentRobotCount = robotCount; // Capturer la valeur actuelle de robotCount
    finishButton.addEventListener("click", () => {
        alert("Configuration du Robot #" + currentRobotCount + " terminée !");
        GriserElement(); // Appeler la fonction GriserElement

        zoneCommand = document.getElementById("commandsDiv");
        let array = [];
        for (const child of zoneCommand.children) 
        {
            let angleInput = child.querySelector('input'); 
            if (angleInput) 
            {
                array.push(angleInput.value); 
            }
        }

        CreationRobot(array);
    });
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
    // Créer un conteneur pour "Pause" et "Annuler"
    let pauseCancelContainer = document.createElement('div');
    pauseCancelContainer.setAttribute('class', 'pause-cancel-container'); // Classe pour le style
    // Créer les conteneurs "Pause" et "Annuler" 
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



// POST: API.php/Robots/ip (avec body)
async function CreationRobot(array) 
{
    let jsonObject = {
        "commands": array.map(item => ({ item: item }))
    };

    callAPI("POST", jsonObject);
}

init();