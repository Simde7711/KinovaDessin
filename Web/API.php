<?php
    // Require_Once pour les fichers nécessaires;
    require_once(dirname(__FILE__).'/controller/controllerGestionCode.php');
    require_once(dirname(__FILE__).'/controller/controller.LMG.php');

    // Header permettant le pretty print du json
    header('Content-Type: application/json; charset=utf-8');

    // Methode.
    $methode = $_SERVER['REQUEST_METHOD'];

    // Recuperer l'URL entiere
    $url = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);

    // Separer l'URL en morceaux
    $uris = explode('/', $url);

    // Eliminer les uri non pertinents
    for ($i=0; $i < count($uris); $i++) 
    { 
        // Si le uri correspond à la page, c'est qu'on est arriver au bout
        if ($uris[$i] == basename(__FILE__))
        {
            array_splice($uris, 0, $i + 1);
            break;
        }
    }

    // Variable des classes.
    $controllerLMG = new ControllerLMG();
    $controllerGestionCode = new GestionCode();

    // Traiter la demande.
    $reponse;

    // Vérifier les endpoints pour read dans l'inventaire et l'achats d'articles.
    switch ($uris[0]) 
    {
        case "Robot": 
            if ($methode == "GET" && count($uris) == 1)
            {
                // $reponse =
            }

            if ($methode == "POST" && count($uris) == 2 && (preg_match('/^(?:\d{1,3}\.){3}\d{1,3}$/',$uris[1])))
            {
                $reponse = $controllerLMG->CreationRobot($uris[1]);
            }
            
            break;

        default:
        // Si aucun des endpoints correspond.
        $reponse = $controllerGestionCode->GererCode(404, "Vous n'avez pas les bons endpoints.");
        break;
    }

    echo json_encode($reponse,JSON_UNESCAPED_UNICODE);
?>