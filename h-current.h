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

#include <default_gui_model.h>
#include <math.h>

class G_H : public DefaultGUIModel {

	public:
		G_H(void);
		virtual ~G_H(void);

		void execute(void);
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
	
	private:
		double AlphaBeta(double a, double b, double k, double Vm) {
			return (a * Vm + b) / (1 - exp((Vm + b / a) / k));
		};
	
		double gain;
		double offset;
		double Vh_rev;
		double Gh_max;
		double h_fast, h_slow;
	
		double h_slow_inf, h_fast_inf, tau_h_slow, tau_h_fast;
		double Vm, Im;
		double alpha_1, beta_1, alpha_2, beta_2;
		double DeltaTSec;
		double Ih;
};

