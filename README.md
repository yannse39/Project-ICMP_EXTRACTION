Exfiltration de fichiers via le protocole ICMP

Un projet permettant d'exfiltrer des fichiers via le protocole ICMP sur des sockets RAW

Installation

Dans le répertoire racine, exécutez la commande make

Utilisation

`./icmp_server`

Exécutez le binaire en tant que server_master. Ceci est la valeur par défaut.

Options du serveur :
- `-f, --file` : Spécifie le fichier de données à exfiltrer.

`./icmp_client`

Exécutez le binaire en tant que client.

Options du client :
- `-f, --file` : Spécifie le fichier de données à exfiltrer. Incompatible avec -d.
- `-d, --directory` : Spécifie un répertoire. Chaque fichier dans le répertoire spécifié doit être extrait (non récursif). Incompatible avec -f.

License

GNU Licence.

Project by 

https://github.com/matpone and https://github.com/yannse39 .

Note: Assurez-vous d'avoir les droits d'exécution sur les fichiers avant de les utiliser.
