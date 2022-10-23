#include "Liste.hpp"
#include "Concepteur.hpp"
#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
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

//TODO: Fonction qui cherche un concepteur par son nom dans une ListeJeux.
// Cette fonction renvoie le pointeur vers le concepteur si elle le trouve dans
// un des jeux de la ListeJeux. En cas contraire, elle renvoie un pointeur nul.
shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, string nom)
{
	for (int i = 0; i < listeJeux.obtenirNElements(); i++) {
		// Normalement on voudrait retourner un pointeur const, mais cela nous
		// empêcherait d'affecter le pointeur retourné lors de l'appel de cette
		// fonction.
		for (int j = 0; j < listeJeux[i]->concepteurs.obtenirNElements(); j++) {
			
			return listeJeux[i]->trouverConcepteur(nom);
		}
	}
	
}


shared_ptr<Concepteur> lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur = {}; // On initialise une structure vide de type Concepteur.
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);
	// Rendu ici, les champs précédents de la structure concepteur sont remplis
	// avec la bonne information.

	//TODO: Ajouter en mémoire le concepteur lu. Il faut revoyer le pointeur créé.
	// Attention, valider si le concepteur existe déjà avant de le créer, sinon
	// on va avoir des doublons car plusieurs jeux ont des concepteurs en commun
	// dans le fichier binaire. Pour ce faire, cette fonction aura besoin de
	// la liste de jeux principale en paramètre.
	// Afficher un message lorsque l'allocation du concepteur est réussie.
	shared_ptr<Concepteur> concepteurExistant = trouverConcepteur(listeJeux, concepteur.nom);
	if (concepteurExistant != nullptr)
		return concepteurExistant;

	//cout << concepteur.nom << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
	cout << "\033[92m" << "Allocation en mémoire du concepteur " << concepteur.nom
				<< "\033[0m" << endl;
	return make_shared<Concepteur>(concepteur); //TODO: Retourner le pointeur vers le concepteur crée.
}



shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu = {}; // On initialise une structure vide de type Jeu
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	jeu.concepteurs.ajusterNElements((size_t) lireUintTailleVariable(fichier));
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.

	//TODO: Ajouter en mémoire le jeu lu. Il faut revoyer le pointeur créé.
	// Attention, il faut aussi créer un tableau dynamique pour les concepteurs
	// que contient un jeu. Servez-vous de votre fonction d'ajout de jeu car la
	// liste de jeux participé est une ListeJeu. Afficher un message lorsque
	// l'allocation du jeu est réussie.
	auto ptrJeu =  make_shared<Jeu>(jeu);  // Ou allouer directement au début plutôt qu'en faire une copie ici.
	cout << "\033[96m" << "Allocation en mémoire du jeu " << jeu.titre
			  << "\033[0m" << endl;
	// cout << jeu.titre << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
	
	for (int i = 0; i < jeu.concepteurs.obtenirNElements(); i++) {
		
		jeu.concepteurs.ajouterElement(lireConcepteur(fichier, listeJeux)); //TODO: Ajouter le jeu à la liste des jeux auquel a participé le concepteur.
	}
	return ptrJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

Liste<Jeu> creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	Liste<Jeu> listeJeux;
	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		listeJeux.ajouterElement(lireJeu(fichier, listeJeux)); //TODO: Ajouter le jeu à la ListeJeux.
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}


void afficherConcepteur(const Concepteur& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
			  << endl;
}



//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses concepteurs.
// Servez-vous de la fonction afficherConcepteur ci-dessus.
void afficherJeu(const Jeu& j)
{
	cout << "Titre : " << "\033[94m" << j.titre << "\033[0m" << endl;
	cout << "Parution : " << "\033[94m" << j.anneeSortie << "\033[0m"
			  << endl;
	cout << "Développeur :  " << "\033[94m" << j.developpeur << "\033[0m"
			  << endl;
	cout << "Concepteurs du jeu :" << "\033[94m" << endl;
	




	cout << "\033[0m";
}

static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

ostream& operator<< (ostream& stream, const Liste<Concepteur>& concepteurs)
{
	for (int i = 0; i < concepteurs.obtenirNElements(); i++)
		stream << "\t" << concepteurs[i]->nom << ", " << concepteurs[i]->anneeNaissance << ", " << concepteurs[i]->pays << "\n";
}

ostream& operator<< (ostream& stream, const Liste<Jeu>& jeux)
{
	for (int i = 0; i < jeux.obtenirNElements(); i++)
	{
		stream << ligneSeparation;

		stream << "Titre : " << jeux[i]->titre << "\n " << "Parution : " << jeux[i]->anneeSortie << "\n " << "Developpeur : " << jeux[i]->developpeur << "\n";
		stream << "Concepteurs du jeu : \n";

		stream << jeux[i]->concepteurs;

		stream << ligneSeparation;
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


	cout << lj[2]->titre << endl;
	cout << lj[2]->concepteurs[1] << endl;

}