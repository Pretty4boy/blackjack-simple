
#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm> 
#include <ctime> 

using namespace std;

class Card
{
public:
	enum rank {
		ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
		JACK, QUEEN, KING};
	enum suit { CLUBS, DIAMONDS, HEARTS, SPADES };
	friend ostream& operator <<(ostream& os, const Card& aCard);
	Card(rank r = ACE, suit s = SPADES, bool ifu = true);
	int GetValue() const;
	void Flip();
private:
	rank m_Rank;
	suit m_Suit;
	bool m_IsFaceUp;
};

Card::Card(rank r, suit s, bool ifu) : m_Rank(r), m_Suit(s), m_IsFaceUp(ifu)
{}

int Card::GetValue() const
{
	int value = 0;
	if (m_IsFaceUp)
	{
		value = m_Rank;
		if (value > 10)
		{
			value = 10;
		}
	}
	return value;
}

void Card::Flip()
{
	m_IsFaceUp = !(m_IsFaceUp);
}

class Hand
{
public:
	Hand();
	virtual ~Hand();
	void Add(Card* pCard);
	void Clear();
	int GetTotal() const;
protected:
	vector<Card*> m_Cards;
};

Hand::Hand()
{
	m_Cards.reserve(7);
}

Hand::~Hand()
{
	Clear();
}

void Hand::Add(Card* pCard)
{
	m_Cards.push_back(pCard);
}

void Hand::Clear()
{
	vector<Card*>::iterator iter = m_Cards.begin();
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		delete* iter;
		*iter = 0;
	}
	m_Cards.clear();
}

int Hand::GetTotal() const
{
	if (m_Cards.empty())
	{
		return 0;
	}
	if (m_Cards[0]->GetValue() == 0)
	{
		return 0;
	}
	int total = 0;
	vector<Card*>::const_iterator iter;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		total += (*iter)->GetValue();
	}
	bool constainsAce = false;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		if ((*iter)->GetValue() == Card::ACE)
		{
			constainsAce = true;
		}
	}
	if (constainsAce && total <= 11)
	{
		total += 10;
	}
	return total;
}

class GenericPlayer : public Hand
{
	friend ostream& operator <<(ostream& os, const GenericPlayer& aGenericPlayer);
public:
	GenericPlayer(const string& name = "");
	virtual ~GenericPlayer();
	virtual bool IsHitting() const = 0;
	bool isBusted() const;
	void Bust() const;
protected:
	string m_Name;
};

GenericPlayer::GenericPlayer(const string& name):
 m_Name (name)
{}

GenericPlayer::~GenericPlayer()
{}

bool GenericPlayer::isBusted() const
{
	return (GetTotal() > 21);
}

void GenericPlayer::Bust() const
{
	cout << m_Name << " have end in the game." << endl;
}

class Player : public GenericPlayer
{
public:
	Player(const string& name = "");
	virtual ~Player();
	bool IsHitting() const;
	void Win() const;
	void Lose() const;
	void Push() const;
};

Player::Player(const string& name):
GenericPlayer(name)
{}

Player::~Player()
{}

bool Player::IsHitting() const
{
	cout << m_Name << ", do u want to get card? (Y/N): ";
	char response;
	cin >> response;
	return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
	cout << m_Name << " wins." << endl;
}

void Player::Lose() const
{
	cout << m_Name << " loses. " << endl;
}


void Player::Push() const
{
	cout << m_Name << " pushes. " << endl;
}

class House : public GenericPlayer
{
public:
	House(const string& name = "House");
	virtual ~House();
	bool IsHitting() const;
	void FlipFirstCard();
};
	
House::House(const string& name):
	GenericPlayer(name)
{}

House::~House()
{}

bool House::IsHitting() const
{
	return (GetTotal() <= 16);
}

void House::FlipFirstCard()
{
	if (!(m_Cards.empty())) {
			m_Cards[0]->Flip();
	}
	else
	{
		cout << "No card to flip!" << endl;
	}
}

class Deck : public Hand
{
public:
	Deck();
	virtual ~Deck();
	void Populate();
	void Shuffle();
	void Deal(Hand& aHand);
	void AdditionalCards(GenericPlayer& aGenericPlayer);
};
	
	Deck::Deck()
{
		m_Cards.reserve(52);
		Populate();
}

Deck::~Deck()
{}

void Deck::Populate()
{
	Clear();
	for (int s = Card::CLUBS; s <= Card::SPADES; ++s)
	{
		for (int r = Card::ACE; r <= Card::KING; ++r)
		{
			Add(new Card(static_cast<Card::rank>(r), static_cast<Card::suit>(s)));
		}
	}
}

