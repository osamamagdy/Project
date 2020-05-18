#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "Restaurant.h"
#include "..\Events\ArrivalEvent.h"
#include "..\Events\CancellationEvent.h" 
#include "..\Events\PromotionEvent.h" 
#include"Cook.h"
#include "Order.h"

Restaurant::Restaurant()
{
	pGUI = NULL;
}

void Restaurant::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE	mode = pGUI->getGUIMode();

	switch (mode)	//Add a function for each mode in next phases
	{
	case MODE_INTR:
		break;
	case MODE_STEP:
		break;
	case MODE_SLNT:
		loadfile();
		FillDrawingList();
		silenceMode();
		break;
	case MODE_DEMO:
		//Just_A_Demo();
		ExecuteEvents(30);
		


	};

}

void Restaurant::silenceMode()
{
	int timestep = 1;

	while (!EventsQueue.isEmpty())
	{
		bool Vflag = true;
		bool Gflag = true;
		bool Nflag = true;
		ExecuteEvents(timestep);
		checkunavailblecooks(timestep);
		assignOrdertofinish(timestep);
		while (!Vorders.isEmpty() && Vflag)
		{
			Vflag = assignOrderVIP(timestep);
		}
		while (Vorders.isEmpty() && !Gorders.isEmpty() && Gflag)
		{
			Gflag = assignOrderVegan(timestep);
		}
		while (Vorders.isEmpty() && !Norders.isEmpty() && Nflag)
		{
			Nflag = assignOrderNormal(timestep);
		}
		urgentForVIP(timestep);
		autopormotedForNormal();
		timestep++;
	}
}


void Restaurant::autopormotedForNormal()
{
	Queue<Order*> temp;
	Order* ptr;
	while (!Norders.isEmpty())
	{
		Norders.dequeue(ptr);
		int waitTime = ptr->IncWait();
		int prmotedTime = ptr->GetAUto();
		if (waitTime == prmotedTime)
		{
			ptr->SetType(TYPE_VIP);
			AddOrders(ptr);
			ptr->increase_promotion();
		}
		else
			temp.enqueue(ptr);
	}
	while (!temp.isEmpty())
	{
		temp.dequeue(ptr);
		Norders.enqueue(ptr);
	}

}
void Restaurant::urgentForVIP(int timestep)
{
	Queue<Order*> temp;
	Order* ptr;
	while (!Vorders.isEmpty())
	{
		Vorders.dequeue(ptr);
		int waitTime = ptr->IncWait();
		int urgentTime = ptr->getVIP_WT();
		if (waitTime == urgentTime)
		{
			if (!assignOrderVIP(timestep))
			{
				if (!assignOrderBreak(timestep, ptr))
				{
					if(!assignOrderInjured(timestep, ptr))
						temp.enqueue(ptr);
				}
			}
		}
		else
			temp.enqueue(ptr);
	}
	while (!temp.isEmpty())
	{
		temp.dequeue(ptr);
		Norders.enqueue(ptr);
	}
}

