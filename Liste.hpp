#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "gsl/span"

using namespace std;


template <class T>
class Liste
{
private:

	std::size_t nElements_ = 0, capacite_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_ = make_unique<shared_ptr<T>[]>(capacite_);


	void ajusterTaille(size_t capacite)
	{
		auto ptrAjuste = make_unique<shared_ptr<T>[]>(capacite);

		for (size_t i = 0; i < nElements_; i++)
		{
			ptrAjuste[i] = elements_[i];
		}

		capacite_ = capacite;
		elements_ = move(ptrAjuste);
	}

public:

	Liste() = default;
	
	Liste(const Liste<T>& liste)
	{
	
		
		for (size_t i = 0; i < liste.obtenirNElements(); i++)
		{
			ajouterElement(liste[i]);
		}
	}
	

	void ajouterElement(const shared_ptr<T> ptr)
	{
		if (nElements_ == capacite_)
		{
			ajusterTaille(max(capacite_*2, size_t(1)));
		}

		elements_[nElements_] = ptr;
		nElements_++;
	}

	template <typename PredicatUnaire>
	shared_ptr<T> trouverCritere(const PredicatUnaire& critere)
	{
		for (int i = 0; i < nElements_; i++)
		{
			if (critere(elements_[i]))
				return elements_[i];
		}
		return nullptr;
	}


	shared_ptr<T>& operator[] (const size_t indice) const
	{
		return elements_[indice];
	}



	size_t obtenirNElements() const
	{
		return nElements_;
	}


	size_t obtenirCapacite() const
	{
		return capacite_;
	}


	unique_ptr<shared_ptr<T>[]> obtenirElements() const
	{
		return elements_;
	}


	void ajusterNElements(const size_t nb)
	{
		nElements_ = nb;
	}


};
