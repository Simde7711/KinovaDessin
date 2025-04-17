<?php
    Class ModelCommands
    {
        public string $ip;
        public array $info;
        
        public function __construct(string $_ip,  array $_info)
        {
            $this->ip = $_ip;
            $this->info = $_info;
        }
    }
?>