bool Restaurant::assignOrderVIP(int timestep)
{
	if (!Vcooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		//dequeue the cook from cooks queue and dequeue the order from order queue
		Vcooks.dequeue(the_cook);
		Vorders.dequeue(the_order);
		//set order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cook information 
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());
		return true;
	}
	else if (!Ncooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		Ncooks.dequeue(the_cook);
		Norders.dequeue(the_order);
		//set order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cook information 
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());

		return true;
	}
	else if (!Gcooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		Gcooks.dequeue(the_cook);
		Gorders.dequeue(the_order);
		//set order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cook information 
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());
		return true;
	}
	return false;
}
bool Restaurant::assignOrderVegan(int timestep)
{
	if (!Gcooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		Gcooks.dequeue(the_cook);
		Gorders.dequeue(the_order);
		//setting order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cooks informations
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());
		
		return true;
	}
	return false;
}
bool Restaurant::assignOrderNormal(int timestep)
{
	if (!Ncooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		Ncooks.dequeue(the_cook);
		Norders.dequeue(the_order);
		//setting order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cook informations 
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());
		
		return true;
	}
	else if (!Vcooks.isEmpty())
	{
		Order* the_order;
		Cook* the_cook;
		Vcooks.dequeue(the_cook);
		Vorders.dequeue(the_order);
		//setting order new status and waiting time and calculate service time and assign to preprinng orders
		the_order->setStatus(SRV);
		the_order->setwaittime(timestep - the_order->getorderarrivaltime());
		the_order->setservicetime(the_order->getOrderSize() / the_cook->getspeed());
		prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		//setting cook informations 
		the_cook->setStatus(UNAVILABLE);
		the_cook->setTimesteptobeavailabale(the_order->getOrderSize() / the_cook->getspeed() + timestep);
		the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
		the_cook->CalUnavailabalePriority(timestep);
		UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());
		return true;
	}
	return false;
}
bool Restaurant::assignOrderInjured(int timestep, Order* orderptr)
{
	bool IsAssined = false;
	Queue<Cook*> temp;
	Cook* ptr;
	while (!UnavailabaleCooks.isEmpty())
	{
		UnavailabaleCooks.dequeue(ptr);
		if (ptr->GetStatus() == INJURD && timestep >= (ptr->getTimesteptobeavailabale() - ptr->getRstPrd()))
		{
			ptr->setTimesteptobeavailabale(orderptr->getOrderSize() / ptr->getspeed() + timestep);
			ptr->CalUnavailabalePriority(timestep);
			IsAssined = true;
		}
		temp.enqueue(ptr);
	}
	while (!temp.isEmpty())
	{
		temp.dequeue(ptr);
		UnavailabaleCooks.enqueue(ptr, ptr->getUnavailabalePriority());
	}
	return IsAssined;
}
bool Restaurant::assignOrderBreak(int timestep, Order* orderptr)
{
	bool IsAssined = false;
	Queue<Cook*> temp;
	Cook* ptr;
	while (!UnavailabaleCooks.isEmpty())
	{
		UnavailabaleCooks.dequeue(ptr);
		if (ptr->GetStatus() == BREAK && timestep >= (ptr->getTimesteptobeavailabale() - ptr->getBreakduration()))
		{
			ptr->setTimesteptobeavailabale(orderptr->getOrderSize() / ptr->getspeed() + timestep + ptr->getBreakduration());
			ptr->CalUnavailabalePriority(timestep);
			IsAssined = true;
		}
		temp.enqueue(ptr);
	}
	while (!temp.isEmpty())
	{
		temp.dequeue(ptr);
		UnavailabaleCooks.enqueue(ptr, ptr->getUnavailabalePriority());
	}
	return IsAssined;
}

void Restaurant::assignOrdertofinish(int timestep)
{
	bool flag = true;
    Order* the_order;
	while (!prepare_Order.isEmpty()&& flag)
	{
		prepare_Order.dequeue(the_order);
		the_order->CalFinish();
		if (the_order->getFinshtime() == timestep)
		{
			finished_order.enqueue(the_order);
			the_order->setStatus(DONE);

		}
		else
		{
			flag = false;
			prepare_Order.enqueue(the_order, 1000 - the_order->getservicetime());
		}
	}

}

void Restaurant::checkunavailblecooks(int timestep)
{
	bool flag = true;
	Cook* the_cook;
	while (!(UnavailabaleCooks.isEmpty())&& flag)
	{
		UnavailabaleCooks.dequeue(the_cook);
		if (the_cook->getTimesteptobeavailabale() == timestep)
		{
			the_cook->setN_orders_Finished(the_cook->getN_orders_Finished() + 1);
			if (the_cook->getN_orders_Finished() >= the_cook->get_order_to_break())
			{
				the_cook->setStatus(BREAK);
				the_cook->setN_orders_Finished(0);
				the_cook->setTimesteptobeavailabale(timestep + the_cook->getBreakduration());
				the_cook->CalUnavailabalePriority(timestep);
				UnavailabaleCooks.enqueue(the_cook, the_cook->getUnavailabalePriority());

			}
			else
			{
				the_cook->setStatus(AVAILABLE);
				switch (the_cook->GetType())
				{
				case TYPE_NRM:
					Ncooks.enqueue(the_cook, the_cook->getspeed());
					break;
				case TYPE_VGAN:
					Gcooks.enqueue(the_cook, the_cook->getspeed());
					break;
				case TYPE_VIP :
					Vcooks.enqueue(the_cook, the_cook->getspeed());
					break;
				}
				flag = false;
			}
		}

	}
}




