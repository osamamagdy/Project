#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include "..\Defs.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
#include "..\GUI\GUI.h"
#include "..\Generic_DS\Queue.h"
#include "..\Events\Event.h"
#include "..\Generic_DS\BagList.h"
#include "..\Generic_DS\PriorityQueue.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "Order.h"




// it is the maestro of the project




class Restaurant  
{	
private:   // We should give here every data member used in the project, remember OOP course and don't break class resbonsibilities
	GUI *pGUI;


	Queue<Event*> EventsQueue;	//Queue of all events that will be loaded from file

	PQueue<Order*> Vorders;	//Priority Queue for all vip orders

	Queue<Order*> Norders;  // Queue for all normal orders

	Queue<Order*> Gorders;  // Queue for all vegan orders

	PQueue<Cook*> Vcooks;   //Priority Queue for all vip cooks

	PQueue<Cook*> Ncooks;   //Priority Queue for all normal cooks

	PQueue<Cook*> Gcooks;   //Priority Queue for all vegan cooks

	PQueue<Cook*>BusyCooks; // for cooks who took orders
	
	PQueue<Cook*>InjuredCooks; // for cooks who are injured
	
	PQueue<Cook*>BreakCooks; // for cooks who are in Break



	 

	PQueue<Order*>  prepare_Order;   //queue for served orders 
	Queue<Order*> finished_order; //done orders 

	
public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();
	void AddOrders(Order* po);
	void loadfile();
	void Restaurant_modes(int mode);
	void Seacrh( int ID, Order  *&frntEntry);

	void FillDrawingList();
	void autopormotedForNormal(int time);
	void urgentForVIP(int timestep);

	bool assignOrderVIP(int timestep);
	bool assignOrderVegan(int timestep);
	bool assignOrderNormal(int timestep);
	bool assignOrderInjured(int timestep, Order* orderptr); // is it by reference or value?
	bool assignOrderBreak(int timestep, Order* orderptr);
	void assignOrdertofinish(int timestep);
	void checkunavailblecooks(int timestep);
	//for output file 
	//algorithms to sort 
	
	//void reheapdown(Order **& arr, int n, int root);
	//void heapSort(Order **& arr, int n);
	void outputfile();
	void getavgSTandWT(Order** arr,int count , float& avgWT, float& avgST);
	void shellSort(Order* arr[], int n);

	//
	// TODO: Add More Member Functions As Needed
	//


	



/// ===================    DEMO-related functions. Should be removed in phases 1&2   ================= 

	//void Just_A_Demo();	//just to show a demo and should be removed in phase1 1 & 2
	//void AddtoDemoQueue(Order* po);	//adds an order to the demo queue

/// ================================================================================================== 



};

#endif