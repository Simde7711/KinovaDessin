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

        public function LireInformation()
        {
            $jsonFile = file_get_contents($this->pathDepot.'robotsInformation.json');
            $jsonFileContenu = json_decode($jsonFile, true);

            $infoRobot = array();
            foreach ($jsonFileContenu["robots"] as $robot)
            {
                array_push($infoRobot, $robot);
            }

            if ($infoRobot != "")
            {
                return array(
                    "message" => $this->gestionCode->GererCode(200, "information du robot récupéré"),
                    "robots" => $infoRobot
                );
            }
            else
            {
                return array(
                    "message" => $this->gestionCode->GererCode(400, "aucune robot n'a cette ip")
                );
            }
        }

        public function CreationRobot(String $_ipRobot)
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
                for ($i=0; $i < count($jsonFileContenu["robots"]); $i++) { 
                   array_push($array["robots"], $jsonFileContenu["robots"][$i]);
                }
            }

            $estDansJson = false;
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
                $jsonReady = json_encode($array);

                file_put_contents($this->pathDepot.'commands.json', $jsonReady);
            
                return array("message" => $this->gestionCode->GererCode(201, "tip chiasse. Le robot a été starter"));
            }
            else
            {
                return array("message" => $this->gestionCode->GererCode(400, "Le robot est déjà en fonction"));
            }
        }    

        public function UpdateParametresRobot(String $_ipRobot)
        {
            // Récupère les informations du json qui est passé par le POST.
            $paramsJson = file_get_contents('php://input');

            // Décode les informations pour les utiliser.
            $params = json_decode($paramsJson, true);

            $jsonFile = file_get_contents($this->pathDepot.'parameters.json');
            $jsonFileContenu = json_decode($jsonFile, true);

            $array = array(
                "robots" => array()
            );
        }
    }
?>