void Restaurant::loadfile()
{
	int Ncook, Gcook, Vcook, NspeedMin, NspeedMax, GspeedMin, GspeedMax, VspeedMin, VspeedMax, OrdersToBreak, NBreakMin, NBreakMax, GbreakMin, GbreakMax, VbreakMin, VbreakMax, AutoP, EventsNum;
	char EventType;
	char OTYP; 
	int TS, ID, SIZE;
	ORD_TYPE TYP;
	double MONY;
	int  RstPrd, VIP_WT;
	float InjProp;
	ifstream myfile;
	//pGUI->PrintMessage("Enter file input name.txt");
	//string file_name = pGUI->GetString();

	myfile.open("input.txt");

	myfile >> Ncook >> Gcook >> Vcook;
	myfile >> NspeedMin >> NspeedMax >> GspeedMin >> GspeedMax >> VspeedMin >> VspeedMax;
	myfile >> OrdersToBreak >> NBreakMin >> NBreakMax >> GbreakMin >>GbreakMax >>VbreakMin >> VbreakMax;
	myfile >>InjProp>>RstPrd;
	myfile >>AutoP>>VIP_WT;
	myfile >>EventsNum;


	//creating Ncooks
	Cook* newNcooks = new Cook[Ncook];
	//creating Gcooks
	Cook* newGcooks = new Cook[Gcook];
	//creating Vcooks
	Cook* newVcook = new Cook[Vcook];
	//setting number of cooks for output file 
	Cook::setVcount(Vcook);
	Cook::setNcount(Ncook);
	Cook::setGcount(Gcook);

	//setting static data type order to break 
	Cook::setordertobreak(OrdersToBreak);
	//setting static data type autopormotion
	Order::setautopormotion(AutoP);
	Order::SetVIP_WT(VIP_WT);
	//setting static
	Cook::SetRstPrd(RstPrd);
	//setting static
	Cook::SetInjProp(InjProp);
	//setting data for normal cooks and enqueue them
	for (int i = 0; i < Ncook; i++)
	{
		newNcooks[i].setSpeed(NspeedMin + rand() % NspeedMax);

		newNcooks[i].setType(TYPE_NRM);
		newNcooks[i].setBreakduration(NBreakMin + rand() % NBreakMin);

		newNcooks[i].setID(i + 1);
		//we should ask for bonous that every cook has different periorty depend on different speed
		Ncooks.enqueue(&newNcooks[i], newNcooks[i].getspeed());
	}
	//setting data for vegan cooks and enqueue them
	for (int i = 0; i < Gcook; i++)
	{
		newGcooks[i].setSpeed(GspeedMin + rand() % GspeedMax);

		newGcooks[i].setType(TYPE_VGAN);
		newGcooks[i].setBreakduration(GbreakMin + rand() % GbreakMax);

		newGcooks[i].setID(i + 1);
		//we should ask for bonous that every cook has different periorty depend on different speed
		Gcooks.enqueue(&newGcooks[i], newGcooks[i].getspeed());
	}
	//setting data for VIP cooks and enqueue them
	for (int i = 0; i < Vcook; i++)
	{
		newVcook[i].setSpeed(VspeedMin + rand() % VspeedMax);

		newVcook[i].setType(TYPE_VIP);
		newVcook[i].setBreakduration(VbreakMin + rand() % VbreakMax);

		newVcook[i].setID(i + 1);
		//we should ask for bonous that every cook has different periorty depend on different speed
		Vcooks.enqueue(&newVcook[i], newVcook[i].getspeed());
	}
	//reading events lines and set its data the enqueue them in events queue

	//pointer to base class event 
	Event* newevent;

	for (int i = 0; i < EventsNum; i++)
	{
		myfile >> EventType;

		switch (EventType)
		{
			//arrival event
		case 'R':
		{
			myfile >> OTYP >> TS >> ID >> SIZE >> MONY;
			switch (OTYP)
			{
			case 'N':
				TYP = TYPE_NRM;
				break;
			case 'G':
				TYP = TYPE_VGAN;
				break;
			case 'V':
				TYP = TYPE_VIP;
				break;
			}
			newevent = new ArrivalEvent(TS, ID, TYP, MONY, SIZE);
			break;
		}
		//cancellation event
		case 'X':
		{
			myfile >> TS >> ID;
			newevent = new CancellationEvent(TS, ID);
			break;
		}
		//Promotion event
		case 'P':
		{
			myfile >> TS >> ID >> MONY;
			newevent = new PromotionEvent(TS, ID, MONY);
			break;
		}

		}
		EventsQueue.enqueue(newevent);

	}
	myfile.close();

}

