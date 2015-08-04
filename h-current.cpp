/*
Copyright (C) 2011 Georgia Institute of Technology

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

*/

/*
*   This program simulates the current caused by an H current in the cell.
*   The parameters are taken from Dickson, Magistretti, Shalinsky, Fransen, Hasselmo and Alonso,
*   Properties and Role of Ih in the pacing of subthreshold oscillations in entorhinal cortex
*   layer II Neurons, J. Neurophysiol. 83:2562-2579, 2000.
*/

#include <h-current.h>

extern "C" Plugin::Object *createRTXIPlugin(void) {
	return new G_H();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "Vin", "The voltage of the cell which Ih is to be added",
		DefaultGUIModel::INPUT, },
	{ "Iout", "Current Output", DefaultGUIModel::OUTPUT, },
	{ "MaxGh", "Siemens", DefaultGUIModel::PARAMETER
		| DefaultGUIModel::DOUBLE, },
	{ "Ih", "A", DefaultGUIModel::STATE, }, 
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

G_H::G_H(void) : DefaultGUIModel("H Conductance", ::vars, ::num_vars) {
	createGUI(vars, num_vars);
	Gh_max = 10e-12; //Peak conductance in Amps
	Vh_rev = -0.02; //Reversal potential of Ih channel not corrected for junction potential
	Ih = 0.0; //Display 0 upon initialization instead of junk
	update(INIT);
	refresh();
	resizeMe();
}

G_H::~G_H(void) {}

void G_H::execute(void) {
	Vm = input(0) - 0.01275; //membrane voltage corrected for junction potential;
	
	alpha_1 = AlphaBeta(-2.89e-3, -0.445, 24.02, Vm);
	beta_1 = AlphaBeta(2.71e-2, -1.024, -17.4, Vm);
	
	alpha_2 = AlphaBeta(-3.18e-3, -0.695, 26.72, Vm);
	beta_2 = AlphaBeta(2.16e-2, -1.065, -14.25, Vm);
	
	tau_h_fast = 1.0 / (alpha_1 + beta_1);
	tau_h_slow = 1.0 / (alpha_2 + beta_2);
	h_fast_inf = alpha_1 / (alpha_1 + beta_1);
	h_slow_inf = alpha_2 / (alpha_2 + beta_2);
	
	h_fast = h_fast_inf + (h_fast - h_fast_inf) * exp(-DeltaTSec / tau_h_fast);
	h_slow = h_slow_inf + (h_slow - h_slow_inf) * exp(-DeltaTSec / tau_h_slow);
	
	Im = -Gh_max * (0.65 * h_fast + 0.35 * h_slow) * (Vm - Vh_rev);
	
	Ih = Im;
	output(0) = Ih;
}

void G_H::update(DefaultGUIModel::update_flags_t flag) {
	switch (flag) {
		case INIT:
			setParameter("MaxGh", Gh_max);
			setState("Ih", Ih);
			break;

		case MODIFY:
			Gh_max = getParameter("MaxGh").toDouble();
			break;
	
		case PAUSE:
			output(0) = 0;
			break;
	
		default:
			break;
	}
	h_slow = 0;
	h_fast = 0;
	DeltaTSec = RT::System::getInstance()->getPeriod() * 1e-9;
}
