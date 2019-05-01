#include <iostream>
#include <fstream>
#include <chrono>
#include<Windows.h>
#include "inlet_condition.h"
#include "pipe_geometry.h"
#include "fluid_properties.h"
#include "velocity_profile.h"
#include "user_flow_enum.h"
#include "temperature_profile.h"
#include "coolant_condition.h"
#include "deposit_properties.h"
#include "debug_deposit_profile.h"
namespace spdepo{



void CheckLibrary3() {
  InletCondition inlet;
  inlet.SetProperties("case0", 4 , 0, (105-32.0)/1.8);
  PipeGeometry pipe;
  pipe.SetProperties("pipe0",0.651*0.0254,0.75*0.0254,
    50.0e-6,0.995*0.0254,50.0e-6,0.0,0.7, 0.84); //0.84
  //Creating South Pelto viscosity function.
  Fluid oil;
  oil.SetConstantProperties("oil", 850, 0.003, 2000, 0.13);
  usr::BigEquation oil_mu, oil_rho;
  usr::Equation mu_0, mu_1,mu_2, rho_0;
  mu_0.SetId(3);
  mu_0.SetCoefficient({0.0, 1000.0, 1.0, 273.15, 0.0});
  mu_1.SetId(2);
  mu_1.SetCoefficient({2.533129,-11.906456,14.023469});
  mu_2.SetId(4);
  mu_2.SetCoefficient({0.001,1.0});
  oil_mu.SetEquationVector({mu_0,mu_1,mu_2});
  oil.SetFunctionalProperties(usr::kFluid::mu,oil_mu);
  rho_0.SetId(1);
  rho_0.SetCoefficient({-0.738, 876.62});
  oil_rho.SetEquationVector({rho_0});
  oil.SetFunctionalProperties(usr::kFluid::rho,oil_rho);
  //debug + test oil properties function
  {
    double degree_c;
    double result_mu, result_rho;
    std::ofstream test_oil_mu_rho;
    test_oil_mu_rho.open("test_oil_mu_rho_debug.csv");
    test_oil_mu_rho << "degree f, degree c, mu[Pa.s], rho[kg/m3]\n";
    for (int degree_f = 80; degree_f < 110; degree_f++) {
      degree_c = (degree_f - 32.0) / 1.8;
      result_mu = oil.GetEquation(usr::kFluid::mu).At(degree_c);
      result_rho = oil.GetEquation(usr::kFluid::rho).At(degree_c);
      test_oil_mu_rho << degree_f << ',' << degree_c << ',' <<
        result_mu << ',' << result_rho << '\n';
    }
    test_oil_mu_rho.close();
  }
  Solid wax;
  wax.SetConstantProperties("wax", 950, 2000, 0.25);
  SinglePhaseHydrodynamics h_flow;
  h_flow.SetAll(oil, pipe, inlet.GetProperties(usr::kTwoPhaseVar::v_sl),
                inlet);
  h_flow.DiscretizeR(20);
  h_flow.Solve(20, 0, pipe.GetProperties(usr::kGeometry::eps_p)); //solve at 20 C
  std::cout << h_flow;
  std::ofstream v_dis_debug;
  v_dis_debug.open("v_dis_debug.csv");
  h_flow.PrintRVProfile(v_dis_debug);
  v_dis_debug.close();
  
  Fluid kWater;
  kWater.SetConstantProperties("water",
    1000, 0.001, 4186, 0.591);
  Solid copper_pipe;
  copper_pipe.SetConstantProperties();
  CoolantCondition coolant_inlet;
  coolant_inlet.SetAll(3.785411784*4.5 / 1000.0 / 60.0,
    true, (84-32.0)/1.8);
  heat_calculation::MacroZHeatBalance t_z_profile;
  t_z_profile.SetAll(h_flow, coolant_inlet, kWater, copper_pipe);
  DepositProfile deposit;
  usr::BigEquation k_oil;
  usr::BigEquation k_wax;
  k_wax = wax.GetEquation(usr::kSolid::k);
  k_oil = oil.GetEquation(usr::kFluid::k);
  t_z_profile.ZDiscretization();
  int no_section = t_z_profile.GetSecSize();
  deposit.Initialize(t_z_profile.GetSecSize(), 
    k_oil, k_wax, oil_rho, pipe.GetProperties(usr::kGeometry::d)/2.0);
  //set deposit thickness profile for debugging T profile purposes
  //non-linear case
  //= ((0.002 - (5 - H2) ^ 2 / 121) + 1)*H2 / 1000 * EXP(-H2 / (10 / LN(100)))

  DepositProfile deposit2(deposit);
  TestDepositProfile profile_generator;
  profile_generator.SetProfile(&deposit2,1);
  t_z_profile.Solve(deposit);
  t_z_profile.HeatBalanceVerification();
  std::ofstream t_profile_debug;
  t_profile_debug.open("tprofile_debug.csv");
  t_z_profile.PrintTSectionProfile(t_profile_debug);
  t_profile_debug.close();
  std::vector<double> z_mesh;
  t_z_profile.GetProfile(usr::kProfile::z_mesh, &z_mesh);
  heat_calculation::RadialTProfile t_radial;
  t_radial.SetAll(t_z_profile, h_flow);
  t_radial.ZDepositGrouping();

  t_radial.PrintParameters(std::cout,usr::kRadialTProfile::z_group_);
  t_radial.Solve();

  t_radial.StandardOutput();
  t_radial.PlotPColorGraph();
  t_radial.HeatBalanceVerification();
  //Debug Test Runtime for different parameter passing method
  {
    std::string name_golf1;
    std::string name_golf2;
    std::string name_golf3;
    std::cout << "1000 oil.GetName(&name_golf1) time[microsecond]: ";
    auto start_time_g1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
      oil.GetName(&name_golf1);
    }
    auto end_time_g1 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
      end_time_g1 - start_time_g1).count() << '\n';

    std::cout << "1000 name_golf2 = oil.GetName2() time[microsecond]: ";
    auto start_time_g2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
      name_golf2 = oil.GetName2();
    }
    auto end_time_g2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
      end_time_g2 - start_time_g2).count() << '\n';

    std::cout << "1000 name_golf3 = oil.GetName3(name_golf3) time[microsecond]: ";
    auto start_time_g3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
      oil.GetName3(name_golf3);
    }
    auto end_time_g3 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
      end_time_g3 - start_time_g3).count() << '\n';

    std::cout << name_golf1 << '\n';
    std::cout << name_golf2 << '\n';
    std::cout << name_golf3 << '\n';
  }
}
} //namespace spdepo