//////////////////////////////////  Event handling functions   /////////////////////////////

//Executes ALL events that should take place at current timestep
void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
	Event* pE;
	while (EventsQueue.peekFront(pE))	//as long as there are more events
	{
		if (pE->getEventTime() > CurrentTimeStep)	//no more events at current timestep
			return;

		pE->Execute(this);
		EventsQueue.dequeue(pE);	//remove event from the queue
		delete pE;		//deallocate event object from memory
	}

}


Restaurant::~Restaurant()
{
	if (pGUI)
		delete pGUI;
}

void Restaurant::FillDrawingList()
{
	//This function should be implemented in phase1
	//It should add ALL orders and Cooks to the drawing list
	Order* orderptr;
	Cook* cookptr;
	//orders copy 
	PQueue<Order*> Vorderscopy;
	Queue<Order*> Norderscopy;
	Queue<Order*> Gorderscopy;
	//cooks copy
	PQueue<Cook*> Vcookscopy;
	PQueue<Cook*> Ncookscopy;
	PQueue<Cook*> Gcookscopy;
	//Drawing orders using add to drawing list and copy it 
	while (Vorders.dequeue(orderptr))
	{
		pGUI->AddToDrawingList(orderptr);
		Vorderscopy.enqueue(orderptr, orderptr->getPriority());

	}
	while (Norders.dequeue(orderptr))
	{
		pGUI->AddToDrawingList(orderptr);
		Norderscopy.enqueue(orderptr);

	}
	while (Gorders.dequeue(orderptr))
	{
		pGUI->AddToDrawingList(orderptr);
		Gorderscopy.enqueue(orderptr);

	}
	//filling orders with copied data 
	while (Vorderscopy.dequeue(orderptr))
	{
		Vorders.enqueue(orderptr, orderptr->getPriority());
	}
	while (Norderscopy.dequeue(orderptr))
	{
		Norders.enqueue(orderptr);
	}
	while (Gorderscopy.dequeue(orderptr))
	{
		Gorders.enqueue(orderptr);
	}
	//Drawing cooks using add to drawing list and copy it 
	while (Vcooks.dequeue(cookptr))
	{
		pGUI->AddToDrawingList(cookptr);
		Vcookscopy.enqueue(cookptr, cookptr->getspeed());
	}
	while (Ncooks.dequeue(cookptr))
	{
		pGUI->AddToDrawingList(cookptr);
		Ncookscopy.enqueue(cookptr, cookptr->getspeed());
	}
	while (Gcooks.dequeue(cookptr))
	{
		pGUI->AddToDrawingList(cookptr);
		Gcookscopy.enqueue(cookptr, cookptr->getspeed());
	}
	//filling cooks with copied data 
	while (Vcookscopy.dequeue(cookptr))
	{
		Vcooks.enqueue(cookptr, cookptr->getspeed());
	}
	while (Ncookscopy.dequeue(cookptr))
	{
		Ncooks.enqueue(cookptr, cookptr->getspeed());
	}
	while (Gcookscopy.dequeue(cookptr))
	{
		Gcooks.enqueue(cookptr, cookptr->getspeed());
	}
	//update interface 
	pGUI->UpdateInterface();
	pGUI->ResetDrawingList();
}




