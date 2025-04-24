robot = [];
zoneRobot;

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
    title.textContent = 'robot';
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
    command.textContent = 'commande: ';
    command.setAttribute('id', 'command');
    commands.appendChild(command);

    // Ajouter les champs d'entrée pour les commandes
    let x = document.createElement("input");
    x.setAttribute('type', 'number');
    x.setAttribute('placeholder', 'X');
    command.appendChild(x);

    let y = document.createElement("input");
    y.setAttribute('type', 'number');
    y.setAttribute('placeholder', 'Y');
    command.appendChild(y);

    let i = document.createElement("input");
    i.setAttribute('type', 'number');
    i.setAttribute('placeholder', 'I');
    command.appendChild(i);

    let draw = document.createElement("input");
    draw.setAttribute('type', 'checkbox'); // Utiliser 'checkbox' pour un booléen
    command.appendChild(draw);

    // Ajouter le conteneur des commandes au robot
    robot.appendChild(commands);

    // Ajouter le robot à la zone principale
    zoneRobot.appendChild(robot);
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