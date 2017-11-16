# 42-electrons

Electronic project, group repertory

## Project specifications list

- [ ] A project name

- [ ] A project introduction

- [ ] A block diagram who's showing all the needed elements
    - [ ] At center the PIC32 chip
    - [ ] On borders the connectors
    - [ ] Between them, what is connected to what, how, and with what protocol

- [ ] A previsionnal manual who explain how to use
    - [ ] A final user must know how to use the final product

- [ ] Product specifications
    - [ ] Showing module (2 lines * 16 chars)
    - [ ] Memory
    - [ ] ...

- [ ] Functionnal calculations who explains the technical choices

- [ ] A list of protocols who will be used
    - [ ] How they will be used
    - [ ] How are they usefull
    - [ ] RFC
    - [ ] Implementation
    - [ ] Limitations

- [ ] A bill of materials
    - [ ] Components list
        - [ ] Compatibility
            - [ ] Interface
            - [ ] Packaging
            - [ ] Volts/Current
        - [ ] Datasheet
        - [ ] Builder reference
        - [ ] Farnell reference

- [ ] Example schematics of implementation of chosen components
    - [ ] Example diagrams instead if we don't know how to implement

### Protips

- We must use a PIC32 microcontroller of Microchip, no exceptions
- The only providers of school are Farnell & Olimex, the other must be paid with our own money
- The school take in charge PCBs and components (POC and prototypes) but not the "on sides" like Rasps, SD cards...

## Goals

- Lire des tags.
    - Autorisé
        - Sortie sur l'ecran
            - Granted
        - Action sur le relais
        - LED verte.
    - Refusé
        - Sortie sur l'écran
            - Refused
        - Buzz.
        - LED rouge.
- Ajouter/Supprimer des tags. (Via un ou plusieurs tags ***maitre*** / Port série)
    - Ajout
        - Sorties sur l'ecran
            - Temps d'Ajout
            - Confirmation
    - Suppression
        - Sorties sur l'ecran
            - Temps de suppression
            - Confirmation
    - Tag de maintenance
        - Sorties sur l'ecran
            - Statistiques
                - Slots restants
                - Uptime


### Obligatoire

- Proof of concept
    - Un lecteur RFID
    - Un écran
- Projet complet
    - Un port série
    - Un relai électrique

### Facultatifs (Bonus)

- Gestion du bluetooth
- Gestion de la stack IP
- Power Over Ethernet (Passif)
- Power Over Ethernet (Actif) # OP
