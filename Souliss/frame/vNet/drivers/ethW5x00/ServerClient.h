/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2012  Veseo

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
/**************************************************************************/

void srvcln_setsocket(uint8_t sock);
void srv_listen(uint16_t port);
uint8_t srvcln_connected(uint16_t port);
uint8_t srvcln_send(uint8_t *data, uint8_t len);
uint8_t srvcln_dataavailable(uint16_t port);
uint8_t srvcln_retrieve(uint8_t *data, uint8_t len);
void srvcln_stop();
void srvcln_num2str(uint8_t *data, uint8_t base, uint8_t *len);
