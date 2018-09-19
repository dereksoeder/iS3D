#include "viscous_correction.h"
#include <math.h>


Milne_Basis_Vectors::Milne_Basis_Vectors(double ut, double ux, double uy, double un, double uperp, double utperp, double tau)
{
      Xx = 1.0; Xy = 0.0;
      Yx = 0.0; Yy = 1.0;

      double sinhL = tau * un / utperp;
      double coshL = ut / utperp;

      Xt = uperp * coshL;
      Xn = uperp * sinhL / tau;
      Zt = sinhL;
      Zn = coshL / tau;

      if(uperp > 1.e-5) // stops (ux=0)/(uperp=0) nans
      {
        Xx = utperp * ux / uperp;
        Xy = utperp * uy / uperp;
        Yx = - uy / uperp;
        Yy = ux / uperp;
      }
}

dsigma_Vector::dsigma_Vector(double dsigmat_in, double dsigmax_in, double dsigmay_in, double dsigman_in)
{
    dsigmat = dsigmat_in;
    dsigmax = dsigmax_in;
    dsigmay = dsigmay_in;
    dsigman = dsigman_in;
}

void dsigma_Vector::boost_dsigma_to_lrf(Milne_Basis_Vectors basis_vectors, double ut, double ux, double uy, double un)
{
    double Xt = basis_vectors.Xt;
    double Xx = basis_vectors.Xx;
    double Xy = basis_vectors.Xy;
    double Xn = basis_vectors.Xn;

    double Yx = basis_vectors.Yx;
    double Yy = basis_vectors.Yy;

    double Zt = basis_vectors.Zt;
    double Zn = basis_vectors.Zn;

    dsigmat_LRF = dsigmat * ut  +  dsigmax * ux  +  dsigmay * uy  +  dsigman * un;
    dsigmax_LRF = -(dsigmat * Xt  +  dsigmax * Xx  +  dsigmay * Xy  +  dsigman * Xn);
    dsigmay_LRF = -(dsigmax * Yx  +  dsigmay * Yy);
    dsigmaz_LRF = -(dsigmat * Zt  +  dsigman * Zn);

}

void dsigma_Vector::compute_dsigma_max()
{
    dsigma_magnitude = fabs(dsigmat_LRF) + sqrt(dsigmax_LRF * dsigmax_LRF  +  dsigmay_LRF * dsigmay_LRF  +  dsigmaz_LRF * dsigmaz_LRF);
}

void Shear_Tensor::boost_to_lrf(double Xt, double Xx, double Xy, double Xn, double Yx, double Yy, double Zt, double Zn, double tau2)
{
  // pimunu in the LRF: piij = Xi.pi.Xj
  double pixx_LRF = pitt*Xt*Xt + pixx*Xx*Xx + piyy*Xy*Xy + tau2*tau2*pinn*Xn*Xn
          + 2.0 * (-Xt*(pitx*Xx + pity*Xy) + pixy*Xx*Xy + tau2*Xn*(pixn*Xx + piyn*Xy - pitn*Xt));
  double pixy_LRF = Yx*(-pitx*Xt + pixx*Xx + pixy*Xy + tau2*pixn*Xn) + Yy*(-pity*Xy + pixy*Xx + piyy*Xy + tau2*piyn*Xn);
  double pixz_LRF = Zt*(pitt*Xt - pitx*Xx - pity*Xy - tau2*pitn*Xn) - tau2*Zn*(pitn*Xt - pixn*Xn - piyn*Xy - tau2*pinn*Xn);
  double piyy_LRF = pixx*Yx*Yx + piyy*Yy*Yy + 2.0*pixy*Yx*Yy;
  double piyz_LRF = -Zt*(pitx*Yx + pity*Yy) + tau2*Zn*(pixn*Yx + piyn*Yy);
  double pizz_LRF = - (pixx_LRF + piyy_LRF);

  //reset components to LRF components
  pixx = pixx_LRF;
  pixy = pixy_LRF;
  pixz = pixz_LRF;
  piyy = piyy_LRF;
  piyz = piyz_LRF;
  pizz = pizz_LRF;

}

//note this max is frame dependent, make sure pimunu has first been boosted to correct frame first
double Shear_Tensor::compute_max()
{
  // probably want to make LRF variables
  double max = 2.0 * ( fabs(pixx) + fabs(pixy) + fabs(pixz) + fabs(piyy) + fabs(piyz) + fabs(pizz) );
  return max;
}


Shear_Stress_Tensor::Shear_Stress_Tensor(double pitt_in, double pitx_in, double pity_in, double pitn_in, double pixx_in, double pixy_in, double pixn_in, double piyy_in, double piyn_in, double pinn_in)
{
    pitt = pitt_in;
    pitx = pitx_in;
    pity = pity_in;
    pitn = pitn_in;
    pixx = pixx_in;
    pixy = pixy_in;
    pixn = pixn_in;
    piyy = piyy_in;
    piyn = piyn_in;
    pinn = pinn_in;
}