//////////////////////////////////////////////////////////////////////////////////////////////
/// ==> 
///  DEMO-related functions. Should be removed in phases 1&2

//Begin of DEMO-related functions

//This is just a demo function for project introductory phase
//It should be removed starting phase 1
void Restaurant::Just_A_Demo()
{

	//
	// THIS IS JUST A DEMO FUNCTION
	// IT SHOULD BE REMOVED IN PHASE 1 AND PHASE 2

//	int EventCnt;	
//	Order* pOrd;
//	Event* pEv;
//	srand(time(NULL));
//
//	pGUI->PrintMessage("Just a Demo. Enter EVENTS Count(next phases should read I/P filename):");
//	EventCnt = atoi(pGUI->GetString().c_str());	//get user input as a string then convert to integer
//
//	pGUI->PrintMessage("Generating Events randomly... In next phases, Events should be loaded from a file...CLICK to continue");
//	pGUI->waitForClick();
//		
//	//Just for sake of demo, generate some cooks and add them to the drawing list
//	//In next phases, Cooks info should be loaded from input file
//	int C_count = 12;	
//	Cook *pC = new Cook[C_count];
//	int cID = 1;
//
//	for(int i=0; i<C_count; i++)
//	{
//		cID+= (rand()%15+1);	
//		pC[i].setID(cID);
//		pC[i].setType((ORD_TYPE)(rand()%TYPE_CNT));
//	}	
//
//		
//	int EvTime = 0;
//
//	int O_id = 1;
//	
//	//Create Random events and fill them into EventsQueue
//	//All generated event will be "ArrivalEvents" for the demo
//	for(int i=0; i<EventCnt; i++)
//	{
//		O_id += (rand()%4+1);		
//		int OType = rand()%TYPE_CNT;	//Randomize order type		
//		EvTime += (rand()%5+1);			//Randomize event time
//		pEv = new ArrivalEvent(EvTime,O_id,(ORD_TYPE)OType);
//		EventsQueue.enqueue(pEv);
//
//	}	
//
//	// --->   In next phases, no random generation is done
//	// --->       EventsQueue should be filled from actual events loaded from input file
//
//	
//	
//	
//	
//	//Now We have filled EventsQueue (randomly)
//	int CurrentTimeStep = 1;
//	
//
//	//as long as events queue is not empty yet
//	while(!EventsQueue.isEmpty())
//	{
//		//print current timestep
//		char timestep[10];
//		itoa(CurrentTimeStep,timestep,10);	
//		pGUI->PrintMessage(timestep);
//
//
//		//The next line may add new orders to the DEMO_Queue
//		ExecuteEvents(CurrentTimeStep);	//execute all events at current time step
//		
//
///////////////////////////////////////////////////////////////////////////////////////////
//		/// The next code section should be done through function "FillDrawingList()" once you
//		/// decide the appropriate list type for Orders and Cooks
//		
//		//Let's add ALL randomly generated Cooks to GUI::DrawingList
//		for(int i=0; i<C_count; i++)
//			pGUI->AddToDrawingList(&pC[i]);
//		
//		//Let's add ALL randomly generated Ordes to GUI::DrawingList
//		int size = 0;
//		//Order** Demo_Orders_Array = DEMO_Queue.toArray(size);
//		
//		for(int i=0; i<size; i++)
//		{
//			pOrd = Demo_Orders_Array[i];
//			pGUI->AddToDrawingList(pOrd);
//		}
///////////////////////////////////////////////////////////////////////////////////////////
//
//		pGUI->UpdateInterface();
//		Sleep(1000);
//		CurrentTimeStep++;	//advance timestep
//		pGUI->ResetDrawingList();
//	}
//
//	delete []pC;
//
//
//	pGUI->PrintMessage("generation done, click to END program");
//	pGUI->waitForClick();


}
////////////////

//void Restaurant::AddtoDemoQueue(Order *pOrd)
//{
//	DEMO_Queue.enqueue(pOrd);
//}

