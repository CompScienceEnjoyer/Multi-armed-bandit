#include <iostream>
#include <vector>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <cmath>




using namespace std;


class Multiarmed_bandit
{
protected:
	int resources, earning, step_price, amount_arms, num_picks_, amount_picks_;	//ресурсы агента, общий выигрыш, стоимость шага, количество ручек многорукого бандита
	vector<double> probabilities;									//вероятность выбора ручки(стратегия)
	vector<double> earn_per_arm;									//выигрыш за выбор конкретной ручки
	vector<int> amount_picks;										//количество выборов какой-либо ручки
	vector<int> already_earn;										//уже полученный выигрыш с какой-либо ручки
	vector<int> pick_order;											//порядок выбора ручек
	vector<int> earning_per_step;
public:
	

	//по умолчанию
	Multiarmed_bandit()
	{
		resources = 10;
		step_price = 1;
		amount_arms = 10;
		num_picks_ = 0;
		amount_picks_ = 0;
		
		probabilities.resize(amount_arms);
		fill(probabilities.begin(), probabilities.end(), 0.5);

		amount_picks.resize(amount_arms);
		fill(amount_picks.begin(), amount_picks.end(), 0);

		already_earn.resize(amount_arms);
		fill(already_earn.begin(), already_earn.end(), 0);

		pick_order.resize(amount_arms);
		fill(pick_order.begin(), pick_order.end(), 0);

		earning_per_step.resize(resources / step_price);
		fill(earning_per_step.begin(), earning_per_step.end(), 0);
		
		earn_per_arm.resize(amount_arms);
		for (auto it = earn_per_arm.begin(); it != earn_per_arm.end(); it++)
			*it = (rand() % 20);
		int random_arm = rand() % amount_arms;
		earn_per_arm[random_arm] += 50;
		
		earning = 0;
	}

	//конструктор случайного многорукого бандита по параметрам resources, step_price, amount_arms
	Multiarmed_bandit(int res, int s_p, int a_a, bool random_prob)
	{
		resources = res;
		step_price = s_p;
		amount_arms = a_a;
		num_picks_ = 0;
		amount_picks_ = 0;

		probabilities.resize(amount_arms);
		if(!random_prob)
			fill(probabilities.begin(), probabilities.end(), 0.5); 
		else
		{
			for (auto it = probabilities.begin(); it != probabilities.end(); it++)
				*it = ((double)rand() / (RAND_MAX));
		}

		amount_picks.resize(amount_arms);
		fill(amount_picks.begin(), amount_picks.end(), 0);

		already_earn.resize(amount_arms);
		fill(already_earn.begin(), already_earn.end(), 0);
		
		pick_order.resize(amount_arms);
		fill(pick_order.begin(), pick_order.end(), 0);
		
		earning_per_step.resize(resources / step_price);
		fill(earning_per_step.begin(), earning_per_step.end(), 0);

		earn_per_arm.resize(amount_arms);
		for (auto it = earn_per_arm.begin(); it != earn_per_arm.end(); it++)
			*it = (rand() % 20) + 1;
		
		int random_arm = rand() % amount_arms;
		earn_per_arm[random_arm] += 50;

		earning = 0;
	}


	//конструктор копий(будет полезен для замеров эффективности алгоритмов)
	Multiarmed_bandit(const Multiarmed_bandit& b)
	{
		resources = b.resources;
		earning = b.earning;
		step_price = b.step_price;
		amount_arms = b.amount_arms;
		probabilities = b.probabilities;
		earn_per_arm = b.earn_per_arm;
		amount_picks = b.amount_picks;
		already_earn = b.already_earn;
		pick_order = b.pick_order;
		num_picks_ = b.num_picks_;
		earning_per_step = b.earning_per_step;
		amount_picks_ = b.amount_picks_;
	}

	//выбор лучшей ручки исходя из имеющихся вероятностей
	void  pick_better_arm()
	{

		auto it = minmax_element(this->probabilities.begin(), this->probabilities.end());
		int max_ind = distance(this->probabilities.begin(), it.second);
		this->already_earn[max_ind] += this->earn_per_arm[max_ind];
		this->amount_picks[max_ind] += 1;
		earning += this->earn_per_arm[max_ind];
		earning_per_step[amount_picks_] += earn_per_arm[max_ind];
		this->probabilities[max_ind] = (double)earn_per_arm[max_ind] / amount_picks[max_ind];
		if (pick_order[max_ind] < 1)
		{
			pick_order[max_ind] += ++num_picks_;
		}
		amount_picks_ += 1;
	}

