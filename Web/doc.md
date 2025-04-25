# endpoints:

### GET
- API.php/Robots/informations

### POST
- API.php/Robots/ip

body: "{
            "ip": "10.10.10.10",
            "infoDessin": {
                "scale": -3,
                "commands": [
                    {
                        "x": 9,
                        "y": 14,
                        "i": 100,
                        "draw": false
                    },
                    {
                        "x": 18,
                        "y": 28,
                        "i": 2,
                        "draw": true
                    }
                ]
            }
}"

### PUT
- API.php/Robots/ip

body: "{
"vitesse":10,
"pause":false,
"cancel": true
}"