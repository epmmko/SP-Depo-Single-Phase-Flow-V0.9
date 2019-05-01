
#include "coolant_condition.h"
#include "deposit_properties.h"
#include "fluid_properties.h"
#include "inlet_condition.h"
#include "single_phase_deposition.h"
#include "solid_properties.h"
#include "pipe_geometry.h"
#include "user_equation_format.h"
#include "simulation_options.h"

namespace spdepo{
void CheckDepositionLibrary() {
  //create input for deposition object
  Fluid oil;
  oil.SetConstantProperties("oil", 850, 0.003, 2000, 0.13);
  InletCondition inlet;
  inlet.SetProperties("case0", 4, 0, (105 - 32.0) / 1.8);
  //set mu vs T for oil
  usr::BigEquation oil_mu, oil_rho;
  usr::Equation mu_0, mu_1, mu_2, rho_0;
  mu_0.SetId(3);
  mu_0.SetCoefficient({ 0.0, 1000.0, 1.0, 273.15, 0.0 });
  mu_1.SetId(2);
  mu_1.SetCoefficient({ 2.533129,-11.906456,14.023469 });
  mu_2.SetId(4);
  mu_2.SetCoefficient({ 0.001,1.0 });
  oil_mu.SetEquationVector({ mu_0,mu_1,mu_2 });
  oil.SetFunctionalProperties(usr::kFluid::mu, oil_mu);
  //set rho vs T for oil
  rho_0.SetId(1);
  rho_0.SetCoefficient({ -0.738, 876.62 });
  oil_rho.SetEquationVector({ rho_0 });
  oil.SetFunctionalProperties(usr::kFluid::rho, oil_rho);
  //make pipe object
  PipeGeometry pipe;
  pipe.SetProperties("pipe0", 0.651*0.0254, 0.75*0.0254,
    50.0e-6, 0.995*0.0254, 50.0e-6, 0.0, 0.7, 0.84);
  //make coolant inlet
  CoolantCondition coolant_inlet;
  coolant_inlet.SetAll(3.785411784*4.5 / 1000.0 / 60.0,
    true, (80 - 32.0) / 1.8);
  //make coolant object
  Fluid kWater;
  Solid copper_pipe;
  copper_pipe.SetConstantProperties();
  kWater.SetConstantProperties("water",
    1000, 0.001, 4186, 0.591);
  //make wax material properties
  Solid wax;
  wax.SetConstantProperties("wax", 950, 2000, 0.25);
  //make precipitation curve
  usr::BigEquation precipitation_eq;
  precipitation_eq.SetId(2);
  precipitation_eq.SetCoefficient({-0.000000000005217221,
                                    0.000000001166511, 
                                   -0.0000000982355,
                                    0.000003649075,
                                   -0.00003596815,
                                   -0.001500418,
                                    0.04093743});
  double t_c_check;
  for (int i = 0; i < 10; i++) {
    t_c_check = (60.0 + 5.0*i - 32.0)/1.8;
    std::cout << t_c_check << "   " << precipitation_eq.At(t_c_check) << '\n';
  }
  SimulationOptions sim_option;
  sim_option.SetDepositionDuration(3600.0*24.0);

  sim_option.SetPrecipitationRate(0.0);
  sim_option.SetAAlpha(0.01); //use in Lee 2008 model
  sim_option.SetAlphaOption(usr::kDepositionOption::Lee2008_k_alpha_eq);
  usr::BigEquation dCdT;
  dCdT.SetId(1);
  dCdT.SetCoefficient({-2.462730E-02, 1.263336E+00});
  SinglePhaseDeposition deposition1;
  double initial_total_oil_volume = 3.785411784 * 30.0 / 1000.0;
  double initial_total_oil_mass = initial_total_oil_volume * 
    oil.GetProperty(usr::kFluid::rho, 
    inlet.GetProperties(usr::kTwoPhaseVar::inlet_temperature));
  deposition1.SetAll(sim_option,
    oil,kWater,inlet,coolant_inlet,pipe,
    copper_pipe);

  std::system("del .\\profile_data\\*.* /Q");
  std::system("del .\\data_1d\\*.* /Q");
  std::system("del .\\data_ave\\*.* /Q");

  deposition1.Calculate();
  deposition1.SimplePostCalculation();
  deposition1.PrintStandardOutput();
  deposition1.DetailedPostCalculation();
  deposition1.PrintDetailedOutput();


}
} //namespace spdepo