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

	template <typename PredicatUnaire>
	shared_ptr<Concepteur> trouverConcepteur(const PredicatUnaire& critere)
	{
		return concepteurs.trouverCritere(critere);
	}
};
