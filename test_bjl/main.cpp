#include <string>
#include <vector>
#include<time.h>
#include"baccarat.h"

void createCard(std::vector<int> &showedCard) {

	int total = 0;
	for (int i = 0; i < 10; i++) {
		if (i == 0) showedCard[i] = rand() % 128 ;
		else showedCard[i] = rand() % 32 ;
		total += showedCard[i];
	}
	for (int i = 0; i < 10; i++) {
		showedCard[i] = (int)(1.0 * showedCard[i] * 200) / total;
		if (i == 0) {
			if (showedCard[i] > 128) showedCard[i] = 128;
		}
		else {
			if (showedCard[i] > 32) showedCard[i] = 32;
		}
	}
}

void showCard(std::vector<int> &showedCard) {
	for (int i = 0; i < 10; i++)
		printf("%d ", showedCard[i]);
	printf("\n");
}

int getOneCard(int &leftCardNum) {
	//int leftCardNum = cardNum(leftCard);
	static std::default_random_engine e{ std::random_device{}() };
	std::uniform_int_distribution<unsigned> u(0, leftCardNum - 1);
	if (leftCardNum <= 0) return -1;
	int num = u(e);
	printf(" %d -- %d \n", leftCardNum, num);
	leftCardNum--;
	return num;
}

void test_oneLeft() {

	std::vector<int> showedCard(10, 0);

	int player, banker;
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		if (i == 0) showedCard[i] = rand() % 128;
		else showedCard[i] = rand() % 32;
		printf("%d ", showedCard[i]);
	}
	printf("\n");

	showedCard[0] = 0;
	showedCard[1] = 24;
	showedCard[2] = 24;
	showedCard[3] = 24;
	showedCard[4] = 24;
	showedCard[5] = 0;
	showedCard[6] = 0;
	showedCard[7] = 0;
	showedCard[8] = 0;
	showedCard[9] = 0;

	time_t old = time(NULL);
	player = banker = 0;
	Baccarat bjl;
	bjl.setLeftCard(showedCard);
	bjl.MonteCarlo(16, 20000, banker, player);

	float bRate, pRate;
	int money;
	int ret = bjl.rateToMoney(banker, player, bRate, pRate, money);
	printf("%d  %d  %f  %f  %d\n", player, banker, pRate, bRate, ret);	
}

void test_forLeft() {

	srand(time(NULL));
	std::vector<int> showedCard(10, 0);

	int player, banker;
	int pnum = 0;
	int bnum = 0;

	
	time_t old = time(NULL);
	int num = 0;
	int num1 = 0;
	int num2 = 0;
	int bcount[30];
	int pcount[30];
	player = banker = 0;
	float bRate, pRate;
	int money;
	
	float bt, pt;
	bt = pt = 0;
	std::vector<int> br = { 0,4 };
	std::vector<int> pr = { 0,3 };

	for (int i = 0; i < 30; i++) {
		/*for (int i = 0; i < 10; i++) {
			if (i == 0) showedCard[i] = rand() % 128;
			else showedCard[i] = rand() % 32;
			printf("%d ", showedCard[i]);
		}
		printf("\n");
		*/
		Baccarat bjl;
		bjl.setLeftCard(showedCard);
		bjl.setInitCard(br, pr);
		bjl.MonteCarlo(16, 200000, banker, player);
		//float bRate, pRate;
		//int money;
		int ret = bjl.rateToMoney(banker, player, bRate, pRate, money);
		printf("%d  %d  %f  %f  %d\n", player, banker, pRate, bRate, ret);
		num++;
		if (ret == 1) num1++;
		if (ret == 2) num2++;
		bt += bRate;
		pt += pRate;
		showedCard[0]++;
		showedCard[1]++;
		showedCard[2]++;
		showedCard[3]++;
		showedCard[4]++;
		showedCard[5]++;
	}
	printf("%lld   %f   %f   %d", time(NULL) - old, bt / 30, pt / 30, num1);
}
 

void test_meanLeft() {

	srand(time(NULL));
	std::vector<int> showedCard(10, 0);

	int player, banker;
	int pnum = 0;
	int bnum = 0;
	
	time_t old = time(NULL);
	int num = 0;
	int num1 = 0;
	int num2 = 0;
	int bcount[30];
	int pcount[30];
	player = banker = 0;
	float bRate, pRate;
	int money;
	Baccarat bjl;
	#pragma omp parallel for
	for (int i = 0; i < 30; i++) {
		//createCard(showedCard);		
		bjl.MonteCarlo(1, 20000, bcount[i], pcount[i]);
		//float bRate, pRate;
		//int money;
		//int ret = bjl.rateToMoney(banker, player, bRate, pRate, money);
	}
	for (int i = 0; i < 30; i++) {
		player += pcount[i];
		banker += bcount[i];
	}
	int ret = bjl.rateToMoney(banker, player, bRate, pRate, money);

	printf("%f  %f  %d\n", pRate, bRate, ret);
	

	bRate = 1.0 * banker / (banker + player);
	pRate = 1.0 * player / (banker + player);

	if (bRate > 0.513) {
		int money = (int)(10000 * (bRate *0.95 - pRate) / 0.95);
		showCard(showedCard);
		printf("%f  %f  %d\n", pRate, bRate, money);
		num++;
	}
	if (pRate > 0.5) {
		int money = (int)(10000 * (pRate  - bRate));
		showCard(showedCard);
		printf("%f  %f  %d\n", pRate, bRate, money);
		num++;
	}

	if (player > banker) pnum++;
	else if (player < banker) bnum++;
	else;
	
	printf("%lld  %d  %d  %f %f  %f\n", time(NULL) - old, pnum, bnum,
		1.0* pnum / (pnum + bnum), 1.0* bnum / (pnum + bnum),  1.0 / 1.95 );
}


int main(int argc, char **argv) {	
	test_forLeft();
	return 1;
}