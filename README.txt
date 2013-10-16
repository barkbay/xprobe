===========
COMPILATION
===========

Ce programme est �crit en C, pour d�marrer la compilation taper la commande 'make'
Un fois la compilation termin� le programme 'myprobe' est �crit dans le r�pertoire
courant.

=====
USAGE
=====

Le programme 'myprobe' ne n�cessite qu'un seul param�tre : la fr�quence d'enregistrement
souhait�e des param�tres syst�mes en secondes.

exemple :

./myprobe 5

La commande ci-dessus va enregistrer les param�tres syst�mes toutes les 5 secondes.

Pour demander une pr�cision � la seconde :

./myprobe 1

Pour lancer en t�che de fond utiliser la commande Unix 'nohup' comme ceci :

nohup ./myprobe 5 &> /dev/null

Pour l'arr�ter il faut alors utiliser la commande 'kill' en ayant au pr�alabale
identifi� le num�ro de processus Unix avec un 'ps -edf | grep myprobe'

=============================
QU'EST CE QUI EST SURVEILLE ?
=============================

Pour chacune des partitions sur lesquelles le programme est lanc� les param�tres suivants
sont enregistr�s :

 * Le nombre de CPU physiques consomm�s : cpu.physc
 * La proportion de l'entitlement CPU consomm� : cpu.entc
 * La quantit� de m�moire vive (RAM) consomm� : mem.used
 * La quantit� de m�moire vive (RAM) disponible : mem.free
 * La quantit� d'espace de pagination (swap) consomm� : mem.paging.used

=================================================
COMMENT CONNAITRE l'ID DE MA MACHINE (PHYSIQUE) ?
=================================================

Utiliser la commande 'uname -F' le code affich� est unique pour un ch�ssis donn�.

==================================================
COMMENT CONNAITRE L'ID DE LA PARTITION (LOGIQUE) ?
==================================================

Utiliser la commande 'hostname'

==============================================
COMMENT CONSULTER LES PARAMETRES ENREGISTRES ?
==============================================

Se rendre � l'adresse : http://aurora.cneio.cn.cnamts.fr/tsd

 * Modifier la p�riode (date de d�but et date de fin) si n�cessaire
 * Dans le champ 'metric' renseigner le param�tre souhait�

 ---------
 ATTENTION
 ---------

 * L'interface web n'est PAS compatible avec Internet Explorer 6

 * Par d�faut l'interface affiche la somme des r�sultats de toutes les sondes
 d�ploy�es, afin de s�lectionner pr�cisement la partition ajouter les 2 tags suivants :

   1. machine : id de la machine (retour de la commande Unix 'uname -F')
   2. host : id de la parition (retour de la comande hostname)

(c.f. les 2 sections pr�c�dentes pour les connaitre)
