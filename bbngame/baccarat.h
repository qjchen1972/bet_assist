#pragma once
#include<vector>
#include <random>

class Baccarat {
public:
	Baccarat(){
		leftCard = std::vector<int>(10, 0);
	}
	~Baccarat() {}

	int oneGame(std::vector<int> leftCard, int cardNum) {

		std::vector<int> player(3, -1);
		std::vector<int> banker(3, -1);
		//int leftCardNum = cardNum;

		if (!dealFourCard(leftCard, cardNum, player, banker)) return 0;

		procPlayer(leftCard, cardNum, player, banker);
		procBanker(leftCard, cardNum, player, banker);

		int playerNum = 0;
		int bankerNum = 0;
		for (int i = 0; i < 3; i++) {
			if (player[i] > 0) playerNum += player[i];
			if (banker[i] > 0) bankerNum += banker[i];
		}

		return (playerNum % 10) - (bankerNum % 10);
	}
	
	void setLeftCard(std::vector<int> &showedCard) {
#pragma omp parallel for
		for (int i = 0; i < leftCard.size(); i++) {
			if (i == 0)
				leftCard[i] = 4 * 32 - showedCard[i];
			else
				leftCard[i] = 32 - showedCard[i];
		}

	}

	int cardNum(std::vector<int> &card) {
		int total = 0;
		for (int i = 0; i < card.size(); i++)
			total += card[i];
		return total;
	}

	int num2Index(int num, std::vector<int> &card) {
		int total = 0;
		for (int i = 0; i < card.size(); i++) {
			total += card[i];
			if (num < total) return i;			
		}
		return -1;
	}

	void MonteCarlo(int iter, int num, int &banker, int &player) {

		banker = 0;
		player = 0;
		int leftCardNum = cardNum(leftCard);
		std::vector<int> bCount(iter, 0);
		std::vector<int> pCount(iter, 0);

#pragma omp parallel for
		for (int i = 0; i < iter; i++) {
			for (int j = 0; j < num; j++) {
				int ret = oneGame(leftCard, leftCardNum);
				if (ret > 0) pCount[i]++;
				else if (ret < 0) bCount[i]++;
				else;
			}
		}

		//float bRate;
		//float pRate;

		for (int i = 0; i < iter; i++) {		
			
			/*bRate = 1.0 * bCount[i] / (pCount[i] + bCount[i]);
			pRate = 1.0 * pCount[i] / (pCount[i] + bCount[i]);
			
			if (bRate > 0.513) {
				banker++;
				//printf("%.4f,%.4f,%d\n", pRate, bRate, 1);
			}
			else if (pRate > 0.5) {
				player++;
				//printf("%.4f,%.4f,%d\n", pRate, bRate, 2);
			}
			else {
				//printf("%.4f,%.4f,%d\n", pRate, bRate, 0);
			}*/

			banker += bCount[i];
			player += pCount[i];
		}
	}

	int rateToMoney(int banker, int player, float &bRate, float &pRate, int &money, int base=10000) {

		bRate = 1.0 * banker / (banker + player);
		pRate = 1.0 * player / (banker + player);

		/*if (bRate > 0.513) {
			money = (int)(base * (bRate *0.95 - pRate) / 0.95);
			return 1;
		}
		if (pRate > 0.5) {
			money = (int)(base * (pRate - bRate));
			return 2;
		}*/
		/*if (pRate > 0.4943 && pRate < 0.4953) {
			money = 10;
			return 2;
		}
		if (pRate > 0.4953 && pRate < 0.4963) {
			money = 20;
			return 2;
		}
		if (pRate > 0.4963) {
			money = 30;
			return 2;
		}*/
		if (pRate > 0.5) {
			money = 30;
			return 2;
		}

		if (pRate > 0.4913 && pRate < 0.4923) {
			money = 10;
			return 1;
		}
		if (pRate > 0.4903 && pRate < 0.4913) {
			money = 20;
			return 1;
		}
		if (pRate < 0.4903) {
			money = 30;
			return 1;
		}
		return 0;
	}

	/*int rateToMoney(int iter, int banker, int player, float &bRate, float &pRate, int &money, int base=10000) {

		int mid = iter - banker - player;
		if (mid > player && mid > banker) return 0;

		bRate = 1.0 * banker / iter;
		pRate = 1.0 * player / iter;

		if (bRate > pRate) {
			money = (int)(base * (bRate - pRate));
			return 1;
		}
		else {
			money = (int)(base * (pRate - bRate));
			return 2;
		}
		return 0;
	}*/

	int getOneCard(std::vector<int> &leftCard, int &leftCardNum){
		//int leftCardNum = cardNum(leftCard);
		static std::default_random_engine e{ std::random_device{}() };
	    std::uniform_int_distribution<unsigned> u(0, leftCardNum - 1);
		if (leftCardNum <= 0) return -1;
		int num = num2Index(u(e), leftCard);
		if (num < 0 || num >= leftCard.size()) return  -1;
		leftCard[num]--;
		leftCardNum--;
		return num;
	}

	int dealFourCard(std::vector<int> &leftCard, int &leftCardNum, 
		             std::vector<int> &player, std::vector<int> &banker) {
		int num;
		for (int i = 0; i < 4; i++) {
			num = getOneCard(leftCard, leftCardNum);
			if (num < 0) return 0;
			if(i < 2) player[i] = num;
			else banker[i - 2] = num;
		}
		return 1;
	}

	void procPlayer(std::vector<int> &leftCard, int &leftCardNum, 
		            std::vector<int> &player, std::vector<int> &banker) {
		int bankerNum = (banker[0] + banker[1]) % 10;
		if (bankerNum >= 8) return;

		int cardNum = (player[0] + player[1]) % 10;
		if (cardNum <= 5) player[2] = getOneCard(leftCard, leftCardNum);
	}

	void procBanker(std::vector<int> &leftCard, int &leftCardNum, 
		            std::vector<int> &player, std::vector<int> &banker) {

		int playerNum = (player[0] + player[1]) % 10;
		if (playerNum >= 8) return;

		int cardNum = (banker[0] + banker[1]) % 10;

		if (cardNum <= 2) banker[2] = getOneCard(leftCard, leftCardNum);
		else if (cardNum == 3) {
			if(player[2] != 8) banker[2] = getOneCard(leftCard, leftCardNum);
		}
		else if (cardNum == 4) {
			if(player[2] != 0 && player[2] != 1 && player[2] != 8 && player[2] != 9)
				banker[2] = getOneCard(leftCard, leftCardNum);
		}
		else if (cardNum == 5) {
			if (player[2] != 0 && player[2] != 1 && player[2] != 2 && player[2] != 3
				&& player[2] != 8 && player[2] != 9)
				banker[2] = getOneCard(leftCard, leftCardNum);
		}
		else if (cardNum == 6) {
			if(player[2] == 6 || player[2] == 7) banker[2] = getOneCard(leftCard, leftCardNum);
		}
		else;
	}
private:
	std::vector<int> leftCard;
	//int leftCardNum = 0;
};