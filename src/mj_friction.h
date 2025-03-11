#include "LookUpTable/LookUpTable.h"
#include <memory.h>
#include <memory>

namespace mc_mujoco
{

struct JointValSet
{
  /**Joint Value */
  double value;
  double velocity;
  double torqueForce;

  /** Friction parameters*/
  double Ts = 2.5; // static friction
  // double Tc = 0.4; //Coulomb friction
  double Tc = 0.2;
  double Tsc = Ts - Tc;
  double Tv = 4.5; // coefficient of viscous friction
  // double wbrk = 0.06; //break-away angular velocity
  double wbrk = 0.04;
  double Kf = 5000; // spring constant
  double Bf = 50; // damper constant
  double e; // error
  double p_prev;
  double lambArg_th = -0.001;

  double dt = 0.001;

  double Z = 1 / (Kf * dt + Bf);
  double den = 1 + Z * Tv;

  double w_ast; // auxiliary relative velocity
  double T_ast;

  double min = Z * Ts;
  double max = Z * Tc - wbrk * den * log(-wbrk / Z * den / Tsc * lambArg_th);
  double LUTstep = 0.001;

  double firstTime;
  double dry = 0.0;

  std::shared_ptr<utl::LookUpTable<double>> dryFricTable;
};

double setFrictionForces(JointValSet & set);

void createTable(JointValSet & set);

} // namespace mc_mujoco