	//выбор случайной ручки
	void pick_random_arm()
	{
		
		int random_pick = rand() % amount_arms;
		this->already_earn[random_pick] += this->earn_per_arm[random_pick];
		this->amount_picks[random_pick] += 1;
		earning += this->earn_per_arm[random_pick];
		earning_per_step[amount_picks_] += earn_per_arm[random_pick];
		this->probabilities[random_pick] = (double)earn_per_arm[random_pick] / amount_picks[random_pick];
		if (pick_order[random_pick] < 1)
		{
			pick_order[random_pick] += ++num_picks_;
		}
		amount_picks_ += 1;
	}

	

	friend ostream& operator<<(ostream& os, const Multiarmed_bandit& bandit);


	int greedy_strategy()
	{
		while (resources >= step_price)
		{
			this->pick_better_arm();
			resources -= step_price;
			for (auto it = this->probabilities.begin(); it != this->probabilities.end(); it++)
				cout << *it << " ";
			cout << "\n";
		}
		for (auto it = pick_order.begin(); it != pick_order.end(); it++)
			cout << *it << " ";
		cout << "\n";

		for (auto it = earning_per_step.begin(); it != earning_per_step.end(); it++)
			cout << *it << " ";
		cout << "\n";

		return earning;
	}


	int softmax(double k)
	{
		int num_picks = 0;

		while (resources >= step_price)
		{
			amount_picks_ += 1;
			for (int i = 0; i < amount_arms; i++)
			{
				probabilities[i] =  exp(((double)(earning)/(double)num_picks)/k)/exp(double(earn_per_arm[i]) / ((double)amount_picks[i]) / k);
			}
			auto it = minmax_element(probabilities.begin(), probabilities.end());
			int max_ind = distance(probabilities.begin(), it.second);
			already_earn[max_ind] += earn_per_arm[max_ind];
			amount_picks[max_ind] += 1;

			if (pick_order[max_ind] < 1)
			{
				pick_order[max_ind] += ++num_picks_;
			}
			
			num_picks += 1;
			earning += earn_per_arm[max_ind];
			earning_per_step[amount_picks_] += earn_per_arm[max_ind];
			resources -= step_price;

			k -= 0.1;
			for (auto it = probabilities.begin(); it != probabilities.end(); it++)
				cout << *it << " ";
			cout << "\n";
		}
		
		for (auto it = pick_order.begin(); it != pick_order.end(); it++)
			cout << *it << " ";
		cout << "\n";

		for (auto it = earning_per_step.begin(); it != earning_per_step.end(); it++)
			cout << *it << " ";
		cout << "\n";

		return earning;
	}

	int random_greedy_strategy(double e)
	{
		while (resources >= step_price)
		{
			
			double r = ((double)rand() / (RAND_MAX));
			if (r <= e)
			{
				this->pick_random_arm();
			}
			else
			{
				this->pick_better_arm();
			}
			resources -= step_price;
			for (auto it = this->probabilities.begin(); it != this->probabilities.end(); it++)
				cout << *it << " ";
			cout << "\n";
		}
		
		for (auto it = pick_order.begin(); it != pick_order.end(); it++)
			cout << *it << " ";
		cout << "\n";

		for (auto it = earning_per_step.begin(); it != earning_per_step.end(); it++)
			cout << *it << " ";
		cout << "\n";

		return earning;
	}
};

ostream& operator<<(ostream& os, const Multiarmed_bandit& bandit)
{
	os << "\nresources: " << bandit.resources << "\nearning: " << bandit.earning << "\nstep price: " << bandit.step_price << "\namount arms: " << bandit.amount_arms << "\nprobabilities: ";
	for (auto it = bandit.probabilities.begin(); it != bandit.probabilities.end(); it++)
		os << *it << " ";
	os << "\nearning per arm: ";
	for (auto it = bandit.earn_per_arm.begin(); it != bandit.earn_per_arm.end(); it++)
		os << *it << " ";
	os << "\namount picks: ";
	for (auto it = bandit.amount_picks.begin(); it != bandit.amount_picks.end(); it++)
		os << *it << " ";
	os << "\nalready earn: ";
	for (auto it = bandit.already_earn.begin(); it != bandit.already_earn.end(); it++)
		os << *it << " ";
	os << "\npick order: ";
	for (auto it = bandit.pick_order.begin(); it != bandit.pick_order.end(); it++)
		os << *it << " ";
	return os;
}



