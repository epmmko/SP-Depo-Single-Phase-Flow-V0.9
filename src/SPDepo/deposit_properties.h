#ifndef SPDEPO_DEPOSIT_PROPERTIES_H_
#define SPDEPO_DEPOSIT_PROPERTIES_H_

#include <vector>
#include "user_equation_format.h"
#include "user_flow_enum.h"
#include "fluid_properties.h"

namespace spdepo {
  namespace heat_calculation {
    class MacroZHeatBalance;
    class RadialTProfile;
  } // namespace spdepo
  namespace mass_calculation {
    class RadialCProfile; //forward declaration
  } // namespace mass_calculation

  class DepositProfile {
   public:
    DepositProfile();
    friend class heat_calculation::MacroZHeatBalance;
    friend class heat_calculation::RadialTProfile;
    friend class spdepo::mass_calculation::RadialCProfile;
    void Initialize(int sec_size, usr::BigEquation k_oil_eq, 
      usr::BigEquation k_wax_eq,
      usr::BigEquation rho_oil_eq,
      double pipe_radius,
      double overall_roughness = 0.0);
    //Directly set the private vector variable
    //This does not change dep_ object
    void SetRiVec(const std::vector<double>& ri_vec);
    void SetFwVec(const std::vector<double>& fw_vec);
    void SetMassVec(const std::vector<double>& mass_vec);
    void SetZMesh(const std::vector<double>& zmesh_vec);
    void SetPipeRadius(double pipe_radius);
    double GetKDeposit(int index, double t_celsius) const;
    double CalculateKDep(double t_celsius, double fw) const;
      //Calculate deposit k [W/m/K] by using the input of
      //temperature and wax fraction, based on
      //the internal k_wax and k_oil equation
    double GetRhoDeposit(int index, double t_celsius) const;
    double GetPipeRadius() const;
    double GetTotalWaxMass(const std::vector<double>& tdep_vec) const;
    //directly get from the vector
    std::vector<double> GetDeltaVec() const;
    std::vector<double> GetFwVec() const;
    std::vector<double> GetRiVec() const;
    std::vector<double> GetMassVec() const;

      //take inner pipe radius, rhw as the input
    double GetTotalDepositVolume(const std::vector<double>& z_mesh, double R) const;
      //deposit volume from the profile
    int GetSecSize() const;
   private:
      //for compatibility with MacroHeatBalance,
      //  Single-Phase Hydrodynamics,
      //  and RadialTProfile
    std::vector<double> ri_vec_;   //for single-phase deposition class
    std::vector<double> fw_vec_;   //for single-phase deposition class
    std::vector<double> mass_vec_; //for single-phase deposition class
    std::vector<double> zmesh_vec_;
    double total_wax_mass_;
    usr::BigEquation k_oil_eq_;
    usr::BigEquation k_wax_eq_;
    usr::BigEquation rho_oil_eq_;
    double overall_roughness_ = 0.0; //default must be 0.0
    int sec_size_;
    bool called_initialize_ = false;
    double pipe_radius_;
  };

  class WaxProperties {
  public:
    //597 is for Panacharoensawad (2012) case
    //This number is subject to CCN
    void SetAll(
      const usr::BigEquation& precipitation_curve,
      const usr::BigEquation& dCdT,
      const Fluid& oil,
      double T_coolant_ini_inlet,
      double wax_molar_volume = 597);

    void SetPrecipitationCurve(usr::BigEquation precipitation_curve);
    //[kg/m3] Get relative concentration T is in degree-C
    double GetC(double T);
    
    //[kg/m3] Get dC/dT T is in degree-C
    double GetDCDT(double T);

    //[kg/kg] T is in degree-C
    double GetSolidFrac(double T);
    
    //[m2/s] T is in degree-C
    double CalculateDiffusivity(double T) const;

    //return 1/(1+alpha^2Fw^2/(1-Fw) not D_e, but D_e / D_wo
    double CalculateEffectiveDiffusivityRatio(
      double k_alpha, double fw) const;
    
    

  private:
    usr::BigEquation precipitation_curve_;
    usr::BigEquation dCdT_;
    Fluid oil_;
    double T_coolant_ini_inlet_;
    double rho_c_;
    //rho_oil at cold inlet initial temperature [kg/m3]
    double sf_c_;
    //solid fraction corresponding to rho_c [kg/kg]
    double depositable_wax_molar_volume_; //[cm3/gmol]
  };


} // namespace spdepo

#endif