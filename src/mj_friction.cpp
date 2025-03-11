/** Please refer to the paper Reliable chattering-free simulation of friction
torque in joints presenting high stiction by Rafael Cisneros */

#include "mj_friction.h"
#include "LambertW/LambertW.h"

#include "LookUpTable/LookUpTable.h"
#include <iostream>
#include <memory>

namespace mc_mujoco
{

double setFrictionForces(JointValSet & set)
{

  double w = set.firstTime ? 0 : (set.value - set.p_prev) / set.dt;

  if(set.firstTime) set.firstTime = !set.firstTime;

  set.w_ast = w + set.Z * set.Kf * set.e;
  set.T_ast = set.w_ast / set.Z;

  double Tf;

  if(set.T_ast > set.Ts)
  {
    Tf = (*set.dryFricTable.get())(set.w_ast) + (set.Tc + set.Tv * set.w_ast) / set.den;
  }
  else if(set.T_ast < -set.Ts)
  {
    Tf = -(*set.dryFricTable.get())(-set.w_ast) + (-set.Tc + set.Tv * set.w_ast) / set.den;
  }
  else
  {
    Tf = set.T_ast;
  }

  set.e = set.Z * (set.Bf * set.e + Tf * set.dt);
  set.p_prev = set.value;

  set.torqueForce -= Tf;

  return set.torqueForce;
}

void createTable(JointValSet & set)
{
  double Tsc = set.Tsc;
  double Tc = set.Tc;
  double Z = set.Z;
  double den = set.den;
  double wbrk = set.wbrk;

  auto dry = [Tsc, Tc, wbrk, Z, den](double w_ast)
  {
    double lambArg = -Z / wbrk * Tsc / den * exp((Z * Tc - w_ast) / (wbrk * den));
    return -(wbrk / Z) * utl::LambertW<0>(lambArg);
  };

  set.dryFricTable = std::make_shared<utl::LookUpTable<double>>(set.min, set.max, set.LUTstep, dry);
}

} // namespace mc_mujoco