void Deck::Shuffle()
{
	random_shuffle(m_Cards.begin(), m_Cards.end());
}

void Deck::Deal(Hand& aHand)
{
	if (!m_Cards.empty())
	{
		aHand.Add(m_Cards.back());
		m_Cards.pop_back();
	}
	else
	{
		cout << "Out of cards. Unable to deal. ";
	}
}

void Deck::AdditionalCards(GenericPlayer& aGenericPlayer)
{
	cout << endl;
	while (!(aGenericPlayer.isBusted()) && aGenericPlayer.IsHitting())
	{
		Deal(aGenericPlayer);
		cout << aGenericPlayer << endl;
		if (aGenericPlayer.isBusted())
		{
			aGenericPlayer.Bust();
		}
	}
}

class Game
{
public:
	Game(const vector<string>& names);
	~Game();
	void Plау();
private:
	Deck m_Deck;
	House m_House;
	vector<Player> m_Players;
};

Game::Game(const vector<string>& names)
{
	vector<string>::const_iterator pName;
	for (pName = names.begin(); pName != names.end(); ++pName)
	{
		m_Players.push_back(Player(*pName));
	}

	srand(static_cast<unsigned int>(time(0)));
	m_Deck.Populate();
	m_Deck.Shuffle();
}

Game::~Game()
{}

void Game::Plау()
{
	vector<Player>::iterator pPlayer;
	for (int i = 0; i < 2; ++i)
	{
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			m_Deck.Deal(*pPlayer);
		}
		m_Deck.Deal(m_House);
	}
	m_House.FlipFirstCard();
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		cout << *pPlayer << endl;
	}
	cout << m_House << endl;
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		m_Deck.AdditionalCards(*pPlayer);
	}
	m_House.FlipFirstCard();
	cout << endl << m_House;
	m_Deck.AdditionalCards(m_House);
	if (m_House.isBusted())
	{
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			if (!(pPlayer->isBusted()))
			{
				pPlayer->Win();
			}
		}
	}
	else
	{
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			if (!(pPlayer->isBusted()))
			{
				if (pPlayer->GetTotal() > m_House.GetTotal())
				{
					pPlayer->Win();
				}
				else if (pPlayer->GetTotal() < m_House.GetTotal())
				{
					pPlayer->Lose();
				}
				else
				{
					pPlayer->Push();
				}
			}
		}
	}
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		pPlayer->Clear();
	}
	m_House.Clear();
}

ostream& operator<<(ostream& os, const Card& aCard);
ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

	int main()
{
		cout << "\t\tWelcome to Blackjack!\n\n";
		int numPlayers = 0;
		while (numPlayers < 1 || numPlayers > 7)
		{
			cout << "How many players? (1 - 7): ";
			cin >> numPlayers;
		}
			vector<string> names;
			string name;
			for (int i = 0; i < numPlayers; ++i)
			{
				cout << "Enter player name: ";
				cin >> name;
				names.push_back(name);
			}
				cout << endl;
				Game aGame(names);
				char again = 'у';
				while (again != 'n' && again != 'N')
				{
					aGame.Plау();
					cout << "\nDo you want to play again? (Y/N): ";
					cin >> again;
				}
				return 0;
			}

ostream& operator<<(ostream & os, const Card & aCard)
{
	const string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9",
	"10", "J", "Q", "K" };
	const string SUIТS[] = { "c", "d", "h", "s" };
	if (aCard.m_IsFaceUp)
		{
			os << RANKS[aCard.m_Rank] << SUIТS[aCard.m_Suit];
	}
	else
	{
		os << "ХХ";
	}
return os;
}

ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer)
{
	os << aGenericPlayer.m_Name << ":\t";
	vector<Card*>::const_iterator pCard;
	if (!aGenericPlayer.m_Cards.empty())
	{
		for (pCard = aGenericPlayer.m_Cards.begin();
			pCard != aGenericPlayer.m_Cards.end();
			++pCard)
		{
			os << *(*pCard) << "\t";
		}
		if (aGenericPlayer.GetTotal() != 0)
		{
			cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	}
	else
	{
		os << "<empty>";
	}
	return os;
}
			// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
	// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

	// Советы по началу работы 
	//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
	//   2. В окне Team Explorer можно подключиться к системе управления версиями.
	//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
	//   4. В окне "Список ошибок" можно просматривать ошибки.
	//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
	//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
