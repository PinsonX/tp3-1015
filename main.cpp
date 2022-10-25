#include "Liste.hpp"
#include "Concepteur.hpp"
#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, string nom)
{
	for (int i = 0; i < listeJeux.obtenirNElements(); i++) {
		
		for (int j = 0; j < listeJeux[i]->concepteurs.obtenirNElements(); j++) {
			
			if (listeJeux[i]->concepteurs[j]->nom == nom)
				return listeJeux[i]->concepteurs[j];
		}
	}
	
	return nullptr;
}


shared_ptr<Concepteur> lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur = {}; 
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);
	
	shared_ptr<Concepteur> concepteurExistant = trouverConcepteur(listeJeux, concepteur.nom);
	if (concepteurExistant != nullptr)
		return concepteurExistant;

	
	cout << "\033[92m" << "Allocation en mémoire du concepteur " << concepteur.nom
				<< "\033[0m" << endl;
	return make_shared<Concepteur>(concepteur); 
}



shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	size_t nbConcepteurs = lireUintTailleVariable(fichier);
	
	shared_ptr ptrJeu = make_shared<Jeu>(jeu); 
	cout << "\033[96m" << "Allocation en mémoire du jeu " << jeu.titre
			  << "\033[0m" << endl;
	
	
	for (int i = 0; i < nbConcepteurs; i++) {

		ptrJeu->concepteurs.ajouterElement(lireConcepteur(fichier, listeJeux));
	}
	return ptrJeu; 
}

Liste<Jeu> creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	Liste<Jeu> listeJeux;
	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		listeJeux.ajouterElement(lireJeu(fichier, listeJeux)); 
	}

	return listeJeux; 
}


static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

ostream& operator<< (ostream& stream, const Liste<Concepteur>& concepteurs)
{
	for (int i = 0; i < concepteurs.obtenirNElements(); i++)
		stream << "\t" << concepteurs[i]->nom << ", " << concepteurs[i]->anneeNaissance << ", " << concepteurs[i]->pays << "\n";
	
	return stream;
}

ostream& operator<< (ostream& stream, const Liste<Jeu>& jeux)
{
	for (int i = 0; i < jeux.obtenirNElements(); i++)
	{
		stream << ligneSeparation;

		stream << "Titre : " << jeux[i]->titre << "\n" << "Parution : " << jeux[i]->anneeSortie << "\n" << "Developpeur : " << jeux[i]->developpeur << "\n";
		stream << "Concepteurs du jeu : \n";

		stream << jeux[i]->concepteurs;
	}

	return stream;
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
#pragma endregion

	//int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	Liste<Jeu> lj = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	//Nombre de jeux et capacite après création
	cout << ligneSeparation;
	cout << "Nombre de jeux : " << lj.obtenirNElements() << endl;
	cout << "Capacite : " << lj.obtenirCapacite() << endl;
	cout << ligneSeparation << endl;
	
	//Test operateur []
	cout << "Titre du deuxieme jeu de lj : " << lj[2]->titre << endl;
	cout << "Nom du premier concepteur du jeu ci-dessus : " << lj[2]->concepteurs[1]->nom << endl;
	cout << ligneSeparation << endl;

	//Test fonction de recherche par critere
	shared_ptr premierConcepteur = lj[0]->trouverConcepteur([](shared_ptr<Concepteur> element) {return "Yoshinori Kitase" == element->nom; });
	shared_ptr deuxiemeConcepteur = lj[1]->trouverConcepteur([](shared_ptr<Concepteur> element) {return "Yoshinori Kitase" == element->nom; });
	
	cout << "Adresse du premier pointeur : " << premierConcepteur.get() << endl;
	cout << "Adresse du deuxieme pointeur : " << deuxiemeConcepteur.get() << '\n' << endl;
	cout << "Date de naissance : " << premierConcepteur->anneeNaissance << endl;
	cout << ligneSeparation << endl;

	//Test operateur <<
	cout << lj << ligneSeparation << endl;
	ofstream("sortie.txt") << lj;

	//Test copie
	Jeu copieJeu = *lj[2];
	copieJeu.concepteurs[1] = lj[2]->concepteurs[0];

	cout << "Jeu à l'indice 2 : " << '\n' << endl;
	cout << "Titre : " << lj[2]->titre << "\n" << "Parution : " << lj[2]->anneeSortie << "\n" << "Developpeur : " << lj[2]->developpeur << endl;
	cout << "Concepteurs du jeu : " << endl;
	cout << lj[2]->concepteurs << endl;

	cout << "Copie du jeu : " << '\n' << endl;
	cout << "Titre : " << copieJeu.titre << "\n" << "Parution : " << copieJeu.anneeSortie << "\n" << "Developpeur : " << copieJeu.developpeur << endl;
	cout << "Concepteurs du jeu : " << endl;
	cout << copieJeu.concepteurs << endl;

	cout << "Adresse du premier concepteur : " << lj[2]->concepteurs[0].get() << endl;
	cout << "Adresse du premier concepteur dans la copie : " << copieJeu.concepteurs[0].get() << endl;


}