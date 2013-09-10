/**************************************************************************
	Souliss
    Copyright (C) 2013  Veseo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef SOULISS_MODRGB_H
#define SOULISS_MODRGB_H

#include "MODRGB.h"

#define Souliss_MODRGB_Init()			mRGB_Init();	\
										mRGB_Reset()
								
#define Souliss_MODRGB_SetColor(rgb)	mRGB_SetColor(rgb)

#endif