/// ==> end of DEMO-related function
//////////////////////////////////////////////////////////////////////////////////////////////
void Restaurant::AddOrders(Order* po)
{
	if (po->GetType() == TYPE_NRM)
	{
		Norders.enqueue(po);
		//add order for norma;
	}
	else if (po->GetType() == TYPE_VGAN)
	{
		Gorders.enqueue(po);
		//add order for vegan
	}
	else if (po->GetType() == TYPE_VIP)
	{
		po->CalPriority();
		Vorders.enqueue(po, po->getPriority());
		//add order for Vip
	}

}
void Restaurant::Seacrh(int Time, int ID, Order*& frntEntry)
{
	Queue<Order*> qtemp;
	Order* Otemp;
	//search about order
	while (Norders.dequeue(Otemp))
	{
		if (Otemp->GetID() != ID)
			qtemp.enqueue(Otemp);
		else
			frntEntry = Otemp;

	}

	while (qtemp.dequeue(Otemp))
		Norders.enqueue(Otemp);
}

void Restaurant::reheapdown(Order **& arr, int n, int root)
{
	int largest = root; // root is the largest element
	int l = 2 * root + 1; // left = 2*root + 1
	int r = 2 * root + 2; // right = 2*root + 2
	
	// If left child is larger than root
	if (l < n && arr[l]->getFinshtime() > arr[largest]->getFinshtime())
		largest = l;
	
	//if left child is equal the largest so far 
	if (l < n && arr[r]->getFinshtime() == arr[largest]->getFinshtime())
		if (arr[l]->getorderarrivaltime() > arr[largest]->getorderarrivaltime())
			largest = l;

	// If right child is larger than largest so far
	if (r < n && arr[r]->getFinshtime() > arr[largest]->getFinshtime())
		largest = r;
	
	//if right child is equal the largest so far 
	if (r < n && arr[r]->getFinshtime() == arr[largest]->getFinshtime())
		if (arr[r]->getorderarrivaltime() > arr[largest]->getorderarrivaltime())
			largest = r;
	// If largest is not root
	if (largest != root)
	{
		//swap root and largest
		
		swap(arr[root], arr[largest]);

		// Recursively heapify the sub-tree
		reheapdown(arr, n, largest);
	}
}

void Restaurant::heapSort(Order **& arr, int n)
{
	// build heap
	for (int i = n / 2 - 1; i >= 0; i--)
		reheapdown(arr, n, i);

	// extracting elements from heap one by one
	for (int i = n - 1; i >= 0; i--)
	{
		// Move current root to end
		swap(arr[0], arr[i]);

		// again call max heapify on the reduced heap
		reheapdown(arr, i, 0);
	}
}

void Restaurant::outputfile()
{
	//sorting the finish queue 
	int count = 0;
	Order ** finisharr =finished_order.toArray(count);
	heapSort(finisharr, count);
	pGUI->PrintMessage("Enter file output name ");
	string file_name=pGUI->GetString();

	ofstream myfile;
	myfile.open(file_name);
	//writing finish time and id and arrival time and wait time and service time of each order
	myfile << "FT" << " " << "ID" << " " << "AT" << " " << "WT" << " " << "ST" << "\n";
	for (int i = 0; i < count; i++)
	{
		myfile << finisharr[i]->getFinshtime() << "  " << finisharr[i]->GetID() << " ";
		myfile << finisharr[i]->getorderarrivaltime() << " " << finisharr[i]->getwaittime() << " ";
		myfile<< finisharr[i]->getservicetime()<<"\n";
	}
	//writing num of all orders and num of all types of orders
	myfile << "Orders: " << Order::getordercount() << " [Norm:" << Order::getNordercount();
	myfile<< ", Veg:" << Order::getGordercount() << ", VIP:" << Order::getVordercount() << "]\n";
	//writing num of all cooks and num of all types of cooks
	//myfile << "cooks: " << Cook::Getcookscount() << " [Norm:" << Cook::GetNcount() << ", Veg:" << Cook::GetGcount() << ", VIP" << Cook::GetVcount();
	//myfile << ", injured:" <</*injurynum*/"]" << "\n";
	

}
