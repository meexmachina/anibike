/*
 * AniBike2V7BT_Internal.h
 *
 * Created: 9/8/2011 12:05:44 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_INTERNAL_H_
#define ANIBIKE2V7BT_INTERNAL_H_

#define F_CPU 32000000

#ifdef _ANIBIKE_MASTER
	#include "AniBike2V7BT_Internal_master.h"
#else	// _ANIBIKE_SLAVE 
	#include "AniBike2V7BT_Internal_slave.h"
#endif


#endif /* ANIBIKE2V7BT_INTERNAL_H_ */