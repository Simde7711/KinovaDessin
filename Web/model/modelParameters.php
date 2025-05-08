<?php
    Class ModelParameters
    {
        public string $ip;
        public array $parameters;
        
        public function __construct(string $_ip,  array $_parameters)
        {
            $this->ip = $_ip;
            $this->parameters = $_parameters;
        }
    }
?>