void Shear_Stress_Tensor::boost_shear_stress_to_lrf(Milne_Basis_Vectors basis_vectors, double tau2)
{
    double Xt = basis_vectors.Xt;
    double Xx = basis_vectors.Xx;
    double Xy = basis_vectors.Xy;
    double Xn = basis_vectors.Xn;

    double Yx = basis_vectors.Yx;
    double Yy = basis_vectors.Yy;

    double Zt = basis_vectors.Zt;
    double Zn = basis_vectors.Zn;

    double pixx_LRF = pitt*Xt*Xt + pixx*Xx*Xx + piyy*Xy*Xy + tau2*tau2*pinn*Xn*Xn
            + 2.0 * (-Xt*(pitx*Xx + pity*Xy) + pixy*Xx*Xy + tau2*Xn*(pixn*Xx + piyn*Xy - pitn*Xt));
    double pixy_LRF = Yx*(-pitx*Xt + pixx*Xx + pixy*Xy + tau2*pixn*Xn) + Yy*(-pity*Xy + pixy*Xx + piyy*Xy + tau2*piyn*Xn);
    double pixz_LRF = Zt*(pitt*Xt - pitx*Xx - pity*Xy - tau2*pitn*Xn) - tau2*Zn*(pitn*Xt - pixn*Xn - piyn*Xy - tau2*pinn*Xn);
    double piyy_LRF = pixx*Yx*Yx + piyy*Yy*Yy + 2.0*pixy*Yx*Yy;
    double piyz_LRF = -Zt*(pitx*Yx + pity*Yy) + tau2*Zn*(pixn*Yx + piyn*Yy);
    double pizz_LRF = - (pixx_LRF + piyy_LRF);
}


Baryon_Diffusion_Current::Baryon_Diffusion_Current(double Vt_in, double Vx_in, double Vy_in, double Vn_in)
{
    Vt = Vt_in;
    Vx = Vx_in;
    Vy = Vy_in;
    Vn = Vn_in;
}


void Baryon_Diffusion_Current::boost_baryon_diffusion_to_lrf(Milne_Basis_Vectors basis_vectors, double tau2)
{
    double Xt = basis_vectors.Xt;
    double Xx = basis_vectors.Xx;
    double Xy = basis_vectors.Xy;
    double Xn = basis_vectors.Xn;

    double Yx = basis_vectors.Yx;
    double Yy = basis_vectors.Yy;

    double Zt = basis_vectors.Zt;
    double Zn = basis_vectors.Zn;

    Vx_LRF = - Vt * Xt  +  Vx * Xx  +  Vy * Xy  +  tau2 * Vn * Xn;
    Vy_LRF = Vx * Yx  +  Vy * Yy;
    Vz_LRF = - Vt * Zt  +  tau2 * Vn * Zn;
}


lrf_momentum Rescale_Momentum(lrf_momentum p_mod, double mass_squared, Shear_Stress_Tensor pimunu, Baryon_Diffusion_Current Vmu, double shear_coeff, double bulk_coeff, double diff_coeff, double baryon, double baryon_enthalpy_ratio)
{
    // rescale modified momentum p_mod with matrix Mij
    double E_mod = pmod.E;
    double px_mod = pmod.px;
    double py_mod = pmod.py;
    double pz_mod = pmod.pz;

    // LRF momentum (ideal by default or no viscous corrections)
    double px_LRF = px_mod;
    double py_LRF = py_mod;
    double pz_LRF = pz_mod;

    // LRF shear stress components
    double pixx_LRF = pimunu.pixx_LRF;
    double pixy_LRF = pimunu.pixy_LRF;
    double pixz_LRF = pimunu.pixz_LRF;
    double piyy_LRF = pimunu.piyy_LRF;
    double piyz_LRF = pimunu.piyz_LRF;
    double pizz_LRF = pimunu.pizz_LRF;

    // LRF baryon diffusion components
    double Vx_LRF = Vmu.Vx_LRF;
    double Vy_LRF = Vmu.Vy_LRF;
    double Vz_LRF = Vmu.Vz_LRF;
    double diffusion_factor = diff_coeff * (E_mod * baryon_enthalpy_ratio + baryon);

    // rescale momentum:
    //::::::::::::::::::::::::::::::::::::::::
    // add shear terms
    px_LRF += shear_coeff * (pixx_LRF * px_mod  +  pixy_LRF * py_mod  +  pixz_LRF * pz_mod);
    py_LRF += shear_coeff * (pixy_LRF * px_mod  +  piyy_LRF * py_mod  +  piyz_LRF * pz_mod);
    pz_LRF += shear_coeff * (pixz_LRF * px_mod  +  piyz_LRF * py_mod  +  pizz_LRF * pz_mod);

    // add bulk terms
    px_LRF += bulk_coeff * px_mod;
    py_LRF += bulk_coeff * py_mod;
    pz_LRF += bulk_coeff * pz_mod;

    // add baryon diffusion terms
    px_LRF += diffusion_factor * Vx_LRF;
    py_LRF += diffusion_factor * Vy_LRF;
    pz_LRF += diffusion_factor * Vz_LRF;
    //::::::::::::::::::::::::::::::::::::::::

    // set LRF momentum
    lrf_momentum pLRF;
    pLRF.px = px_LRF;
    pLRF.py = py_LRF:
    pLRF.pz = pz_LRF;
    pLRF.E = sqrt(mass_squared  +  px_LRF * px_LRF  +  py_LRF * py_LRF  +  pz_LRF * pzLRF);

    return pLRF;
}



