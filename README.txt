===========
COMPILATION
===========

Ce programme est écrit en C, pour démarrer la compilation taper la commande 'make'
Un fois la compilation terminé le programme 'myprobe' est écrit dans le répertoire
courant.

=====
USAGE
=====

Le programme 'myprobe' ne nécessite qu'un seul paramètre : la fréquence d'enregistrement
souhaitée des paramètres systèmes en secondes.

exemple :

./myprobe 5

La commande ci-dessus va enregistrer les paramètres systèmes toutes les 5 secondes.

Pour demander une précision à la seconde :

./myprobe 1

Pour lancer en tâche de fond utiliser la commande Unix 'nohup' comme ceci :

nohup ./myprobe 5 &> /dev/null

Pour l'arrêter il faut alors utiliser la commande 'kill' en ayant au préalabale
identifié le numéro de processus Unix avec un 'ps -edf | grep myprobe'

=============================
QU'EST CE QUI EST SURVEILLE ?
=============================

Pour chacune des partitions sur lesquelles le programme est lancé les paramètres suivants
sont enregistrés :

 * Le nombre de CPU physiques consommés : cpu.physc
 * La proportion de l'entitlement CPU consommé : cpu.entc
 * La quantité de mémoire vive (RAM) consommé : mem.used
 * La quantité de mémoire vive (RAM) disponible : mem.free
 * La quantité d'espace de pagination (swap) consommé : mem.paging.used

=================================================
COMMENT CONNAITRE l'ID DE MA MACHINE (PHYSIQUE) ?
=================================================

Utiliser la commande 'uname -F' le code affiché est unique pour un châssis donné.

==================================================
COMMENT CONNAITRE L'ID DE LA PARTITION (LOGIQUE) ?
==================================================

Utiliser la commande 'hostname'

==============================================
COMMENT CONSULTER LES PARAMETRES ENREGISTRES ?
==============================================

Se rendre à l'adresse : http://aurora.cneio.cn.cnamts.fr/tsd

 * Modifier la période (date de début et date de fin) si nécessaire
 * Dans le champ 'metric' renseigner le paramètre souhaité

 ---------
 ATTENTION
 ---------

 * L'interface web n'est PAS compatible avec Internet Explorer 6

 * Par défaut l'interface affiche la somme des résultats de toutes les sondes
 déployées, afin de sélectionner précisement la partition ajouter les 2 tags suivants :

   1. machine : id de la machine (retour de la commande Unix 'uname -F')
   2. host : id de la parition (retour de la comande hostname)

(c.f. les 2 sections précédentes pour les connaitre)
