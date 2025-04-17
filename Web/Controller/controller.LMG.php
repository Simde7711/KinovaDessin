<?php
    // Dépendances
    require_once(dirname(__FILE__).'/../model/modelCommands.php');

    Class ControllerLMG
    {
        private $gestionCode;
        private $pathDepot;

        function __construct()
        {   
            $this->gestionCode = new GestionCode();
            $this->pathDepot = dirname(__FILE__)."/../zoneDepot/";
        }

        private function Modeliser(string $_ipRobot, array $_params)
        {
            return new ModelCommands(
                $_ipRobot,
                $_params
            );
        }

        public function LireJson()
        {

        }

        public function EcrireJson(String $_ipRobot)
        {
            // Récupère les informations du json qui est passé par le POST.
            $paramsJson = file_get_contents('php://input');

            // Décode les informations pour les utiliser.
            $params = json_decode($paramsJson, true);

            $jsonFile = file_get_contents($this->pathDepot.'commands.json');
            $jsonFileContenu = json_decode($jsonFile, true);
            
            $array = array(
                "robots" => array()
            );

            if ($jsonFileContenu != null)
            {
                for ($i=0; $i < array.count; $i++) { 
                   array_push($array["robots"], $jsonFileContenu["robots"][i]);
                }
            }

            bool $estDansJson = false;
            foreach($array["robots"] as $robot)
            {
                if ($robot["ip"] == $_ipRobot)
                {
                    $estDansJson = true;
                }
            }

            if (!$estDansJson)
            { 
                array_push($array["robots"], $this->Modeliser($_ipRobot, $params));
            }

            $jsonReady = json_encode($array);

            file_put_contents($this->pathDepot.'commands.json', $jsonReady);
            
            return $this->gestionCode->GererCode(201, "tip chiasse.");
        }    
    }
?>