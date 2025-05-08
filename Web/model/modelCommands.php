<?php
    Class ModelCommands
    {
        public string $ip;
        public array $infoDessin;
        
        public function __construct(string $_ip,  array $_infoDessin)
        {
            $this->ip = $_ip;
            $this->infoDessin = $_infoDessin;
        }
    }
?>