int main()
{
	srand(time(NULL));
	Multiarmed_bandit bandit_1;
	Multiarmed_bandit bandit_2(20, 3, 7, false);
	//constructors
	cout << "Default constructor";
	cout << bandit_1;
	cout << "\n\nParameter-dependent constructor";
	cout << bandit_2;

	cout << "\n\nbandit_2 after first arm-pick: ";
	cout << bandit_2;
	//Greedy strategy
	Multiarmed_bandit bandit_3(15, 2, 7, false);
	cout << "\n\nGreedy strategy test: ";
	cout << "\nbandit_3: ";
	cout << bandit_3;
	cout << "\n--------Strategy results-----------\n";
	cout << bandit_3.greedy_strategy();
	cout << bandit_3;
	//random-greedy strategy
	Multiarmed_bandit bandit_4(15, 2, 7, false);
	cout << "\n\nRandom-greedy strategy test: ";
	cout << "\nbandit_4: ";
	cout << bandit_4;
	cout << "\n--------Strategy results-----------\n";
	cout << bandit_4.random_greedy_strategy(0.4);
	cout << bandit_4;
	//softmax strategy
	Multiarmed_bandit bandit_5(15, 2, 7, false);
	cout << "\n\nSoftmax strategy test: ";
	cout << "\nbandit_5: ";
	cout << bandit_5;
	cout << "\n--------Strategy results----------\n";
	cout << bandit_5.softmax(1.5);
	cout << bandit_5;

	cout << "\n\n\n\t\t\tComparing strategies\n\n\n";
	Multiarmed_bandit bandit_6_greedy(20, 3, 7, false);
	Multiarmed_bandit bandit_6_random_greedy(bandit_6_greedy);
	Multiarmed_bandit bandit_6_softmax(bandit_6_greedy);
	cout << "\n\n\t\t\tGreedy strategy:\n";
	cout << bandit_6_greedy.greedy_strategy();
	cout << "\n\n\t\t\tRandom-greedy strategy:\n";
	cout << bandit_6_random_greedy.random_greedy_strategy(0.25);
	cout << "\n\n\t\t\tSoftmax strategy:\n";
	cout << bandit_6_softmax.softmax(1.5);

	cout << "\n\n\n\t\t\tComparing strategies on big butch\n\n\n";
	Multiarmed_bandit bandit_7_greedy(500, 3, 50, false);
	Multiarmed_bandit bandit_7_random_greedy(bandit_7_greedy);
	Multiarmed_bandit bandit_7_softmax(bandit_7_greedy);
	cout << bandit_7_greedy << "\n\n";
	cout << "\n\n\t\t\tGreedy strategy: \n";
	cout << bandit_7_greedy.greedy_strategy();
	cout << "\n\n\t\t\tRandom-greedy strategy: \n";
	cout << bandit_7_random_greedy.random_greedy_strategy(0.25);
	cout << "\n\n\n\t\t\tSoftmax strategy: \n";
	cout << bandit_7_softmax.softmax(1.5);

	/*Multiarmed_bandit bandit_8_random_greedy_0_25(150, 3, 20, false);
	Multiarmed_bandit bandit_8_random_greedy_0_35(bandit_8_random_greedy_0_25);
	Multiarmed_bandit bandit_8_random_greedy_0_45(bandit_8_random_greedy_0_25);
	Multiarmed_bandit bandit_8_random_greedy_0_55(bandit_8_random_greedy_0_25);
	Multiarmed_bandit bandit_8_random_greedy_0_65(bandit_8_random_greedy_0_25);
	Multiarmed_bandit bandit_8_random_greedy_0_75(bandit_8_random_greedy_0_25);

	cout << "\n\n\t\tBandit:\n";
	cout << bandit_8_random_greedy_0_25;
	cout << "\n\n\t\ttest of various e parameters for the Random-greedy strategy";

	cout << "\n\n\t\tRandom-greedy e = 0.25: \n";
	cout << bandit_8_random_greedy_0_25.random_greedy_strategy(0.25);
	cout << "\n\n\t\tRandom-greedy e = 0.35: \n";
	cout << bandit_8_random_greedy_0_35.random_greedy_strategy(0.35);
	cout << "\n\n\t\tRandom-greedy e = 0.45: \n";
	cout << bandit_8_random_greedy_0_45.random_greedy_strategy(0.45);
	cout << "\n\n\t\tRandom-greedy e = 0.55: \n";
	cout << bandit_8_random_greedy_0_55.random_greedy_strategy(0.55);
	cout << "\n\n\t\tRandom-greedy e = 0.65: \n";
	cout << bandit_8_random_greedy_0_65.random_greedy_strategy(0.65);
	cout << "\n\n\t\tRandom-greedy e = 0.75: \n";
	cout << bandit_8_random_greedy_0_75.random_greedy_strategy(0.75);*/
}