<?php
    Class GestionCode
    {
        // Fonction Pour gerer les codes.
        public function GererCode($code, $messagePerso)
        {
            $reponseCode;
            switch ($code) 
            {
                case 200:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "OK",
                        "contenu" => $messagePerso
                    );
                    break;
                case 201:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "Created",
                        "contenu" => $messagePerso
                        );
                    break;
                case 204:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "No Content",
                        "contenu" => $messagePerso
                        );
                    break;
                case 400:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "Bad Request",
                        "contenu" => $messagePerso
                        );
                    break;
                case 401:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "Unauthorized",
                        "contenu" => $messagePerso
                        );
                    break;
                case 404:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "Not Found",
                        "contenu" => $messagePerso
                        );
                    break;
                case 406:
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "Not Acceptable",
                        "contenu" => $messagePerso
                        );
                    break;
                case 418;
                    $reponseCode = array(
                        "code" => $code, 
                        "titre" => "I'm a teapot",
                        "contenu" => $messagePerso
                    );
                    break;
                
                default:
                    break;
            }

            return $reponseCode;
        }
    }
?>