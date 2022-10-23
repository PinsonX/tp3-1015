#pragma once
#include <string>
#include "Liste.hpp"
#include "Concepteur.hpp"


struct Jeu
{
	std::string titre;
	int anneeSortie;
	std::string developpeur;
	Liste<Concepteur> concepteurs;

	shared_ptr<Concepteur> trouverConcepteur(const string concepteur)
	{
		return concepteurs.trouverCritere([concepteur](shared_ptr<Concepteur> element) {return concepteur == element->nom; });
	}
};
